CFLAGS=-std=c++11 -g -static

mamecc: mamecc.cpp

test: mamecc
	./test.sh

clean:
	rm -f mamecc *.o *~ tmp*

.PHONY: test clean