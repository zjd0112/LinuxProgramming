#include <unistd.h>

#include "comm.h"

int main() {
    SharedMem* mySharedMem = new SharedMem();
    mySharedMem->getShm();
    char* mem = mySharedMem->Shmat(NULL, 0);
    while(1) {
        printf("%s\n", mem);
        fflush(stdout);
        sleep(1);
    }
    mySharedMem->Shmdt(mem);
    return 0;
}
