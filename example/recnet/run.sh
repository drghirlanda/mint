#!/bin/sh
./recnet
dot -T pdf recnet.dot > recnet-arc.pdf
Rscript recnet.R
