#include "comm.h"

int main() {
    MsgQueue* myMsgQueue = new MsgQueue();

    myMsgQueue->getMsgQueue();  // 创建一个新的消息队列
    char out[2*MYSIZE];
    char buf[MYSIZE];

    while(1) {
        cout << "please enter:" << endl;
        fflush(stdout);
        ssize_t s = read(0, buf, sizeof(buf) - 1);

        if (s > 0) {
            buf[s] = '\0';
            myMsgQueue->sendMsg(CLIENT_TYPE, buf);
        }
        if (myMsgQueue->recvMsg(SERVER_TYPE, out) < 0) {
            break;
        }
        cout << "server: " << out << endl;
    }
    delete myMsgQueue;
    return 0;
}