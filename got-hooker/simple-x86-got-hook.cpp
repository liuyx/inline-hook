#include "simple-x86-got-hook.h"

/**
  * @param old plt address
  * @param newAddr new function address
  */
void gotHook(void *old, void *newAddr) {
	char *f = reinterpret_cast<char *>(old);
	long gotAddr = *(reinterpret_cast<long *>(&f[2]));
	*(long *)gotAddr = (long)newAddr;
}
