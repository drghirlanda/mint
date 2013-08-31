network 3 2
feedforward
nodes 10 0 input sigmoid 0.05 1
nodes 5 0 hidden sigmoid 0.05 1
nodes 1 0 output sigmoid 0.05 1
weights 5 10 0 from input to hidden random -.5 .15
weights 1 5 0 from hidden to output random -.5 .15
