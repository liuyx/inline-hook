#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>

#define PAGE_SRART(x,pagesize)		((x) &~ (pagesize - 1))
#define likely(x)					__builtin_expect(!!(x), 1)
#define unlikely(x)					__builtin_expect(!!(x), 0)

#define DEBUG		0
#if DEBUG
#define log(...)	printf(__VA_ARGS__)
#else
#define log(...)	((void)0)
#endif

typedef enum inline_error {
	I_OOM = -1,
	I_PERM = -2,
	I_HASH_INIT = -3,
	I_HASH_GET = -4
}i_err;

#endif
