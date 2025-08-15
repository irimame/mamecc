CC=g++
CFLAGS=-std=c++11 -g -static
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

mamecc: $(OBJS)
			$(CC) -o mamecc $(OBJS) $(LDFLAGS)

$(OBJS): $(wildcard *.h)

test: mamecc
	./test.sh

clean:
	rm -f mamecc *.o *~ tmp*

.PHONY: test clean