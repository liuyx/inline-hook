all:
	make libinline-intel.so
	make test
test:
	g++ -std=c++11 -o $@ main.cpp -L. -linline-intel

libinline-intel.so:
	g++ -std=c++11 -o $@ inline-intel.cpp -fPIC -shared

clean:
	rm -rf test 
	rm -rf libinline-intel.so

