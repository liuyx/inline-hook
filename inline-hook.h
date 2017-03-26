#ifndef _INLINE_HOOK_H
#define _INLINE_HOOK_H

int hook(void *old, void *new_addr);
int unhook(void *old);

#endif
