#!/bin/sh -ex
PROGRAM=phototaxis
scons $PROGRAM
./$PROGRAM
dot -Tpdf output/$PROGRAM.dot > output/$PROGRAM.pdf
Rscript plot.R
