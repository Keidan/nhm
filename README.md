nhm
===

(GPL) Netfilter Hook Module is a FREE kernel module.



This module is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.

Instructions
============


To compile this module you should have the kernel source headers.

Download the software :

	mkdir devel
	cd devel
	git clone git://github.com/Keidan/nhm.git
	cd nhm
	make
	insmod module/nhm.ko or insmod module/nhm.ko ipv6_support=1


Udev
====


To get the UDEV information (after inserting the module):

	udevadm info -a -p /sys/class/nhm/nhm


To insert the module with the specific rights:

	KERNEL=="nhm", SUBSYSTEM=="nhm", MODE="0666", GROUP="nhmgroup"



API
====


The NHM module offers an API (api/nhm.h) specifying the transfered datas format and the list of IOCTL's that you can use to monitor the module.

You can also read the device (/dev/nhm) to obtain the list of the rules known by the module (circular buffer).

A sample code explaining how to communicate with the nhm module is available into the test folder.
  

License (like GPL)
==================

	You can:
		- Redistribute the sources code and binaries.
		- Modify the Sources code.
		- Use a part of the sources (less than 50%) in an other software, just write somewhere "nhm is great" visible by the user (on your product or on your website with a link to my page).
		- Redistribute the modification only if you want.
		- Send me the bug-fix (it could be great).
		- Pay me a beer or some other things.
		- Print the source code on WC paper ...
	You can NOT:
		- Earn money with this Software (But I can).
		- Add malware in the Sources.
		- Do something bad with the sources.
		- Use it to travel in the space with a toaster.
	
	I reserve the right to change this licence. If it change the version of the copy you have keep its own license


