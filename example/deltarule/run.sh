#!/bin/sh -ex
./deltarule
Rscript deltarule.R
dot -Tpdf deltaruleArc.dot > deltaruleArc.pdf

