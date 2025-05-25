CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

stcc: $(OBJS)
	$(CC) -o stcc $(OBJS) $(LDFLAGS)

$(OBJS): stcc.h

test: stcc
	./test.sh

clean:
	rm -f stcc *.o *~ tmp*

.PHONY: test clean