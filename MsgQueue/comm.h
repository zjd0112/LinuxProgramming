#ifndef _COMM_
#define _COMM_

#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>
#include <string.h>

using namespace std;

#define MYSIZE 128
#define SERVER_TYPE 1
#define CLIENT_TYPE 2


struct MsgBuf {
    long mtype;
    char mtext[MYSIZE];
};


class MsgQueue {

    private:
        int msgid;

        int commMsgQueue(int flags);

    public:
        string path_name = ".";
        int proj_id = 0x6666;
        
        MsgQueue();
        ~MsgQueue();
        void createMsgQueue();
        void getMsgQueue();
        int sendMsg(long type, string msg);
        int recvMsg(int type, char out[]);
        int destroyMsgQueue();
};

#endif
