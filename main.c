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

#define libc_path "/lib/i386-linux-gnu/libc-2.19.so"
#define LIB_TEST "/home/liuyouxue/work/hook/hook/libtest.so"

#include "hook.h"
#include "test.h"

#define INVALID_ADDR ((void *)-1)

static int lib_base_addr;

static int my_strcmp(const char *s1, const char *s2) {
	printf("your input, s1 = %s, s2 = %s\n", s1, s2);
	return 0;
}

static int my_strlen(const char *s) {
	return 0;
}

static int my_add(int a, int b) {
	if (unhook(add) == 0) {
		return add(a * 2, b);
	}
	return 0;
}

struct libc {
	long start, stop;
	char *path;
} libc;

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
            for (j = 0; j < shdr[i].sh_size / sizeof(Elf32_Sym); j++, symtab++) {
                if (strcmp(&shsymtab[symtab->st_name],name) == 0) {
                    return symtab->st_value;
                }
            }
        }
    }
}

unsigned long get_lib_path(const char *lib_name) {
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
        if (strstr(buf,lib_name) != NULL && strstr(buf, ".so") != NULL) {
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


Elf32_Addr get_sym_from_lib(const char *lib_name,const char *name) {
    int fd, i;
    struct stat st;
    Elf32_Addr symaddr;

    if ( (fd = open(lib_name, O_RDONLY)) < 0) {
        perror("open libc");
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        perror("fstat libc");
        return -1;
    }

    uint8_t *libp = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (libp == MAP_FAILED) {
        perror("mmap libc");
        return -1;
    }

    symaddr = resolve_symbol((char *)name, libp);
    if (symaddr == 0) {
        printf("[!] resolve_symbol failed for symbol '%s'\n", name);
        printf("Trying using --manual-elf-loading option\n");
        return -1;
    }
    symaddr = symaddr + lib_base_addr;

    return symaddr;
}


static Elf32_Addr get_func_plt_addr(const char *lib_path, const char *func_name) {
	Elf32_Ehdr *ehdr;
	Elf32_Shdr *shdr;
	Elf32_Phdr *phdr;
	uint8_t *mem;
	int fd;
	struct stat st;
	int i, j;

	if ( (fd = open(lib_path, O_RDONLY)) < 0) {
		perror("open");
		exit(1);
	}

	if (fstat(fd, &st) < 0) {
		perror("fstat");
		exit(1);
	}

	mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mem == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	if (mem[0] != 0x7f && strcmp(&mem[1], "ELF")) {
		fprintf(stderr,"It's not an ELF\n");
		exit(1);
	}

	ehdr = (Elf32_Ehdr *)mem;
	shdr = (Elf32_Shdr *)(mem + ehdr->e_shoff);
	phdr = (Elf32_Phdr *)(mem + ehdr->e_phoff);
	const char *shstrtab = (const char *)&mem[shdr[ehdr->e_shstrndx].sh_offset];

	Elf32_Sym *symtab;
	const char *strtab;
	Elf32_Addr plt_addr;
	Elf32_Off dlopen_off;

	Elf32_Rel *rel;
	int k;
	const char *sh_name;
	int rel_index;
	for (i = 0; i < ehdr->e_shnum; i++) {
		sh_name = &shstrtab[shdr[i].sh_name];
		if (strcmp(sh_name,".plt") == 0) {
			plt_addr = shdr[i].sh_addr;
		}
		if (strcmp(sh_name,".rel.plt") == 0) {
			rel = (Elf32_Rel *)&mem[shdr[i].sh_offset];
			Elf32_Rel *relp = rel;
			for (k = 0; k < shdr[i].sh_size / sizeof(Elf32_Rel); k++) {
				if (ELF32_R_SYM(relp->r_info) == j) {
					rel_index = relp - rel;
				}
				relp++;
			}
		}
		if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM) {
			symtab = (Elf32_Sym *)&mem[shdr[i].sh_offset];
			strtab = (const char *)&mem[shdr[shdr[i].sh_link].sh_offset];
			for (j = 0; j < shdr[i].sh_size / sizeof(Elf32_Sym); j++) {
				if (strcmp(&strtab[symtab->st_name],func_name) == 0) {
					dlopen_off = symtab->st_value;
					break;
				}
				symtab++;
			}
		}
	}

	Elf32_Addr result = (rel_index + 1) * 16 + plt_addr; // 每个plt两个字节且加上plt0

	return result + lib_base_addr;

}

long search(long start_addr,long call_path) {
	// 0x460 e8 offset
	int i = 0;
	const char *func = (const char *)start_addr;
	long dst_path;
	long offset;
	long mem_off;

	call_path = call_path - lib_base_addr;
	while (1) {
		if ((func[i] & 0xff) == 0xe8) { // call
			// call_path = 0x460
			// dst_path = start_addr + i + 5;
			// offset = call_path - dst_path
			dst_path = i + 5 + start_addr;
			dst_path = dst_path - lib_base_addr;
			offset = call_path - dst_path;
			mem_off = *(long *)&func[i + 1];

			if (mem_off == offset) {
				printf("-----------find mem_off = 0x%lx---------------\n", (unsigned long)mem_off);
				break;
			}
		}
		++i;
	}
	return start_addr + i;
}

void *handle;
Elf32_Addr next_call_dlopen_addr;
char mem[6];

void fun() {
	printf("%p\n",(void *)handle);
}

//static void test() {
//	long h;
//	__asm__("movl %%eax,%0"::"g"(h));
//	handle = (void *)h;
//	__asm__("pushl %eax");
//	memcpy((void *)next_call_dlopen_addr,mem,6);
//	__asm__("popl %eax");
//	__asm__("jmp *%0"::"g"(next_call_dlopen_addr));
//}

void copy() {
	memcpy((void *)next_call_dlopen_addr,mem,6);
}

int main(int argc, char **argv) {
	lib_base_addr = get_lib_path(LIB_TEST);

	extern int add(int, int);
	extern int __attribute__((weak)) do_add(int,int);

	Elf32_Addr add_addr = get_sym_from_lib(LIB_TEST,"add");

	Elf32_Addr dlopen_addr = get_func_plt_addr(LIB_TEST,"dlopen");

	Elf32_Addr call_dlopen_addr = search(add_addr, dlopen_addr);
	printf("call_dlopen_addr = %x\n",call_dlopen_addr);

	next_call_dlopen_addr = call_dlopen_addr + 5;
	memcpy(mem,(void *)next_call_dlopen_addr, 6);

	int a = 3, b = 4;
	printf("%d\n", add(a, b));

	extern void test();
	if (hook((void *)next_call_dlopen_addr, test) < 0) {
		fprintf(stderr,"hook error\n");
		exit(1);
	}

	printf("%d\n", add(a, b));

	printf("handle = %p\n",(void *)handle);

	getchar();

	return 0;
}
