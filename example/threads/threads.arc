network
  display
  threads 3 1 1
  clocked 10

nodes n1 
  size 5 
  rows 2
  noise .5 .2
  sigmoid

nodes n2 
  size 425
  rows 15 
  sigmoid

weights n1-n2 
  normal

weights n2-n1 
  normal
