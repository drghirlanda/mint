network
  camera 320 240
  display
#  threads 4 0 1
#  clocked 10

nodes eye
  size 3072
  color 1 1 1 1
  rows 48

nodes brain
  size 3072
  states 1
  rows 48
  integrator 2 .5
  habituation 25 1 2 0
  bounded 0 1 1

weights eye-brain
diagonal 1

