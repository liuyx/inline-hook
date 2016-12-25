test:
	export LD_LIBRARY_PATH=.
	gcc main.c hash.c hook.c test.S -o test -L. -ltest -g -ldl

clean:
	rm -rf test
