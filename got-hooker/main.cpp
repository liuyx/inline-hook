#include "simple-x86-got-hook.h"
#include <cstdio>
#include <iostream>

static int my_puts(const char *s) {
	std::cout << "hook" << " " << s << std::endl;
}

int main() {
	gotHook(reinterpret_cast<void *>(puts),reinterpret_cast<void *>(my_puts));
	puts("hello,world");
}
