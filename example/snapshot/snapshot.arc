network
  display
  threads 20 1 0

nodes mitral 
  size 49
  states 1
  rows 7
  noise .5 .5 0
  integrator 15 0 0 2
  sigmoid 0.1 1 2

nodes granule
  size 900
  states 1
  rows 30
  noise 0 0.05 0
  integrator 30 0 0 2
  sigmoid 0.1 1 2

weights mitral-granule
  sparse
  uniform 0 .75 .01 

weights granule-mitral
  sparse
  uniform -.25 0 .025 
