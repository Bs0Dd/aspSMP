#!/bin/bash
gcc -m32 -O2 -s -I../ -Ilibusb-compat aspsmp.c opendevice.c -o aspsmp libusb-compat/libusb.so -lusb-1.0
