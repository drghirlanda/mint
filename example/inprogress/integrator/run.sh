#!/bin/sh -ex
scons -c
scons
./integrator
Rscript integrator.R
