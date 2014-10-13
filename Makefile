
obj-m += nhm.o
nhm-objs := ./src/nhm.o ./src/nhm_sysfs.o

KDIR = /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
