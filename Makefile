KVERSION = $(shell uname -r)
obj-m := pentab.o

all: compile_title modules detach

compile_title:
	@echo "  make test : complie test exec file for analyzing."

modules:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

detach:
	gcc detach.c -o detach `pkg-config libusb-1.0 --libs --cflags`

# apt-get install libusb-1.0-0-dev
test:
	gcc test.c -o test `pkg-config libusb-1.0 --libs --cflags`

install:
	sudo insmod pentab.ko

uninstall:
	sudo rmmod pentab

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	rm -rf detach
	rm -rf test
