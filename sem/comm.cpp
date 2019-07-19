#include "comm.h"

Sem::Sem() {

}

Sem::~Sem() {

}

int Sem::commSemSet(int nums, int flags) {
    key_t k = ftok(path_name.c_str(), proj_id);
    semid = semget(k, nums, flags);

    if (semid < 0) {
        perror("semget error");
        return -1;
    }
    return 0;
}

int Sem::createSemSet(int nums) {
    return commSemSet(nums, IPC_CREAT | IPC_EXCL | 0666);
}

int Sem::getSemSet() {
    return commSemSet(0, 0);
}

int Sem::initSem(int which) {
    union semun u;

    u.val = 1;
    int ret = semctl(semid, which, SETVAL, u);
    if (ret < 0) {
        perror("semctl error");
        return -1;
    }
    return 0;
}

int Sem::semOP(int which, int op) {
    struct sembuf s;
    s.sem_num = which;
    s.sem_op = op;
    s.sem_flg = 0;
    int ret = semop(semid, &s, 1);
    if (ret < 0) {
        perror("semop error");
        return -1;
    }
    return 0;
}

int Sem::P_op(int which) {
    return semOP(which, -1);
}

int Sem::V_op(int which) {
    return semOP(which, 1);
}

int Sem::destroySemSet() {
    int ret = semctl(semid, 0, IPC_RMID);
    if (ret < 0) {
        perror("semctl error");
        return -1;
    }
    return 0;
}