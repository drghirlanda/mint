network
  camera 320 240
  display
#  threads 4 0 1
  clocked 10

nodes eye
  size 76800
  color 1 1 1 1
  rows 240

nodes phototaxis
  size 200
  rows 2
  color 1 1 1 1

nodes brain
  size 49
  rows 7
  fastlogistic

weights eye-brain
  sparse
  normal 0 .1 .1

weights brain-brain
  sparse
  normal 0 .1 .1

  