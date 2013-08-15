#!/bin/bash

N=4000

gcc -O2 -Wall -Werror threads.c -o threads -lmint -lm -lpthread
gcc -O0 -g -Wall -Werror threads.c -o threads-debug -lmint-debug -lm -lpthread

/bin/rm threads.dat

for T in $(seq 1 20); do
    cat<<EOF>threads.arc
network 2 1
threads $T 4
nodes $N 0
nodes $N 0
weights $N $N 0 from 0 to 1
EOF
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads.dat
done

Rscript threads.R
