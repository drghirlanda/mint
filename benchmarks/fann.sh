#!/bin/sh -ex

FANNDAT=fann/fann.dat
FANNACT=FANN_SIGMOID_SYMMETRIC
echo "N time memory minor major" > $FANNDAT

for N in 500 1000 2000 3500 5000 10000 20000; do
    echo $N

# fann configuration file
    cat<<EOF>fann/fanntest.h
#define N $N
#define ACT $FANNACT
EOF

# fann test run
    cc -O2 -o fann/fanntest fann/fanntest.c -lm -lfann
    echo -n "$N " >>$FANNDAT
    /usr/bin/time -f "%U %M %R %F" ./fann/fanntest 2>>$FANNDAT

done