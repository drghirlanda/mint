#!/bin/sh -ex
$(cd ../.. ./yruba example/feature-reversal)
./feature-reversal
Rscript feature-reversal.R


