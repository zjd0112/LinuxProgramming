#include <iostream>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

struct PACKET_HEAD {
    int length;
};

class Client {

private:
    struct sockaddr_in server_addr;
    socklen_t server_addr_len;
    int fd;

public:
    Client(string ip, int port);
    ~Client();
    void Connect();
    void Send(string str);
    string Recv();
};

Client::Client(string ip, int port) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) == 0) {
        cout << "Server IP Address Error" << endl;
        exit(1);
    }
    server_addr.sin_port = htons(port);
    server_addr_len = sizeof(server_addr);
    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        cout << "Create Socket Failed" << endl;
        exit(1);
    }
}

Client::~Client() {
    close(fd);
}

void Client::Connect() {
    cout << "Connecting......" << endl;
    if (connect(fd, (struct sockaddr*) &server_addr, server_addr_len) < 0) {
        cout << "Can not connect to server IP" << endl;
        exit(1);
    }
    cout << "connect to server successfully" << endl;
}

void Client::Send(string str) {
    PACKET_HEAD head;
    head.length = str.size() + 1;
    int ret1 = send(fd, &head, sizeof(head), 0);
    int ret2 = send(fd, str.c_str(), head.length, 0);
    if (ret1 < 0 || ret2 < 0) {
        cout << "Send message fail" << endl;
        exit(1);
    }
}

string Client::Recv() {
    PACKET_HEAD head;
    
    recv(fd, &head, sizeof(head), 0);

    char* buffer = new char[head.length];
    memset(buffer, 0, head.length);
    int total = 0;
    while (total < head.length) {
        int len = recv(fd, buffer + total, head.length - total, 0);
        if (len < 0) {
            cout << "recv error" << endl;
            break;
        }
        total = total + len;
    }
    string result(buffer);
    delete buffer;
    return result;
}

int main() {
    Client client("127.0.0.1", 15000);
    client.Connect();

    while (true) {
        string msg;
        getline(cin, msg);
        if (msg == "exit") {
            break;
        }
        client.Send(msg);
        cout << client.Recv() << endl;
    }
    return 0;
}