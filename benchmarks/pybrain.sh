#!/bin/sh -ex

PYBRAINDAT=pybrain/pybrain.dat
echo "N time memory minor majorx" > $PYBRAINDAT

for N in 500 1000 2000 3500 5000 10000 20000; do
#for N in 500; do
    echo $N
    cat<<EOF>pybrain/pybraintest.py
#!/usr/bin/python
from pybrain.tools.shortcuts import buildNetwork
from numpy import zeros
input = zeros( $N )
net = buildNetwork( $N, $N, 1 )
for i in range(0,1000):
    net.activate( input )
EOF
# pybrain test run
    chmod u+x pybrain/pybraintest.py
    echo -n "$N " >>$PYBRAINDAT
    /usr/bin/time -f "%U %M %R %F" ./pybrain/pybraintest.py 2>>$PYBRAINDAT
done