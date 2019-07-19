#include <unistd.h>
#include "comm.h"

int main() {
    SharedMem* mySharedMem = new SharedMem();
    mySharedMem->createShm();
    char* mem = mySharedMem->Shmat(NULL, 0);

    int i = 0;
    while(1) {
        sleep(1);
        mem[i++] = 'A';
        i = i % (SIZE - 1);
        mem[i] = '\0';
    }
    mySharedMem->Shmdt(mem);
    mySharedMem->destroyShm();
    return 0;
}