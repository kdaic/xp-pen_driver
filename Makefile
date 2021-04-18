KVERSION = $(shell uname -r)
obj-m := pentab.o

all: modules detach

help:
	@echo "Help"
	@echo "  make           : compile pentab.ko kernel module and detach command file."
	@echo "  make install   : register pentab.ko into kernel"
	@echo "  make uninstall : remove pentab.ko from kernel"
	@echo "  make test      : complie test application for analyzing."
	@echo "  make clean     : remove pentab.ko, detach, test"

modules:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

detach:
	gcc detach.c -o detach `pkg-config libusb-1.0 --libs --cflags`

install:
	insmod pentab.ko

uninstall:
	rmmod pentab.ko

# apt-get install libusb-1.0-0-dev
test:
	gcc test.c -o test `pkg-config libusb-1.0 --libs --cflags`

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	rm -rf detach
	rm -rf test
