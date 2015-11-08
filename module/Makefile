
EXTRA_CFLAGS=-I$(PWD)/../api
obj-m += nhm.o
#nhm-objs := ./nhm.o

KDIR = /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

module_sign:
	if [ ! -f certs/signing_key.priv ] ; then \
	  openssl req -new -nodes -utf8 -sha512 -days 36500 -batch -x509 -config certs/x509.genkey -outform DER -out certs/signing_key.x509 -keyout certs/signing_key.priv ; \
	fi;
	/usr/src/linux-headers-$(shell uname -r)/scripts/sign-file sha512 ./certs/signing_key.priv ./certs/signing_key.x509 ./nhm.ko
