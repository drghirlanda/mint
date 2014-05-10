#!/bin/sh -ex
cd ../..
./yruba example/feature-reversal
cd example/feature-reversal
./feature-reversal
Rscript feature-reversal.R


