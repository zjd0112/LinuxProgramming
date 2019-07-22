#include <iostream>
#include <vector>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>


using namespace std;

#define BUFFSIZE 1024

/** 
 * struct sockaddr_in {
 *      short int sin_family;           // 协议族
 *      unsigned short int sin_port;    // 端口号（网络字节序）
 *      struct in_addr sin_addr;        // IP地址
 *      unsigned char sin_zero[8];      // 为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节
 * }
 */

struct PACKET_HEAD {
    int length;
};

class Server {
private:
    struct sockaddr_in server_addr;
    socklen_t server_addr_len;
    int listen_fd;                  // 监听fd
    int max_fd;                     // 监听fd的最大值
    fd_set master_set;              // fd集合，包括监听fd和客户端fd
    fd_set working_set;             // 
    struct timeval timeout;

public:
    Server(int port);
    ~Server();
    void Bind();
    void Listen(int queue_len = 20);
    void Accept();
    void Run();
    void Recv(int nums);
};

Server::Server(int port) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;   // IPV4 协议族
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // create socket to listen
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        cout << "Create Listen Socket failed" << endl;
        exit(1);
    }
    // 设置socket状态
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

Server::~Server() {
    for (int fd = 0; fd < max_fd; fd++) {
        // FD_ISSET: 判断指定描述符是否在集合中
        if (FD_ISSET(fd, &master_set)) {
            close(fd);
        }
    }
}

void Server::Bind() {
    int res = bind(listen_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (res < 0) {
        cout << "Server bind failed" << endl;
        exit(1);
    }
    cout << "Server bind Successfully" << endl;
}

void Server::Listen(int queue_len) {
    int res = listen(listen_fd, queue_len);
    if (res < 0) {
        cout << "Server listen failed" << endl;
        exit(1);
    }
    cout << "Server listen successfully" << endl;
}

void Server::Accept() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int new_fd = accept(listen_fd, (struct sockaddr*) &client_addr, &client_addr_len);

    if (new_fd < 0) {
        cout << "Server accept failed" << endl;
        exit(1);
    }
    cout << "New connection was accepted" << endl;

    // 将新建立的连接加入master_set
    FD_SET(new_fd, &master_set);
    if (new_fd > max_fd) {
        max_fd = new_fd;
    }
}

void Server::Run() {
    
    max_fd = listen_fd;     // 初始化max_fd
    FD_ZERO(&master_set);   // 置空master_set
    FD_SET(listen_fd, &master_set);     // 将listen_fd 加入master_fd

    while (true) {
        FD_ZERO(&working_set);
        memcpy(&working_set, &master_set, sizeof(master_set));
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;

        // timeout: 指定等待时间。NULL：一直等待；0：不等待，遍历一遍直接返回
        int nums = select(max_fd + 1, &working_set, NULL, NULL, &timeout);

        // select返回，用FD_ISSET测试给定位是否置位
        if (nums < 0) {
            cout << "select() error" << endl;
            exit(1);
        }
        if (nums == 0) {
            continue;
        }
        // 判断listen_fd是否可读，可读代表有新的客户端请求
        if (FD_ISSET(listen_fd, &working_set)) {
            Accept();   //有新的客户端请求
        }
        else {
            Recv(nums); //接收客户端消息 
        }
    }

}

void Server::Recv(int nums) {
    // 遍历，找到可读的fd
    for (int fd = 0; fd <= max_fd; fd++) {
        if (FD_ISSET(fd, &working_set)) {
            bool close_conn = false;    //  标记当前连接是否断开
            PACKET_HEAD head;
            recv(fd, &head, sizeof(head), 0);   // 先收包头
            char* buffer = new char[head.length];
            memset(buffer, 0, sizeof(buffer));
            int total = 0;
            while (total < head.length) {
                int len = recv(fd, buffer + total, head.length - total, 0);
                if (len < 0) {
                    cout << "recv error" << endl;
                    close_conn = true;
                    break;
                }
                total = total + len;
            }
            // 接收数据完毕， 将收到的消息发给客户端
            if (total == head.length) {
                int ret1 = send(fd, &head, sizeof(head), 0);
                int ret2 = send(fd, buffer, head.length, 0);
                if (ret1 < 0 || ret2 < 0) {
                    cout << "send error" << endl;
                    close_conn = true;
                }
            }
            delete buffer;
            
            // 连接有问题， 关闭
            if(close_conn) {
                close(fd);
                cout << "close connection" << endl;
                // 从侦听集合中清除
                FD_CLR(fd, &master_set);
                // 更新max_fd
                if (fd == max_fd) {
                    while (FD_ISSET(max_fd, &master_set) == false) {
                        max_fd--;
                    }
                }
            }
        }
    }
}

int main() {
    Server server(15000);
    server.Bind();
    server.Listen();
    server.Run();
}