CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

mamecc: $(OBJS)
				$(CC) -o mamecc $(OBJS) $(LDFRAGS)

$(OBJS): mamecc.h

test: mamecc
				./test.sh

clean:
				rm -f mamecc *.o *~ tmp*

.PHONY: test clean