#include "hook.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

static int g_pagesize;
static hash_table_t *g_hash_table;

int hook(void *old_addr, const void *new_addr);
int unhook(void *old_addr);

static int put_to_bucket(long addr, void *func, size_t len) {
	char *copy;
	copy = (char *)malloc(len);
	if (copy == NULL) {
		perror("malloc");
		return -1;
	}
	const char *f = (const char *)func;
	memcpy(copy, f, len);

	g_hash_table->put(g_hash_table,(void *)addr, copy);
	
}

static int get_bucket(long addr, long *data) {
	long hash = hash_long(addr, sizeof(long));
	long value;
	int ret;
	ret = g_hash_table->get(g_hash_table,(void *)addr,&value);
	if (ret < 0) {
		return I_HASH_GET;
	}
	*data = value;

	log("get addr = 0x%lx from hash 0x%lx\n", value, hash);

	return 0;
}

int hook(void *old_func, const void *new_func) {
	if (g_pagesize == 0) {
		g_pagesize = getpagesize();
		if ( (g_hash_table = init_hash_table()) == NULL)
			return I_HASH_INIT;
	}

	int pz = g_pagesize;

	if (unlikely(mprotect((void *)PAGE_SRART((long)old_func, pz), pz, PROT_READ | PROT_WRITE | PROT_EXEC)) < 0) {
		perror("mprotect");
		return I_PERM;
	}

	int ret = put_to_bucket((long)old_func, old_func, 6);
	if (unlikely(ret < 0))
		return ret;
	char *func = (char *)old_func;

	func[0] = 0x68;
	*(long *)&func[1] = (long)new_func;
	func[5] = 0xc3;

	return 0;
}

int unhook(void *old_func) {
	int ret;
	long addr;
	if (unlikely(get_bucket((long)old_func, &addr)) < 0)
		return -1;

	char *ins = (char *)addr;
	char *f = (char *)old_func;
	memcpy(f, (void *)addr, 6);

	free((void *)addr);

	return 0;
}
