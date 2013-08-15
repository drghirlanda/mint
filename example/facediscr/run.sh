#!/bin/sh -ex
make
time ./facediscr
Rscript facediscr.R
dot -Tpdf output/facediscrArc.dot > output/facediscrArc.pdf

