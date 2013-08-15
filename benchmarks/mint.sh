#!/bin/sh -ex

# compile minttest once for all (configuration done via config file)
cc -O2 -o mint/minttest mint/minttest.c ../src/libmint.a -lm -lpthread

echo "N time memory minor major" > mint/mint.dat

for N in 500 1000 2000 3500 5000 10000 20000; do
    echo $N
    cat<<EOF>mint/mint.arc
network 3 2
nodes $N 0
nodes $N 0 sigmoid 0.5 1
nodes 1 0 identity
weights $N $N 0 from 0 to 1 random -.1 .1 1
weights 1 $N 0 from 1 to 2 random -.1 .1 1
EOF
echo -n "$N " >>mint/mint.dat
cd mint
/usr/bin/time -f "%U %M %R %F" ./minttest 2>>mint.dat
cd ..
done
