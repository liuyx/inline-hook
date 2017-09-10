//
// Created by lamoliu on 2017/4/1.
//

#ifdef __i386__
#include "HookerX86.h"

void hooker::HookerX86::doHook(void *func, void *newAddr, void **origFunc) const {
    char *f = (char *)func;
    f[0] = 0x68;
    *(long *)&f[1] = (long)newAddr;
    f[5] = 0xc3;
}
#endif
