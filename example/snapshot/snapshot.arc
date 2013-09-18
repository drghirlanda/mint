network
  clocked 2

nodes e 
  size 800
  rows 20
  noise 0 .001 .002
  sigmoid 0.1 1 
  snapshot 1 1 1

nodes i
  size 200
  sigmoid 0.1 1

weights e-e
  sparse
  uniform 0 .85 .075

weights e-i
  sparse
  uniform 0 1 .1

weights i-e
  sparse
  uniform -1 0 .1
