#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

static int my_strcmp(const char *s1, const char *s2) {
	puts("-----------hook-------------");
	printf("your input, s1 = %s, s2 = %s\n", s1, s2);
	return 0;
}

static int my_strlen(const char *s) {
	return 0;
}

int main(int argc, char **argv) {
	char *s1 = "hello";
	char *s2 = "world";
	int ret;

	ret = hook(strcmp, my_strcmp);
	if (ret < 0) {
		exit(1);
	}

	if (strcmp(s1, s2) == 0) {
		puts("equal\n");
	} else {
		puts("not equal\n");
	}

	if ( (ret = unhook(strcmp)) < 0) {
		exit(1);
	}

	if (strcmp(s1, s2) == 0) {
		puts("equal\n");
	} else {
		puts("not equal\n");
	}

	if ( (ret = hook(strlen, my_strlen)) < 0) {
		exit(1);
	}

	printf("the length is %d\n", strlen(s1));

	if ( (ret = unhook(strlen)) < 0) {
		exit(1);
	}

	printf("the length is %d\n", strlen(s1));

	return 0;
}
