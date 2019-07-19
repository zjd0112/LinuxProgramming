#include "comm.h"

SharedMem::SharedMem() {

}

SharedMem::~SharedMem() {

}

int SharedMem::commShm(int flags) {
    key_t k = ftok(path_name.c_str(), proj_id);
    if (k < 0) {
        perror("ftok error");
        return -1;
    }

    shmid = shmget(k, SIZE, flags);
    if (shmid < 0) {
        perror("shmget error");
        return -1;
    }
    return 0;
}

int SharedMem::createShm() {
    return commShm(IPC_CREAT | IPC_EXCL | 0666);
}

int SharedMem::getShm() {
    return commShm(IPC_CREAT);
}

char* SharedMem::Shmat(const void* shmaddr, int shmflg) {
    return (char*)shmat(shmid, shmaddr, shmflg);
}

int SharedMem::Shmdt(const void* shmaddr) {
    return shmdt(shmaddr);
}

int SharedMem::destroyShm() {
    int ret = shmctl(shmid, IPC_RMID, NULL);
    if (ret < 0) {
        perror("shmctl error");
        return -1;
    }
    return 0;
}