network
  display
  clocked 24

nodes mitral 
  size 100
  states 1
  rows 10
  noise 0 1 1
  integrator 100 0
#  sigmoid 0.1 .1 

nodes granule
  size 400
  states 1
  rows 20
  integrator 500 0
#  sigmoid 0.1 1 

weights mitral-granule
  sparse
  uniform 0 .1 .05 

weights granule-mitral
  sparse
  uniform -.1 0 .1 
