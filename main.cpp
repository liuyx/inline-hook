#include "inline-hook.h"
#include <iostream>
#include <cstring>
#include <future>
#include <mutex>
#include <thread>
#include <random>
#include <ctime>
#include <vector>
#include <ctime>

std::mutex g_mutex;

void print(const char *s) {
	std::lock_guard<std::mutex> lock(g_mutex);
	puts(s);
}

int my_strcmp(const char *s1, const char *s2) {
	print("hahahahah,it's been hooked");
	return 0;
}

int my_strlen(const char *s) {
	return 11;
}

int do_strcmp(const char *s1, const char *s2) {
	return strcmp(s1,s2);
}

static int counter;

void f() {
	//static std::default_random_engine e(time(0));
	//static std::uniform_int_distribution<unsigned> u(10,50);
	if (counter++ & 1)
	//	std::this_thread::sleep_for(std::chrono::milliseconds(u(e)));
	//else
		std::this_thread::sleep_for(std::chrono::seconds(1));
	const char *s1 = "hello";
	const char *s2 = "world";
	if (strcmp(s1,s2) == 0) {
		print("in f equal");
	} else {
		print("in f not equal");
	}
}

int main() {
	const char *s1 = "hello";
	const char *s2 = "world";

	std::vector<std::thread> vec;
	for (int i = 0; i < 10; i++) {
		vec.push_back(std::thread(f));
	}

	print("start hook in main thread");
	if (hook((void *)strcmp,(void *)my_strcmp) < 0) {
		perror("hook");
		exit(1);
	}

	if (strcmp(s1,s2) == 0) {
		print("equal");
	} else {
		print("not equal");
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	if (unhook((void *)strcmp) < 0) {
		perror("unhook");
		exit(1);
	}
	if (strcmp(s1,s2) == 0) {
		print("equal");
	} else {
		print("not equal");
	}

	for (auto &t : vec)
		t.join();

	return 0;
}
