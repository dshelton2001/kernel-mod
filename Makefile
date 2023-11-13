obj-m += lkmasg2.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	gcc test.c -o test
	gcc test2.c -o test2
	gcc testmax.c -o testmax
	gcc testbuffer.c -o testbuffer
	gcc testread.c -o testread

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm test
	rm test2
	rm testmax
	rm testbuffer
	rm testread
