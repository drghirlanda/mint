#!/bin/bash

N=5000

gcc -O2 -Wall -Werror sparse.c -o sparse -lmint -lm -lpthread

for D in $(seq 0.1 0.1 .1); do
    cat<<EOF>sparse.arc
network 2 1
nodes $N 0
nodes $N 0 sigmoid
weights $N $N 0 from 0 to 1 normal -1 1 $D
EOF
    /usr/bin/time -f '%e %M' ./sparse
done
