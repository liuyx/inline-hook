//
// Created by lamoliu on 2017/4/1.
//

#ifdef __thumb__
#include <sys/cachectl.h>
#include "HookerThumb.h"

void hooker::HookerThumb::doHook(void *func, void *newAddr, void **origFunc) const {
	long addr = (long)func;
	long clearBit0 = addr & 0xfffffffe;
	char *f = (char *)clearBit0;
	if (clearBit0 % 4 != 0) {
	    *(uint16_t *)&f[0] = 0xbe00; // NOP
	}
	*(uint16_t *)&f[2] = 0xf8df;
	*(uint16_t *)&f[4] = 0xf000; // LDR.W PC, [PC]
	*(uint16_t *)&f[6] = (long)newAddr & 0xffff;
	*(uint16_t *)&f[8] = (long)newAddr >> 16;
	cacheflush(clearBit0, clearBit0 + getHookHeadSize(), 0);
}
#endif
