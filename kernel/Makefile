obj-m := konami_ppp.o
konami_ppp-objs := hid-konami-ppp.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	mkdir -p /lib/modules/$(shell uname -r)/extra
	cp konami_ppp.ko /lib/modules/$(shell uname -r)/extra
