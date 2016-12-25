libtest.so:
	gcc -fPIC -shared -o libtest.so test.c -ldl -O0
	strip libtest.so
	cp libtest.so ../
	cd ../
clean:
	rm -rf libtest.so
	
