#!/bin/sh -ex
make
time -p ./deltarule
Rscript deltarule.R
dot -Tpdf output/deltaruleArc.dot > output/deltaruleArc.pdf

