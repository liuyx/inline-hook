#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <elf.h>

#include <dlfcn.h>

struct libc {
	long start, stop;
	char *path;
} libc;

#define libc_path "/lib/i386-linux-gnu/libc-2.19.so"
#define INVALID_ADDR -1

Elf32_Addr resolve_symbol(char *name, uint8_t *target) {
    Elf32_Ehdr *ehdr;
    Elf32_Shdr *shdr;

    ehdr = (Elf32_Ehdr *)target;
    shdr = (Elf32_Shdr *)(target + ehdr->e_shoff);

    int i,j;
    Elf32_Sym *symtab;
    char *shsymtab;

    for (i = 0; i < ehdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM) {
            symtab = (Elf32_Sym *)(target + shdr[i].sh_offset);
            shsymtab = (char *)&target[shdr[shdr[i].sh_link].sh_offset];
            //shsymtab = (char *)get_section_index(shdr[i].sh_link, target);
            for (j = 0; j < shdr[i].sh_size / sizeof(Elf32_Sym); j++, symtab++) {
                if (strcmp(&shsymtab[symtab->st_name],name) == 0) {
                    puts("bingo");
                    return symtab->st_value;
                }
            }
        }
    }
}

unsigned long get_lib_path(pid_t pid) {
#define BUF_SIZE 256
    FILE *fp;
    char buf[BUF_SIZE], file[BUF_SIZE];
    char *p, *q;
    long start, stop;

    //snprintf(file,sizeof(file), "/proc/self/maps",(int)pid);
    snprintf(file,sizeof(file), "/proc/self/maps");

    if ( (fp = fopen(file,"r")) == NULL) {
        perror("fopen");
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if (strstr(buf,"libc") != NULL && strstr(buf, ".so") != NULL) {
            p = strchr(buf, '-');
            start = strtoul(buf, NULL, 16);

            stop = strtoul(p + 1, NULL, 16);

            while(*p++ != '/')
                ;

            *(char *)strchr(p, '\n') = '\0';

            libc.start = start;
            libc.stop = stop;
            libc.path = strdup(p - 1);

            return start;
        }
    }

    return 0;
}

Elf32_Addr get_sym_from_libc(pid_t pid,const char *name) {
    int fd, i;
    struct stat st;
    Elf32_Addr libc_base_addr = get_lib_path(pid);
    Elf32_Addr symaddr;

    if ( (fd = open(libc_path, O_RDONLY)) < 0) {
        perror("open libc");
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        perror("fstat libc");
        return -1;
    }

    uint8_t *libcp = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (libcp == MAP_FAILED) {
        perror("mmap libc");
        return -1;
    }

    symaddr = resolve_symbol((char *)name, libcp);
    if (symaddr == 0) {
        printf("[!] resolve_symbol failed for symbol '%s'\n", name);
        printf("Trying using --manual-elf-loading option\n");
        return -1;
    }
    symaddr = symaddr + libc_base_addr;

    return symaddr;
}

int main(int argc, char **argv) {
	long libc_addr = get_lib_path(1);
	printf("start = %lx, stop = %lx\n",libc.start, libc.stop);
	Elf32_Addr addr = get_sym_from_libc(1,"puts");
	printf("%p\n",(void *)addr);
}
