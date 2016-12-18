#ifndef __HOOK_H_
#define __HOOK_H_

#include "common.h"

int hook(void *old_addr, const void *new_addr);
int unhook(void *old_addr);

#endif
