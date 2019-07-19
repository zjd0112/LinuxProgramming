#include "comm.h"

int main() {
    char buf[2*MYSIZE];
    MsgQueue* myMsgQueue = new MsgQueue();
    myMsgQueue->createMsgQueue();

    while (1) {
        if (myMsgQueue->recvMsg(CLIENT_TYPE, buf) < 0) {
            break;
        }
        else {
            cout << "client: " << buf << endl;
        }
        if (myMsgQueue->sendMsg(SERVER_TYPE, buf) < 0) {
            break;
        }
    }
    myMsgQueue->destroyMsgQueue();
    delete myMsgQueue;
    return 0;
}