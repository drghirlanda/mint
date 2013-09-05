#!/bin/bash

N=1000

/bin/rm -f threads-*.dat

for T in $(seq 1 20); do
    cat<<EOF>threads.arc
network 2 1
threads $T 4
nodes n1 size $N sigmoid .1 1
nodes n2 size $N sigmoid .1 1
weights n1-n2
EOF
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads-w=1-t=4.dat
done


for T in $(seq 1 20); do
    cat<<EOF>threads.arc
network 2 2
threads $T 4
nodes n1 size $N sigmoid .1 1
nodes n2 size $N sigmoid .1 1
weights n1-n2
weights n1-n2.2
EOF
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads-w=2-t=4.dat
done


for T in $(seq 1 20); do
    cat<<EOF>threads.arc
network 2 2
threads $T 6
nodes n1 size $N sigmoid .1 1
nodes n2 size $N sigmoid .1 1
weights n1-n2
weights n1-n2.2
EOF
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads-w=2-t=6.dat
done

Rscript threads.R
