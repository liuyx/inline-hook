#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

#define DEBUG		1
#if DEBUG
#define log(...)	printf(__VA_ARGS__)
#else
#define log(...)	((void)0)
#endif

typedef unsigned int u32;
typedef unsigned long long u64;

#define PAGE_SRART(x,pagesize)		((x) &~ (pagesize - 1))
#define likely(x)					__builtin_expect(!!(x), 1)
#define unlikely(x)					__builtin_expect(!!(x), 0)

#define BITS_PER_LONG				32

/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GLODEN_RATIO_PRIME_32		0x9e370001UL
/* 2^63 + 2^61 - 2^57 + 2^54 - 2^51 - 2^18  + 1 */
#define GLODEN_RATIO_PRIME_64		0x9e37fffffffc0001UL

#if BITS_PER_LONG == 32
#define GLODEN_RATIO_PRIME			GLODEN_RATIO_PRIME_32
#define hash_long(val, bits)		hash_32(val, bits)
#elif BITS_PER_LONG == 64
#define hash_long(val, bits)		hash_64(val, bits)
#define GLODEN_RATIO_PRIME			GLODEN_RATIO_PRIME_64
#else
#error Wordsize not 32 or 64
#endif

static inline u64 hash_u64(u64 val, unsigned int bits) {
	u64 hash = val;

	/* Sigh, gcc can't optimise this alone like it does for 32 bits. */
	u64 n = hash;
	n <<= 18;
	hash -= n;
	n <<= 33;
	hash -= n;
	n <<= 3;
	hash += n;
	n <<= 3;
	hash -= n;
	n <<= 4;
	hash += n;
	n <<= 2;
	hash += n;

	/* High bits are more random, so use them */
	return hash >> (64 - bits);
}

static inline u32 hash_32(u32 val, unsigned int bits) {
	/* On some cpus multiply is faster, on others gcc will do shifts */
	u32 hash = val * GLODEN_RATIO_PRIME_32;

	/* High bits are more random, so use them */
	return hash >> (32 - bits);
}

static inline unsigned long hash_ptr(const void *ptr, unsigned int bits) {
	return hash_long((unsigned long)ptr, bits);
}

typedef struct hash_table {
	long *bucket;
	int size;
	int (*put)(void *key, void *value);
	void* (*get)(void *key);
#define ADMIN_HASH_INIT_SIZE 32
}hash_table_t;

static hash_table_t *g_hash_table;

static int put(void *key, void *value);

static int init_hash_table_if_null() {
	if (g_hash_table == NULL) {
		g_hash_table = malloc(sizeof(struct hash_table));
		if (g_hash_table == NULL) {
			perror("malloc");
			return -1;
		}
		if ( (g_hash_table->bucket = malloc(ADMIN_HASH_INIT_SIZE * sizeof(long))) == NULL) {
			perror("malloc");
			return -1;
		}
		memset(g_hash_table->bucket,0,ADMIN_HASH_INIT_SIZE);
		g_hash_table->size = ADMIN_HASH_INIT_SIZE;
		g_hash_table->put = put;
	}
}

static int put(void *key, void *value) {
	init_hash_table_if_null();
	u32 hash = hash_ptr(key, sizeof(void *));
	long *bucket = g_hash_table->bucket;
	if (hash > g_hash_table->size) {
		g_hash_table->bucket = realloc(bucket, (int)(hash / 0.75f));
	}

	bucket[hash] = (long)value;

	log("put addr = 0x%lx to hash %d\n", (long)value, hash);

	return 0;
}

static int g_pagesize;

static int put_to_bucket(long addr, void *func, size_t len) {
	char *copy = malloc(len);
	if (copy == NULL) {
		perror("malloc");
		return -1;
	}
	const char *f = (const char *)func;
	memcpy(copy, f, len);

	// copy to hash
	return put((void *)addr,copy);
}

static int get_bucket(long addr, long *data) {
	long hash = hash_long(addr, sizeof(long));
	long value = (long)(g_hash_table->bucket[hash]);
	*data = value;

	log("get addr = 0x%lx from hash 0x%lx\n", value, hash);

	return 0;
}

int hook(void *old_func, const void *new_func) {
	if (g_pagesize == 0) 
		g_pagesize = getpagesize();

	int pz = g_pagesize;

	if (unlikely(mprotect((void *)PAGE_SRART((long)old_func, pz), pz, PROT_READ | PROT_WRITE | PROT_EXEC)) < 0) {
		perror("mprotect");
		return -1;
	}

	int ret = put_to_bucket((long)old_func, old_func, 6);
	if (unlikely(ret < 0))
		return ret;
	char *func = (char *)old_func;

	func[0] = 0x68;
	*(long *)&func[1] = (long)new_func;
	func[5] = 0xc3;
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

static int my_strcmp(const char *s1, const char *s2) {
	puts("-----------hook-------------");
	printf("your input, s1 = %s, s2 = %s\n", s1, s2);
	return 0;
}

static int my_puts(const char *s) {
	if (unhook(puts) < 0)
		return -1;
	puts("-------my hook start---------");
	puts(s);
	puts("-------my hook end---------");
}

static int my_strlen(const char *s) {
	return 0;
}

int main(int argc, char **argv) {
	char *s1 = "hello";
	char *s2 = "world";
	int ret;

	ret = hook(strcmp, my_strcmp);
	if (ret < 0) {
		exit(1);
	}

	if (strcmp(s1, s2) == 0) {
		puts("equal\n");
	} else {
		puts("not equal\n");
	}

	if ( (ret = unhook(strcmp)) < 0) {
		exit(1);
	}

	if (strcmp(s1, s2) == 0) {
		puts("equal\n");
	} else {
		puts("not equal\n");
	}

	if ( (ret = hook(strlen, my_strlen)) < 0) {
		exit(1);
	}

	printf("the length is %d\n", strlen(s1));

	if ( (ret = unhook(strlen)) < 0) {
		exit(1);
	}

	printf("the length is %d\n", strlen(s1));

	return 0;
}
