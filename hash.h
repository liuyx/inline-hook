#ifndef _HASH_H_
#define _HASH_H_

typedef unsigned int u32;
typedef unsigned long long u64;

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
	int (*get)(const struct hash_table *,const void *key,void *value);
	int (*put)(struct hash_table *,const void *key, const void *value);
#define ADMIN_HASH_INIT_SIZE 32
} hash_table_t;

hash_table_t *init_hash_table();


#endif
