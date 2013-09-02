network 3 2
feedforward
nodes n1 size 10 sigmoid 0.05 1
nodes n2 size 5 sigmoid 0.05 1
nodes n3 size 1 sigmoid 0.05 1
weights w1 from n1 to n2 random -.5 .15
weights w2 from n2 to n3 random -.5 .15
