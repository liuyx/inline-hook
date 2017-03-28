#include "inline-hook.h"
#include <iostream>
#include <cstring>
#include <future>
#include <mutex>
#include <thread>

std::recursive_mutex g_mutex;

int my_strcmp(const char *s1, const char *s2) {
	puts("hahahahah,it's been hooked");
	return 0;
}

int my_strlen(const char *s) {
	return 11;
}

int do_strcmp(const char *s1, const char *s2) {
	std::lock_guard<std::recursive_mutex> lock(g_mutex);
	std::cout << "the threadid = " << pthread_self() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return strcmp(s1,s2);
}

int main() {
	const char *s1 = "hello";
	const char *s2 = "world";

	std::future<int> fut = std::async(std::launch::async,do_strcmp,s1,s2);

	std::lock_guard<std::recursive_mutex> lock(g_mutex);
	std::cout << "the result is " << fut.get() << std::endl;

	if (hook((void *)strcmp,(void *)my_strcmp) < 0) {
		perror("hook");
		exit(1);
	}
	if (strcmp(s1,s2) == 0) {
		puts("equal");
	} else {
		puts("not equal");
	}

	std::future<int> fut2 = std::async(std::launch::deferred,do_strcmp,s1,s2);

	std::cout << "the result is " << fut2.get() << std::endl;

	if (unhook((void *)strcmp) < 0) {
		perror("unhook");
		exit(1);
	}
	if (strcmp(s1,s2) == 0) {
		puts("equal");
	} else {
		puts("not equal");
	}

	std::future<int> fut3 = std::async(std::launch::deferred,do_strcmp,s1,s2);

	std::cout << "the result is " << fut3.get() << std::endl;

	return 0;
}
