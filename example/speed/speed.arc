network
nodes exc size 100 integrator
nodes inh size 10000 integrator 
nodes output size 1 
weights exc-inh sparse uniform 0 1 .2
weights inh-exc sparse uniform -1 0 .2
weights exc-output
