#!/bin/sh -ex
PROGRAM=phototaxis-evolve
scons -c
scons $PROGRAM
/usr/bin/time ./$PROGRAM
dot -Tpdf output/$PROGRAM.dot > output/$PROGRAM.pdf
Rscript plot.R $PROGRAM
