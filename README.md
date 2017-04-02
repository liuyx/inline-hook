It's a simple inline-hook framework works for intel(32bit,64bit) and arm(only works for arm32 and thumb) just now, and it's based on new c++11 feature.

<b>How to build</b>:
the framework based on cmake 2.8, all you have to do is:<br/>
cmake .</br>
make<br/>

<b>How to use</b>:<br/>
The usage of the this framework is very easy,example(main.cpp) shows below:<br/>

```c++

int my_strcmp(const char *s1, const char *s2) <br/>
{<br/>
	return 0;<br/>
}<br/>

const char *s1 = "hello";<br/>

const char *s2 = "world";<br/>

if (hook((void *)strcmp,(void *)my_strcmp) < 0) <br/>
{<br/>
	perror("hook");<br/>
	exit(1);<br/>
}<br/>

if (strcmp(s1,s2) == 0) <br/>
{<br/>
	puts("equal");<br/>
}<br/>
else<br/>
{<br/>
	puts("not equal");<br/>
}<br/>

```

The output is equal.

