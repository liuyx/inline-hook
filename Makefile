test:
	export LD_LIBRARY_PATH=.
	g++ -std=c++11 -o $@ main.cpp -L. -linline-intel

libinline-intel.so:
	g++ -std=c++11 -o $@ inline-intel.cpp -fPIC -shared

