#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <wait.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char *argv[]) {
    pid_t pid = 30698;
    char *p = "0x55c1e3bfd2a0";
    char *m = "mundo\0";
    int v = 0;

    unsigned long long address = strtoull(p, NULL, 16);

    for (int i=0; i < argc; i++) {
        if (strcmp(argv[i],"-h") == 0) {
            printf("Type: ./a.out -p 18893 -v 2 -a 0x55c1e3bfd2a0 -m mundo\n");
            return 0;
        } else if (strcmp(argv[i],"-p") == 0) {
            i++;
            pid = (pid_t)atoi(argv[i]);
        }else if (strcmp(argv[i],"-a") == 0) {
            i++;
            address = 0x0;
            address = strtoull(argv[i], NULL, 16);
        }else if (strcmp(argv[i],"-m") == 0) {
            i++;
            m = argv[i];
        } else if (strcmp(argv[i],"-v") == 0) {
            i++;
            v = atoi(argv[i]);
            if (v < 0 || v > 2) {
                printf("Error: vebosity must be >= 0 and <= 2\n");
                return 1;
            }
        }
    }


    int mlen = (int)strlen(m);
    
    if (v > 0) {
        printf("Remote data pointer: %p\n", (void *)address);
        printf("Local data pointer:  %p\n", (void *)m);
    }
    if (v > 1) {
        printf("Data: %s\n", m);
    }

    if (v > 0)
        printf("Attaching pid: %d\n", (int)pid);
    if ((ptrace(PTRACE_ATTACH, pid, NULL,NULL)) != 0) {
        if (v > 0)
            printf("Error: Problems Attaching\n");
        return 1;
    }
    int waitxxx;
    if (waitpid(pid, &waitxxx, WUNTRACED) != pid) {
        if (v > 0)
            printf("Error: Waiting proc %d\n",pid);
        return 1;
    }

    for (int i = 0; i < mlen; i++, address++,m++) {
        if (v > 1)
            printf("Write %p <- %p: %c\n",(void *)address, m, *(int *)m);
        if (ptrace(PTRACE_POKEDATA, pid, (void *)(address), *m) != 0) {
            if (v > 0) {
                printf("Error: Problems writting data.\n");
            }
        }
    }

    if (v > 0)
        printf("Deataching pid: %d\n", (int)pid);
    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) != 0) {
        if (v > 0) {
            printf("Error: Problems deataching pid.\n");
        }
        return 1;
    }
}