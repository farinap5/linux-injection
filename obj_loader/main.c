#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef union objhdr {
  const Elf64_Ehdr *hdr;
  const uint8_t *data;
} objhdr;

/*
  Round up to the next multiple of the page size
*/
static inline uint64_t page_align(uint64_t n, uint64_t page_size) {
  return (n + (page_size - 1)) & ~(page_size - 1);
}

Elf64_Shdr *lookup_section(const char *name, objhdr objhdr,
                           const char *shstrtab, Elf64_Shdr *sections) {
  size_t name_len = strlen(name);
  /*
   * Pass over each entry in sections table.
   */
  for (int i = 0; i < objhdr.hdr->e_shnum; i++) {
    /*
     * The sh_name is the offset of the string that names the sections
     * We must sum with shstrtab to get the string.
     */
    const char *sec_name = shstrtab + sections[i].sh_name;
    size_t sec_name_l = strlen(sec_name);
    /*
     * If the len is the same and the string too, return the section.
     */
    if (name_len == sec_name_l && !strcmp(name, sec_name)) {
      return &sections[i];
    }
  }
  return NULL;
}

void *lookup_entrypont(char *name, int symbles_n, Elf64_Sym *symbols, char *strtab, uint8_t *text) {
  size_t name_len = strlen(name);
  
  for (int i = 0; i < symbles_n; i++) {
    if (ELF64_ST_TYPE(symbols[i].st_info) == STT_FUNC) {
      /*
      * The st_name is the offset of the string that names the function
      * We must sum with strtab (where the string is) to get the string.
      */
      char *f_name = strtab + symbols[i].st_name;
      size_t f_name_len = strlen(f_name);
      if (f_name_len == name_len && !strcmp(name, f_name)) {
        /*
          The st_value is the offset from the beginning of .text section.
        */
        return text + symbols[i].st_value;
      }
    }
  }
  return NULL;
}


int main() {
  FILE *obj_f = fopen("obj.o", "r");

  if (obj_f == 0) {
    printf("Error 1");
    return 1;
  }

  int fd = fileno(obj_f);
  struct stat stt;

  if (fstat(fd, &stt)) {
    printf("Error 2");
    return 1;
  }

  objhdr objhdr;
  objhdr.data = mmap(NULL, stt.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (objhdr.data == MAP_FAILED) {
    printf("Error 3");
    return 1;
  }

  /*
      Get the address of objhdr.data, sum it with the offset of header sections
      based on the objfile, and return as an elf header.
  */
  Elf64_Shdr *sections = (Elf64_Shdr *)(objhdr.data + objhdr.hdr->e_shoff);

  /*
      e_shstrndx holds the section string table. shstrtab
  */
  uint8_t *shstrtab = (uint8_t *)(objhdr.data + sections[objhdr.hdr->e_shstrndx].sh_offset);

  /*
    Lookup for the .symtab, .strtab and .text.
  */
  Elf64_Shdr *symtab_hdr = lookup_section(".symtab", objhdr, shstrtab, sections);
  Elf64_Shdr *strtab_hdr = lookup_section(".strtab", objhdr, shstrtab, sections);
  Elf64_Shdr *text_hdr   = lookup_section(".text", objhdr, shstrtab, sections);

  if (!symtab_hdr || !strtab_hdr || !text_hdr) {
    printf("Error 4");
    return 1;
  }
  char *sec_name = shstrtab + symtab_hdr->sh_name;
  //printf("%s\n", sec_name);

  /* 
    Extract offset for the symbols table.
  */
  Elf64_Sym *symbols = (Elf64_Sym *)(objhdr.data + symtab_hdr->sh_offset);
  
  /*
    The section size in bytes is divided by the size of the section.
    The section has fixed size.
  */
  int symbols_num = symtab_hdr->sh_size / symtab_hdr->sh_entsize;

  char *strtab = (char *)(objhdr.data + strtab_hdr->sh_offset);

  uint16_t page_size = sysconf(_SC_PAGESIZE);



  uint8_t *text = mmap(NULL, page_align(text_hdr->sh_size, page_size), PROT_READ | PROT_WRITE, MAP_PRIVATE | 0x20, -1, 0);
  if (text == MAP_FAILED) {
    printf("Error 5");
    return 1;
  }
  memcpy(text, objhdr.data + text_hdr->sh_offset, text_hdr->sh_size);
  mprotect(text, page_align(text_hdr->sh_size, page_size), PROT_READ | PROT_EXEC);



  // Run a function
  int (*add)(int);
  add = lookup_entrypont("add", symbols_num, symbols, strtab, text);
  if (!add) {
    printf("Error 6");
    return 1;
  }

  printf("add(%d) = %d\n", 5, add(5));

  return 0;
}
