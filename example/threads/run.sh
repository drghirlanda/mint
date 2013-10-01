#!/bin/bash -e

N=2000
TMAX=10

/bin/rm -f *.dat

## baseline 1: single-threaded MINT
cat<<EOF>threads.arc
network
nodes n1 size $N sigmoid
nodes n2 size $N sigmoid
weights n1-n2 uniform 0 1 0.01
EOF
echo "single-threaded"
/usr/bin/time -f '%e %M' ./threads 1>&2 2>>nothreads.dat

for T in $(seq 1 $TMAX); do
    cat<<EOF>threads.arc
network
threads $T 1 1
nodes n1 size $N sigmoid
nodes n2 size $N sigmoid
weights n1-n2 uniform 0 1 0.01
EOF
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads11.dat
done


for T in $(seq 1 $TMAX); do
    cat<<EOF>threads.arc
network
threads $T 0 1
nodes n1 size $N sigmoid
nodes n2 size $N sigmoid
weights n1-n2 uniform 0 1 0.01
EOF
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads01.dat
done


for T in $(seq 1 $TMAX); do
    cat<<EOF>threads.arc
network
threads $T 1 0
nodes n1 size $N logistic
nodes n2 size $N logistic
weights n1-n2 uniform 0 1 0.01
EOF
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads10.dat
done

Rscript threads.R
