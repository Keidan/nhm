# nhm
[![Build Status](https://img.shields.io/travis/Keidan/nhm/master.svg?style=plastic)](https://travis-ci.org/Keidan/nhm)
[![GitHub license](https://img.shields.io/github/license/Keidan/nhm.svg?style=plastic)](https://github.com/Keidan/nhm/blob/master/LICENSE)

(GPL) LKM Netfilter Hook.



This module is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.



## Instructions


Make targets:

     module: To execute this target correctly, you should have the kernel source headers.
     demo: To execute this target correctly, you should have the Qt devel package.
     all: Build the module and the demo targets.
     clean: Clean the generated files.


Download the software :

	mkdir devel
	cd devel
	git clone git://github.com/Keidan/nhm.git
	cd nhm
	make


Insert the module:

	insmod module/nhm.ko


Or with IPv6 support:

	insmod module/nhm.ko ipv6_support=1



## Udev


To get the UDEV information (after inserting the module):

	udevadm info -a -p /sys/class/nhm/nhm


To insert the module with specific rights:

	KERNEL=="nhm", SUBSYSTEM=="nhm", MODE="0666", GROUP="plugdev"



## API


The NHM module provides an API (api/nhm.h) specifying the format of the transferred data and the list of IOCTLs that you can use to monitor the module.

You can also read the device (/dev/nhm) to get the list of rules known by the module (circular buffer).

The demo codes that explain how to communicate with the nhm module are available in the demo folder.
  

## License

[GPLv3](https://github.com/Keidan/nhm/blob/master/LICENSE)
