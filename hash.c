#include "hash.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOAD_FACTOR	0.75f

int err;

static int put(struct hash_table *hash_table,const void *key, const void *value) {
	u32 hash = hash_ptr(key, sizeof(void *));
	long *bucket = hash_table->bucket;
	if (((int)hash/LOAD_FACTOR) > hash_table->size) {
		hash_table->bucket = realloc(bucket, (int)(hash / LOAD_FACTOR) * 2);
	}

	bucket[hash] = (long)value;

	log("put addr = 0x%lx to hash %d\n", (long)value, hash);

	return 0;
}

static int get(const struct hash_table *hash_table,const void *key,void *value) {
	long addr = (long)key;
	long hash = hash_long(addr, sizeof(long));
	long v = (long)(hash_table->bucket[hash]);
	*(long *)value = v;

	log("get addr = 0x%lx from hash 0x%lx\n", v, hash);

	return 0;
}

hash_table_t *init_hash_table() {
	hash_table_t *hash_table;
	hash_table = malloc(sizeof(struct hash_table));
	if (hash_table == NULL) {
		perror("malloc");
		err = I_OOM;
		return NULL;
	}
	hash_table->bucket = malloc(ADMIN_HASH_INIT_SIZE * sizeof(long));
	if (hash_table->bucket == NULL) {
		perror("malloc");
		err = I_OOM;
		return NULL;
	}
	hash_table->size = ADMIN_HASH_INIT_SIZE;
	hash_table->get = get;
	hash_table->put = put;

	return hash_table;
}

void hash_destroy(hash_table_t *hash_table) {
	if (hash_table->bucket != NULL) {
		free((void *)hash_table->bucket);
		hash_table->bucket = NULL;
	}

	free((void *)hash_table);
}

