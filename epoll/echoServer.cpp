#include <iostream>
#include <vector>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>


using namespace std;

#define BUFFSIZE 1024
#define EPOLLSIZE 100

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
    int epoll_fd;                   // epoll fd
    struct epoll_event events[EPOLLSIZE];   // 存储epoll_wait返回的就绪事件

public:
    Server(int port);
    ~Server();
    void Bind();
    void Listen(int queue_len = 20);
    void Accept();
    void Run();
    void Recv(int fd);
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

    // 在epoll中注册新建立的连接
    struct epoll_event event;
    event.data.fd = new_fd;
    event.events = EPOLLIN;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &event);
}

void Server::Run() {
    epoll_fd = epoll_create(1);     // 创建epoll句柄
    struct epoll_event event;
    event.data.fd = listen_fd;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);    // 注册 listen fd

    while (true) {
        int nums = epoll_wait(epoll_fd, events, EPOLLSIZE, -1);     // 超时时间设置为-1， 永久阻塞
        if (nums < 0) {
            cout << "epoll wait failed" << endl;
            exit(1);
        }

        if (nums == 0) {
            continue;
        }

        // 遍历所有就绪事件
        for (int i = 0; i < nums; i++) {
            int fd = events[i].data.fd;
            
            // 如果fd是listen fd，有新的客户端请求
            if (fd == listen_fd && events[i].events & EPOLLIN) {
                Accept();
            }
            // 否则，读数据
            else if (events[i].events & EPOLLIN) {
                Recv(fd);
            }
        }

    }
}

void Server::Recv(int fd) {
    
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
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN;

        // 删除
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event);    
    }   
}

int main() {
    Server server(15000);
    server.Bind();
    server.Listen();
    server.Run();
}
