It's a simple inline-hook framework works for intel just now, and it's based on new c++11 feature.

The usage of the this framework is very easy,example shows below:

int my_strcmp(const char *s1, const char *s2) 
{
	return 0;
}

const char *s1 = "hello";

const char *s2 = "world";

if (hook((void *)strcmp,(void *)my_strcmp) < 0) 
{
	perror("hook");
	exit(1);
}

if (strcmp(s1,s2) == 0) 
{
	puts("equal");
} 
else 
{
	puts("not equal");
}

The output is equal.

