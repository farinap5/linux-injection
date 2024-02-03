#include <stdio.h>
#include <string.h>

void *memmap() {
    /*
        mov    edi,0x0
        mov    esi,0x400
        mov    edx,0x7
        mov    r10d,0x22
        mov    r9d,0x0
        mov    eax,0x9
        syscall

    Complete Function:
        push   rbp
        mov    rbp,rsp
        
        mov    edi,0x0
        mov    esi,0x400
        mov    edx,0x7
        mov    r10d,0x22
        mov    r9d,0x0
        mov    eax,0x9
        syscall
        mov    eax,0x0

        pop    rbp
        ret
    */
   __uint8_t mmap[] = { 0x55, 0x48, 0x89, 0xE5, 0xBF, 0x00, 0x00, 0x00, 
   0x00, 0xBE, 0x00, 0x04, 0x00, 0x00, 0xBA, 0x07, 
   0x00, 0x00, 0x00, 0x41, 0xBA, 0x22, 0x00, 
   0x00, 0x00, 0x41, 0xB9, 0x00, 0x00, 0x00, 
   0x00, 0xB8, 0x09, 0x00, 0x00, 0x00, 0x0F, 
   0x05, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x5D, 0xC3 }; 
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
        //printf("%p - %x\n", p, *p);
        printf("%x",*p);
    }
    return 0;
}