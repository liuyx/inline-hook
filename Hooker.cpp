//
// Created by lamoliu on 2017/4/1.
//

#include <unistd.h>
#include <sys/mman.h>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>
#include "Hooker.h"

void hooker::Hooker::changeCodeAttrs(void *func, int attr) {
    int pagesize = getpagesize();
    long start = PAGE_START((long)func,pagesize);

    if (mprotect((void *)start, (size_t)pagesize, attr) < 0) {
        throw std::string("mprotect error");
    }
}

void hooker::Hooker::hook(void *func, void *newAddr, void **origFunc) {
    changeCodeAttrs(func,CODE_WRITE);
    saveOriginFuncBytes(func);
    doHook(func,newAddr,origFunc);
    changeCodeAttrs(func,CODE_READ_ONLY);
}

void hooker::Hooker::saveOriginFuncBytes(void *func) {
    const size_t hookHeadSize = getHookHeadSize();

    void *save_bytes = malloc(hookHeadSize);
    if (save_bytes == nullptr)
        throw std::string("malloc error");

    memcpy(save_bytes,func,hookHeadSize);
    gHookedMap[(long)func] = (long)(save_bytes);

}

void hooker::Hooker::doUnHook(void *func) {
    long addr = gHookedMap[(long)func];
    if (addr == 0)
        throw std::string("it must be hooked before");
    memcpy(func,(void *)addr,getHookHeadSize());
    free((void *)addr);
}

void hooker::Hooker::unhook(void *func) {
    changeCodeAttrs(func,CODE_WRITE);
    doUnHook(func);
    changeCodeAttrs(func,CODE_READ_ONLY);
}
