#!/bin/sh -ex
scons competitive
./competitive
dot -Tpdf output/competitive.dot > output/competitive.pdf
Rscript plot.R
