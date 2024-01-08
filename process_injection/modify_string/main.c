#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <wait.h>
#include <stdint.h>
#include <string.h>

int main() {
    pid_t pid = 30698;
    char *p = "0x55c1e3bfd2a0";
    char *m = "mundo\0";
    int mlen = (int)strlen(m);

    /*
    The address is equal to word size of the machine, 
    so it may be 64bits long.
    */
    unsigned long long address = strtoull(p, NULL, 16);
    
    printf("Remote data pointer: %p\n", (void *)address);
    printf("Local data pointer:  %p\n", (void *)m);

    if ((ptrace(PTRACE_ATTACH, pid, NULL,NULL)) != 0) {
        printf("Error: Problems Attaching\n");
        return 1;
    }
    int waitxxx;
    if (waitpid(pid, &waitxxx, WUNTRACED) != pid) {
        printf("Error: Waiting proc %d\n",pid);
        return 1;
    }

    for (int i = 0; i < mlen; i++, address++,m++) {
        printf("Write %p <- %p: %c\n",(void *)address, m, *(int *)m);
        if (ptrace(PTRACE_POKEDATA, pid, (void *)(address), *m) != 0) {
            printf("Error: Problems writting data\n");
        }
    }

    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) != 0) {
        printf("Error: Problems deataching regs.\n");
        return 1;
    }
}