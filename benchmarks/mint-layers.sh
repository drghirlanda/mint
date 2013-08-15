#!/bin/sh -ex

MINTDAT=mint/mint-layers.dat
echo "N time memory minor major" > $MINTDAT

# compile minttest once for all (configuration done via config file)
cc -O2 -o mint/minttest mint/minttest.c -lm -lmint -lpthread

N = 1000 # size of each layer

for H in 1 5 10 20; do
    echo $L
    CONF=mint/mint.arc
    L=$(( $H + 2 ))
    W=$(( $H + 1 ))
    echo "network $L $W" > $CONF
    echo "nodes $N 0" >> $CONF
    for i in seq( 1, $(($L-2)) ); do
	echo "nodes $N 0 sigmoid 0.5 1" >> $CONF
    done
    echo "nodes 1 0 identity" >> $CONF
    for i in seq( 1, $(($L-2)) ); do
	echo "nodes $N 0 sigmoid 0.5 1" >> $CONF
    done

weights $N $N 0 from 0 to 1 random -.1 .1 1
weights 1 $N 0 from 1 to 2 random -.1 .1 1
EOF
# mint test run
    echo -n "$N " >>$MINTDAT
    /usr/bin/time -f "%U %M %R %F" ./mint/minttest 2>>$MINTDAT
done