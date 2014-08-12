network
nodes receptors size 10 noise 0.1 0.5 0 logistic 0.1 1
nodes brain size 10 logistic 0.1 1
nodes effectors size 2
weights receptors-brain uniform -0.1 1
weights brain-brain sparse normal 0 0.1 0.5
weights brain-effectors uniform -0.1 1
