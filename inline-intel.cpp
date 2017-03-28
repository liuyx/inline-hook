#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sys/mman.h>
#include <unordered_map>
#include <exception>
#include <thread>
#include <mutex>

std::unordered_map<long,long> g_hook_map;
std::mutex g_func_mutex;


#ifdef __x86_64__
#define HOOK_HEAD_SIZE	22
#elif defined __i386__
#define HOOK_HEAD_SIZE  6
#else
#error "Error support architecture fornow"
#endif

#define PAGE_START(x,pagesize)	((x) &~ (pagesize) - 1)

#define CODE_WRITE				PROT_READ | PROT_WRITE | PROT_EXEC
#define CODE_READ_ONLY			PROT_READ | PROT_EXEC

int change_code_attr(void *func,int attr) {
	int pagesize = getpagesize();
	unsigned long start = (long)func &~ (pagesize - 1);

	if (mprotect((void *)start, pagesize, attr) < 0) {
		perror("mprotect");
		return -1;
	}
	return 0;
}

#ifdef __x86_64__
int hook_x64(void *old, void *new_addr) {
	if (change_code_attr(old,CODE_WRITE) < 0) {
		return -1;
	}
	char *f = (char *)old;
	
	void *save_bytes = malloc(HOOK_HEAD_SIZE);
	if (save_bytes == nullptr)
		return -1;

	memcpy(save_bytes,f,HOOK_HEAD_SIZE);
	g_hook_map[(long)old] = (long)save_bytes;

	f[0] = 0xff;
	f[1] = 0x25;
	*(int *)&f[2] = 0x000000;
	*(long *)&f[6] = (long)new_addr;
	*(long *)&f[14] = 0xefcdab8967452301;

	return 0;
}
#endif

#ifdef __i386__
int hook_x86(void *old, void *new_addr) {
	if (change_code_attr(old,CODE_WRITE) < 0) {
		return -1;
	}
	char *f = (char *)old;

	void *save_bytes = malloc(HOOK_HEAD_SIZE);
	if (save_bytes == nullptr)
		return -1;

	memcpy(save_bytes,f,HOOK_HEAD_SIZE);
	g_hook_map[(long)old] = (long)save_bytes;

	f[0] = 0x68;
	*(long *)&f[1] = (long)new_addr;
	f[5] = 0xc3;

	return 0;
}
#endif

int hook(void *old, void *new_addr) {
	std::lock_guard<std::mutex> lock(g_func_mutex);
#ifdef __x86_64__
	return hook_x64(old,new_addr);
#elif defined __i386__
	return hook_x86(old,new_addr);
#else
	throw std::string("unsupported hook architecture");
#endif
}

int unhook(void *old) {
	const int size = HOOK_HEAD_SIZE;
	long addr = g_hook_map[(long)old];
	if (addr == 0) 
		throw std::string("it must be hooked before");
	memcpy(old,(void *)addr,size);
	free((void *)addr);
	if (change_code_attr(old,CODE_READ_ONLY) < 0) 
		return -1;
	return 0;
}

