#include "comm.h"

MsgQueue::MsgQueue() {

}

MsgQueue::~MsgQueue() {

}

int MsgQueue::commMsgQueue(int flags) {
    key_t k = ftok(path_name.c_str(), proj_id);
    if (k < 0) {
        perror("ftok error");
        return -1;
    }

    msgid = msgget(k, flags);
    if (msgid < 0) {
        perror("msgget error");
        return -1;
    }
}

void MsgQueue::createMsgQueue() {
    msgid = commMsgQueue(IPC_CREAT | IPC_EXCL | 0666);
}

void MsgQueue::getMsgQueue() {
    msgid =  commMsgQueue(IPC_CREAT);
}

int MsgQueue::sendMsg(long type, string msg) {
    struct MsgBuf buf;
    
    buf.mtype = type;
    strncpy(buf.mtext, msg.c_str(), MYSIZE);
    
    int ret = msgsnd(msgid, &buf, sizeof(buf.mtext), 0);

    if (ret < 0) {
        perror("msgsnd");
        return -1;
    }
    return 0;
}

int MsgQueue::recvMsg(int type, char out[]) {
    struct MsgBuf buf;
    
    int ret = msgrcv(msgid, &buf, sizeof(buf.mtext), type, 0);
    if (ret > 0) {
        strncpy(out, buf.mtext, ret);
        return 0;
    }
    perror("msgrcv");
    return -1;
}

int MsgQueue::destroyMsgQueue() {
    if (msgctl(msgid, IPC_RMID, NULL) < 0) {
        perror("msgctl");
        return -1;
    }
    return 0;
}