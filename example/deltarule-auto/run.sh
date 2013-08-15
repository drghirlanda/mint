#!/bin/sh -ex
scons
./deltarule
Rscript deltarule.R
dot -Tpdf output/deltaruleArc.dot > output/deltaruleArc.pdf

