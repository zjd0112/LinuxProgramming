#ifndef _COMM_
#define _COMM_

#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string>

using namespace std;

union semun {
    int val;
    struct semid_ds *buf;
    struct seminfo *__buf;
};

class Sem {

    private:
        int semid;
        int commSemSet(int nums, int flags);
        int semOP(int which, int op);

    public:
        string path_name = ".";
        int proj_id = 0x6666;

        Sem();
        ~Sem();
        int createSemSet(int nums);
        int initSem(int which);
        int getSemSet();
        int P_op(int which);
        int V_op(int which);
        int destroySemSet();
};

#endif