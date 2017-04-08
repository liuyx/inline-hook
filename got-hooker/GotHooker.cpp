#include "GotHooker.h"
#include <elf.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <string>

uint8_t *mapFile2Memory(const char *filename) {
	int fd;
	if ( (fd = open(filename, O_RDONLY)) < 0) 
		throw std::string("open file error");

	struct stat st;
	if (fstat(fd,&st) < 0) 
		throw std::string("fstat error");

	uint8_t *mem;
	if ( (mem = (uint8_t *)mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,fd,0)) == MAP_FAILED) 
		throw std::string("mmap error");
	return mem;
}

void hooker::got::GotHooker::parseDetails(uint8_t *mem) {
	ehdr = (ElfW(Ehdr) *)mem;
	shdr = (ElfW(Shdr)* )(mem + ehdr->e_shoff);
	phdr = (ElfW(Phdr)* )(mem + ehdr->e_phoff);
	shstrtab = (const char *)&mem[shdr[ehdr->e_shstrndx].sh_offset];

	const char *section;
	for (int i = 0; i < ehdr->e_shnum; i++) {
		section = &shstrtab[shdr[i].sh_name];
		switch (shdr[i].sh_type) {
			case SHT_DYNSYM:
				if (strcmp(section,".dynsym") == 0) {
					dynsym = (ElfW(Sym) *)&mem[shdr[i].sh_offset];
					dynsymCnt = shdr[i].sh_size / sizeof(ElfW(Sym));
					dynstr = (const char *)&mem[shdr[shdr[i].sh_link].sh_offset];
					dynstrCnt = dynsymCnt;
				}
				break;
			case SHT_REL:
				if (strcmp(section,".rel.plt") == 0) {
					relPlt = (ElfW(Rel) *)(shdr[i].sh_addr);
					relPltCnt = shdr[i].sh_size / sizeof(ElfW(Rel));
				} else if (strcmp(section,".rel.dyn") == 0) {
					relDyn = (ElfW(Rel) *)(shdr[i].sh_addr);
					relDynCnt = shdr[i].sh_size / sizeof(ElfW(Dyn));
				}
				break;
		}
	}

}

void hooker::got::GotHooker::parse(const char *filename) {
	this->mem = mapFile2Memory(filename);
	parseDetails(mem);
}

ElfW(Addr) hooker::got::GotHooker::findRel(const char *func) {
	size_t funcLen = strlen(func);

	ElfW(Rel) *rel;
	int index = 0;
	char *name;
	for (int i = 0; i < relPltCnt; i++) {
		rel = &relPlt[i];
		if (ELF32_R_TYPE(rel->r_info) == R_386_JMP_SLOT) {
			index = ELF32_R_SYM(rel->r_info);
			name = (char *)&dynstr[dynsym[index].st_name];

			if (strncmp(name, func, funcLen) == 0) {
				return (ElfW(Addr))rel->r_offset;
			}
		}
	}
	return 0;
}

void hooker::got::GotHooker::hook(const char *func,const void *newFunc,void **oldFunc) {
	ElfW(Addr) relAddr = findRel(func);

	if (relAddr == 0) 
		throw std::string("can't find rel");

	void **addr = (void **)relAddr;
	if (oldFunc != nullptr)
		*oldFunc = addr;
	*addr = (void *)newFunc;
}
