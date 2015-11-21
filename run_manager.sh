#!/bin/bash

trap leave INT

lv=0

leave() {
    if [ ${lv} -eq 0 ]; then
	echo "Remove module nhm"
	rmmod nhm
	lv=1
    fi
}


echo "Insert module ./module/nhm.ko"
insmod ./module/nhm.ko
echo "Excecute manager app"
./demo/manager/manager.elf
leave
