arch?=arm
modname?=motor_driver

PWD:=$(shell pwd)

ifeq ($(arch), arm)
	KERNELDIR := /home/ubuntu/linux-5.10.10/
endif

ifeq ($(arch), x86)
	KERNELDIR := /lib/modules/$(shell uname -r)/build/
endif

all:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	make -C $(KERNELDIR) M=$(PWD) clean
install:
	sudo cp $(modname).ko ~/nfs/rootfs/driver/

obj-m:=$(modname).o
