
#include "../hooker/HookerFactory.h"
#include <iostream>
#include <cstring>
#include <future>
#include <cstdio>
#include <random>

void print(const char *s) {
    std::cout << s << std::endl;
}

int my_strcmp(const char *s1, const char *s2) {
    std::cout << s1 << " " << s2 << ",haha, it's been hooked" << std::endl;
    return 0;
}

int main() {
    const char *s1 = "hello";
    const char *s2 = "world";

    using namespace hooker;
    HookerFactory* factory = HookerFactory::getInstance();
    const Hooker& hooker = factory->getHooker();
    hooker.hook(reinterpret_cast<void *>(strcmp), reinterpret_cast<void *>(my_strcmp), nullptr);

    if (strcmp(s1,s2) == 0) {
        print("equal");
    } else {
        print("not equal");
    }

    hooker.unhook(reinterpret_cast<void *>(strcmp));

    if (strcmp(s1,s2) == 0) {
        print("equal");
    } else {
        print("not equal");
    }

	delete factory;

    return 0;
}
