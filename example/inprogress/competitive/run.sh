#!/bin/sh -ex
./competitive
dot -Tpdf output/competitive.dot > output/competitive.pdf
Rscript plot.R
