#include <stdio.h>
#include <string.h>

void * memmap() {
    /*
        mov    edi,0x0
        mov    esi,0x400
        mov    edx,0x7
        mov    r10d,0x22
        mov    r9d,0x0
        mov    eax,0x9
        syscall
    */
    register int rdi asm("rdi") = 0; // os will choose mapping destination
    register int rsi asm("rsi") = 1024;
    register int rdx asm("rdx") = 0x07;
    register int r10 asm("r10") = 0x20 | 0x02;
    register int r9 asm("r9") = 0x00;
    register int al asm("al") = 0x09;
    asm("syscall");
}

void memmap_end(){}

int main() {
    char *a = memmap();
    printf("0x%x\n",a);

    int memmap_size = (int)((long)memmap_end - (long)memmap);
    void *ptr = memmap;

    printf("%d\n", memmap_size);

    for (int i = 0; i <= memmap_size; i++) {
        unsigned char *p = ptr+i;
        printf("%p - %x\n", p, *p);
    }
    return 0;
}

/*int main() {
    char *any = "aaa";
    int x;

    register int rax asm("rax") = 5;

    void * buf = mmap(NULL, 1024, 0x1 | 0x2 | 0x3, 0x20 | 0x02, -1, 0);
    if (buf == 0xffffffff) {
        fprintf(stderr, "mmap() failed.\n");
        return 1;
    }
    printf("0x%x\n",buf);

    strcpy(buf, any);

    printf("%s\n",buf);
    scanf("%d",&x);

    
    return 0;
}*/