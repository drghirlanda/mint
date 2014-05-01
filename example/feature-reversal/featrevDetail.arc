network
  feedforward 

nodes value
  size 1 
  states 0 

0 
1 
nodes receptors
  size 5 
  states 0 

0 0 0 0 0 
1 1 0 0 0 
nodes effectors
  size 1 
  states 1 
  sigmoid 0.1 1 0 1 

8.91455 
0.999628 
1 
weights value-effectors
  target 2 
  states 0 
  cols 1 
  rows 1 

1 
weights receptors-effectors
  delta 0.1 2 
  states 0 
  cols 5 
  rows 1 

4.45731 4.45731 -2.92803 -2.92803 -2.92803 
spread
  value
  receptors
  value-effectors
  receptors-effectors
  effectors
