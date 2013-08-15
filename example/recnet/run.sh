#!/bin/sh -ex
/bin/rm -f recnet.o recnet
make
time ./recnet
Rscript recnet.R
dot -Tpdf output/recnetArc.dot > output/recnetArc.pdf

