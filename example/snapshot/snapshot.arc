network
clocked 0.1
display

nodes e 
  size 400
  rows 20
  noise 0 .001 .002
  sigmoid 0.1 1 
#  snapshot 1 1 0

weights e-e
  sparse
  uniform -0.5 1 
