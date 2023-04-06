# Makefile for Linux Module Creation
obj-m += ghostKey.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
