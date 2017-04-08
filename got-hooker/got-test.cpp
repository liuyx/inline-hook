#include "GotHooker.h"
#include <cstdio>
#include <iostream>

static int my_puts(const char *s) {
	std::cout << "hook" << " " << s << std::endl;
}

int main() {
	hooker::got::GotHooker gotHooker;
	gotHooker.parse("/proc/self/exe");
	gotHooker.hook("puts",(void *)my_puts,nullptr);

	puts("hello");
	return 0;
}
