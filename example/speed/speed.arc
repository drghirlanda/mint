network
threads 4 0 1
nodes n0 size 4000 logistic
nodes n1 size 4000 logistic 
nodes n2 size 4000 logistic 
nodes n3 size 4000 logistic 
weights n0-n1 uniform 0 1
weights n1-n0 uniform 0 1
weights n1-n2 uniform 0 1
weights n2-n1 uniform 0 1
weights n2-n3 uniform 0 1
weights n3-n2 uniform 0 1


