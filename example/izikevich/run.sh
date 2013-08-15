#!/bin/sh -ex
scons -c 
scons izzy
/usr/bin/time -p ./izzy
dot -Tpdf output/izzyArc.dot > output/izzyArc.pdf
Rscript izzy.R 
