CFLAGS=-std=c11 -g -static

stcc: stcc.c

test: stcc
	./test.sh

clean:
	rm -f stcc *.o *~ tmp*

.PHONY: test clean