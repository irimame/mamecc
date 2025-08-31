CFLAGS=-std=c11 -g -static

mamecc: mamecc.c

test: mamecc
				./test.sh

clean:
				rm -f mamecc *.o *~ tmp*

.PHONY: test clean