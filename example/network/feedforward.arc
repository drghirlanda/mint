network 3 2
feedforward
nodes receptors size 10 sigmoid 0.05 1
nodes brain size 5 sigmoid 0.05 1
nodes effectors size 1 sigmoid 0.05 1
weights receptors-brain random -.5 .15
weights w2 from brain to effectors random -.5 .15
