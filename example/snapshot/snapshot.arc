network
  display
  threads 40 0 1

nodes mitral 
  size 100
  states 1
  rows 10
  noise 0.5 0.05 0
  integrator 1 0 0 2
  sigmoid 0.1 1 2 1

nodes granule
  size 400
  states 1
  rows 20
  integrator 50 0 0 2
  sigmoid 0.1 1 2 1

weights mitral-granule
  sparse
  uniform 0 .75 .01 

weights granule-mitral
  sparse
  uniform -.15 0 .01 
