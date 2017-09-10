//
// Created by lamoliu on 2017/4/1.
//

#ifdef __x86_64__
#include <cstdlib>
#include <string>
#include "HookerX64.h"

void hooker::HookerX64::doHook(void *func,void *newAddr,void **origFunc) const {
    char *f = (char *)func;
    *(uint16_t *)&f[0] = 0x25ff;
    *(int *)&f[2] = 0x00000000;
    *(long *)&f[6] = (long)newAddr;

}
#endif

