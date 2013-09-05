network 3 2
feedforward 
nodes receptors
size 5 states 0 
0 0 0 0 0 
1 1 0 0 0 
nodes effectors
size 1 states 1 sigmoid 0.1 1 
-1.24526 
0.0519042 
0 
nodes value
size 1 states 0 
0 
1 
weights receptors-effectors
delta 0.1 2 states 0 cols 5 rows 1 
-0.627851 -0.627851 -0.4297 -0.4297 -0.4297 
weights value-effectors
target 2 random 0 0 1 states 0 cols 1 rows 1 
0 
spread 5
receptors value receptors-effectors value-effectors effectors 
