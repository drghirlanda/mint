network 2 3
threads 4 10
nodes 2400 5 noise 0 0 7 izzy .02 .2 -70 7
nodes 600 2 noise 0 0 2 izzy .06 .225 -65 2
weights 2400 2400 0 sparse from 0 to 0 uniform 0 .5 .1 diagonal 0
weights 2400 600 0 sparse from 1 to 0 uniform -1 0 .1
weights 600 2400 0 sparse from 0 to 1 uniform 0 .5 .1