//
// Created by lamoliu on 2017/4/1.
//

#ifdef __arm__
#include "HookerArm.h"
#include <sys/cachectl.h>

void hooker::HookerArm::doHook(void *func, void *newAddr, void **origFunc) const {
    char *f = (char *)func;
    *(long *)&f[0] = 0xe51ff004; // ldr pc, addr
    *(long *)&f[4] = (long)newAddr;

#ifdef cacheflush
    cacheflush((long)func,(long)func + 8, 0);
#endif
}
#endif
