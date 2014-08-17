#!/bin/bash -e

N=2000
TMAX=5

/bin/rm -f *.dat

function writearc {
    cat<<EOF>threads.arc
network
threads $1 $2 $3 
nodes n1 size $4 logistic
nodes n2 size $4 logistic
nodes n3 size $4 logistic
weights n1-n2 uniform 0 1 0.01
weights n2-n3 uniform 0 1 0.01
EOF
}

## baseline 1: single-threaded MINT
echo "single-threaded"
cat<<EOF>threads.arc
network
nodes n1 size $N logistic
nodes n2 size $N logistic
nodes n3 size $N logistic
weights n1-n2 uniform 0 1 0.01
weights n2-n3 uniform 0 1 0.01
EOF
/usr/bin/time -f '%e %M' ./threads 1>&2 2>>nothreads.dat

for T in $(seq 1 $TMAX); do
    writearc $T 1 1 $N
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads11.dat

    writearc $T 0 1 $N
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads01.dat

    writearc $T 1 0 $N
    echo $T
    /usr/bin/time -f '%e %M' ./threads 1>&2 2>>threads10.dat
done

Rscript threads.R
