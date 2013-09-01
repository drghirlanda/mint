network 4 4
nodes n0 size 10 noise 1 1 0.1
nodes n1 size 5 sigmoid 0.01 1
nodes n2 size 5 sigmoid 0.01 1
nodes n3 size 1 sigmoid 0.01 1
weights from n0 to n1 random 0 1
weights from 1 to 2 random 0 1
weights from 2 to 1 random 0 1
weights from 1 to 3 random 0 1
spread 9
n0 n0-n1 n1 n1-n2 n2 n2-n1 n1 n1-n3 n3

