#!/bin/bash

insmod ./module/nhm.ko
./test/nhm_test.elf
rmmod nhm
