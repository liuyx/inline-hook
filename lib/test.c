#include "test.h"
#include <stdio.h>
#include <dlfcn.h>

#define libc_path "/lib/i386-linux-gnu/libc.so.6"

static int do_add(int a, int b) {
	printf("in do_addr, a = %d, b = %d\n",a, b);
	return a + b;
}

static void *g_handle;

int add(int a, int b) {
	puts("-----------------add-----------------");
	void *handle = dlopen(libc_path, RTLD_LAZY);
	if (handle != NULL) {
		g_handle = handle;
	}
	printf("in add handle = %p\n",(void *)handle);
	long addr;
	__asm__("movl %%esp,%0"::"g"(addr));
	printf("esp = 0x%lx\n",addr);

	printf("hello,world\n");
	int ret =  do_add(a, b);
	printf("on ret = %d\n", ret);
	return ret;
}
