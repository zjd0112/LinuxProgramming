#include <unistd.h>
#include <sys/wait.h>
#include "comm.h"

int main() {
    Sem* mySem = new Sem();

    mySem->createSemSet(1);
    mySem->initSem(0);
    pid_t id = fork();

    if (id == 0) {
        // child
        mySem->getSemSet();
        printf("child is running. pid:%d, ppid:%d\n", getpid(), getppid());
        while(1) {
            mySem->P_op(0);
            printf("A");
            usleep(10040);
            fflush(stdout);
            printf("A");
            usleep(1000);
            fflush(stdout);
            mySem->V_op(0);
        }
    }
    else {
        // parent
        printf("father is running. pid:%d, ppid:%d\n", getpid(), getppid());
        while(1) {
            mySem->P_op(0);
            printf("B");
            usleep(10040);
            fflush(stdout);
            printf("B");
            usleep(1000);
            fflush(stdout);
            mySem->V_op(0);
        }
        wait(NULL);
    }
    mySem->destroySemSet();
    return 0;
}