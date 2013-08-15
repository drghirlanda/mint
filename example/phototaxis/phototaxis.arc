network 6 10
nodes 1 0
nodes 1 1 update integrator 5 .2
nodes 1 1 update integrator 5 .2
nodes 1 1 update metabolism .01
nodes 1 0 update sigmoid 0.5 1
nodes 1 0 update tonic 1
weights 1 1 0 0 1 constant .2
weights 1 1 0 0 2 constant 1
weights 1 1 0 1 2 constant -1.25
weights 1 1 0 2 1 constant -1.25
weights 1 1 0 3 1 constant -0.5
weights 1 1 0 3 2 constant 1.05
weights 1 1 0 1 4 constant 1
weights 1 1 0 2 4 constant -1
weights 1 1 0 0 3 constant 1
weights 1 1 0 5 1 constant 1.3

