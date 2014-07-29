network
nodes lr size 2 states 1 
  noise 1 .25 0
  integrator 10 .1 2
  logistic 0.1 1 2 1

nodes motors size 2
   logistic 0.1

weights lr-lr
0 -10 
-10 0

weights lr-motors
1 0
0 1

# network
# feedforward
# nodes input size 10
# nodes output size 1 logistic 0.1 1
# weights input-output uniform -.1 1
