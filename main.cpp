#include "inline-hook.h"
#include <iostream>
#include <cstring>


int my_strcmp(const char *s1, const char *s2) {
	puts("hahahahah,it's been hooked");
	return 0;
}

int my_strlen(const char *s) {
	return 11;
}

int main() {
	const char *s1 = "hello";
	const char *s2 = "world";
	if (hook((void *)strcmp,(void *)my_strcmp) < 0) {
		perror("hook");
		exit(1);
	}
	if (strcmp(s1,s2) == 0) {
		puts("equal");
	} else {
		puts("not equal");
	}

	if (unhook((void *)strcmp) < 0) {
		perror("unhook");
		exit(1);
	}
	if (strcmp(s1,s2) == 0) {
		puts("equal");
	} else {
		puts("not equal");
	}

	printf("the length of %s is %d\n",s1,strlen(s1));
	if (hook((void *)strlen,(void *)my_strlen) < 0) {
		perror("hook");
		exit(1);
	}

	printf("the length of %s is %d\n",s1,strlen(s1));

	if (unhook((void *)strlen) < 0) {
		perror("unhook");
		exit(1);
	}
	printf("the length of %s is %d\n",s1,strlen(s1));

	return 0;
}
