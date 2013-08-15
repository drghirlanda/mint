#!/bin/sh -ex
cc -O2 -o speed speed.c ../../src/libmint.a -lpthread -lfreeimage
./speed


