network
  display
  threads 4 1 1
#  clocked 30

nodes exc 
  size 2500
  states 1
  rows 50
  noise 1 0.5 0
  integrator 10 0.1 0 2
  logistic .1 1 2 1

nodes inh
  size 2500
  states 1
  rows 50
  integrator 10 0.1 0 2
  logistic .1 1 2 1

weights exc-inh
  sparse
  uniform 0 .01 .01

weights inh-exc
  sparse
  uniform -.05 0 .02
