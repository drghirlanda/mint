network
nodes n0 size 10 noise 1 1 0.1
nodes n1 size 5 sigmoid 0.01 1
nodes n2 size 5 sigmoid 0.01 1
nodes n3 size 1 sigmoid 0.01 1
weights w0 from n0 to n1 uniform 0 1
weights w1 from n1 to n2 uniform 0 1
weights w2 from n2 to n1 uniform 0 1
weights w3 from n1 to n3 uniform 0 1
spread 9
n0 w0 n1 w1 n2 w2 n1 w3 n3

