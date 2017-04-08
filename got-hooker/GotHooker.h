#ifndef _GOT_HOOK_H_
#define _GOT_HOOK_H_

#include <cstdint>
#include <sys/mman.h>
#include <elf.h>
#include <link.h>

namespace hooker {
	namespace got {
		class GotHooker {
		public:
			void parse(const char *filename);
			void hook(const char *old, const void *newAddr, void **oldFunc);
		private:
			char *filename;
			uint8_t *mem;
			size_t filesz;
			ElfW(Ehdr) *ehdr;
			ElfW(Shdr) *shdr;
			ElfW(Phdr) *phdr;
			const char *shstrtab;
			ElfW(Sym) *dynsym;
			size_t dynsymCnt;
			const char *dynstr;
			size_t dynstrCnt;
			ElfW(Rel) *relPlt;
			size_t relPltCnt;
			ElfW(Rel) *relDyn;
			size_t relDynCnt;
			const char *strtab;
			size_t strtabCnt;

			ElfW(Addr) findRel(const char *func);
			void parseDetails(uint8_t *mem);
		};
	}
}

#endif
