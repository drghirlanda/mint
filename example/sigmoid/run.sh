#!/bin/sh -ex
scons -c
scons
./sigmoid
Rscript sigmoid.R
