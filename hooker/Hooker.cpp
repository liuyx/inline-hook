//
// Created by lamoliu on 2017/4/1.
//

#include <unistd.h>
#include <sys/mman.h>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include "Hooker.h"
#include "HookerError.h"

hooker::Hooker::~Hooker() {}

void hooker::Hooker::changeCodeAttrs(void *func, int attr) const {
    int pagesize = getpagesize();
    long start = PAGE_START((long)func,pagesize);

    if (mprotect((void *)start, (size_t)pagesize, attr) < 0) {
        throw hooker::error::HookerError("mprotect error");
    }
}

void hooker::Hooker::hook(void *func, void *newAddr, void **origFunc, bool saveOrig) const {
    changeCodeAttrs(func,CODE_WRITE);

	if (saveOrig)
		saveOriginFuncBytes(func);

    doHook(func,newAddr,origFunc);

    changeCodeAttrs(func,CODE_READ_ONLY);
}

void hooker::Hooker::saveOriginFuncBytes(void *func) const {
    const size_t hookHeadSize = getHookHeadSize();
	const size_t originFunctionSize = getOrigFunctionSize();
	assert(originFunctionSize >= hookHeadSize);

    void *save_bytes = malloc(hookHeadSize);
    if (save_bytes == nullptr)
        throw hooker::error::HookerError("malloc error");

	// save the origin bytes here.
    memcpy(save_bytes,func,hookHeadSize);
    gHookedMap[(long)func] = (long)(save_bytes);

	// and now, we want to insert a jump to the next instruction
	// after the hooked start.
	if (originFunctionSize > hookHeadSize) {
		char *f = (char *)func;
		char *n = (char *)save_bytes;
		hook((void *)&n[hookHeadSize],(void *)&f[hookHeadSize],nullptr, false);
	}

}

void hooker::Hooker::doUnHook(void *func, void *oldfunc) const {
	if (oldfunc != nullptr) {
		changeCodeAttrs(oldfunc, CODE_WRITE);
		free(oldfunc);
	}
    long addr = gHookedMap[(long)func];
    if (addr == 0)
        throw hooker::error::HookerError("it must be hooked before");
    memcpy(func,(void *)addr,getHookHeadSize());
    free((void *)addr);
	gHookedMap.erase((long)func);
}

void hooker::Hooker::unhook(void *func, void *oldfunc) const {
    changeCodeAttrs(func,CODE_WRITE);
    doUnHook(func, oldfunc);
    changeCodeAttrs(func,CODE_READ_ONLY);
}
