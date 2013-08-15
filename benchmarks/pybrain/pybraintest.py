#!/usr/bin/python
from pybrain.tools.shortcuts import buildNetwork
from numpy import zeros
input = zeros( 20000 )
net = buildNetwork( 20000, 20000, 1 )
for i in range(0,1000):
    net.activate( input )
