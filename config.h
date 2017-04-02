#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __x86_64__
#include "HookerX64.h"
#elif defined(__i386__)
#include "HookerX86.h"
#elif defined(__arm__)
#include "HookerArm.h"
#elif defined(__thumb__)
#include "HookerThumb.h"
#else
#error "error architecture"
#endif

#endif
