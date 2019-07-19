#ifndef _COMM_
#define _COMM_

#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

#define SIZE 4096

class SharedMem {

    private:
        int shmid;
        int commShm(int flags);

    public:
        string path_name = ".";
        int proj_id = 0x6666;
        
        SharedMem();
        ~SharedMem();
        int createShm();
        int getShm();
        char* Shmat(const void* shmaddr, int shmflg);
        int Shmdt(const void *shmaddr);
        int destroyShm();
};

#endif
