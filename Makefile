obj-m += lkmasg1.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	gcc test.c -o test

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm test
