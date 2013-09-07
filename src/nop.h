#ifndef NLIB_H
#define NLIB_H

#include "nodes.h"
#include <stddef.h>

/** \file nop.h 

    \brief Node operations.
    
    We provide a few update functions for nodes.
*/

/** Identity activation function: output = total input. 

    State variables: none.

    Parameters: none. */
void mint_node_identity( mint_nodes n, int min, int max, float *p );

/** Sigmoidal non-linearity. 

    State variables: none. 

    Parameters: 0: node output when input is zero.
                1: maximum slope of the sigmoid function.

    NOTE: For speed, the nonlinearity is not a logistic function,
    i.e., we do not use an exponential. Rather, we piece two branches
    of hyperbola together.  */
void mint_node_sigmoid( mint_nodes n, int min, int max, float *p );

/** Leaky integrator.  with time constant T=param[0] and leak
    L=param[1]. The update performed is: output += ( input -
    L*old_output)/T.

    State variables: 1, to remember output value between updates
                     (old_output in equation)..

    Parameters: 0: Time constant (T in equation above).
                1: Leak (L in equation).
		2: Index of state variable to use (>=2, default 2). */
void mint_node_integrator( mint_nodes n, int min, int max, float *p );

/** A spiking neuron model from Izikevich EM, IEEE Transactions on
    Neural Networks 14:1569-1572 (2003). Requires 2 state variables, v
    and u, and 4 parameters a,b,c,d. The dynamics is:

    v += 0.04 * v*v + 5*v + 140*u + n[0][i];
    
    u += p[0]*( p[1]*v - u );
  
    where p is the parameter array. If v reaches 30, then node output
    is set to 1 (a spike is generated), v is reset to p[2] and u is
    reset to u + p[3]. If v does not reach 1, the values of v and u
    are saved (internal state dynamics) and node output is set to
    zero. (The threshold value is 30 as a models of thresholds of
    biological neurons, measured in mV.) 

    State variables: 2, to store the values of v and u.

    Parameters: 0-3: The a,b, c, and d parameters in the equations
                     above. The defaults are 0.02, 0.2, -65, 8,
                     corresponding to a pyramidal neuron in the
                     mammalian cortex.
		4: Index of state variable used to store v values
		(>=2, default 2).
		5: Index of state variable used to store u values
		(>=2, default 3, must be different from parameter 4). */
void mint_node_izzy( mint_nodes n, int min, int max, float *p );

/** Adds a normally distributed number to a state variable. Note that
    noise added to input, or to a state variable, should come before
    the transfer function, while noise added to output should come
    after (See example/integrator). 

    State variables: none required, can work on any state variable
                     (see Parameters).  

    Parameters: 0: State variable to add noise to.
                1: Mean of normal distribution (default 0).
		2: Standard deviation of normal distribution (default
                   0.01).  

     NOTE: You can use this op to simulate tonically active neurons,
     just use it with a positive mean activity and the desired
     standard deviation (0 also works). */
void mint_node_noise( mint_nodes n, int min, int max, float *p );

/** Restrict a node state variable to a given range. The default
    behavior is to restrict node output between 0 and 1. 

    State variables: none required, can work on any state variable (see
                     Parameters).  

    Parameters: 0: State variable to bound (default 1, i.e., node output). 
                1: Minimum value (default 0).
		2: Maximum value (default 1). */
void mint_node_bounded( mint_nodes n, int min, int max, float *p );

/** A counter counts the number of updates since a state variable
    (default: node output) has been equal to or above a threshold
    value. 

    State variables: 1 required, to store the counter. Works on any
                     other state variable. 

    Parameters: 0: State variable to monitor (default 1, i.e., node
                   output). 

		1: Threshold value.

		2: Index of state variable that stores the counter
		   (must be different from parameter 0). */
void mint_node_counter( mint_nodes n, int min, int max, float *p );

/** Poissonian source of spikes. Sets node output to 1 or 0 at random,
    with a given mean value.

    State variables: none required.

    Parameters: 0: Mean spike rate in Hz, assuming every network
                   update corresponds to 1 ms of real time (default 5
                   Hz). */
void mint_node_spikes( mint_nodes n, int min, int max, float *p );

/** This is a geometry op that sets nodes geometry to a rectangular
    array with a given number of rows.

    State variables: none required.

    Parameters: 0: Number of rows. Must divide node size exactly as
                   the number of column is calculated as . There is no
                   default, the parameter must be given.

    NOTE: This op does not do anything per se but storing the number
    of rows. It can be used by other ops that need geometry
    information, such as those that interface nodes with images or the
    camera, and those establishing weight matrices that take into
    account the spatial arrangement of nodes. */
void mint_node_rows( mint_nodes n, int min, int max, float *p );

/** Specify the number of state variables for a nodes object. 

    State variables: none required. 
    Parameters: 0: The number of states. */
void mint_node_states( mint_nodes n, int min, int max, float *p );

/** Specify the number of nodes in a nodes object. 

    State variables: none required. 
    Parameters: 0: The number of nodes. */
void mint_node_size( mint_nodes n, int min, int max, float *p );

/** Marks a node group as being involved in image processing (the
    actual op names are "red", "green", "blue", and "gray" depending
    on which image channel the node group should be associated
    with. See also documentation in image.h and camera.h.

    State variables: non required.  
    Parameters: 0: A fixed multiplier for image input (default 1).
                1: The variable to which image information is added
                   (default: 1, i.e., node output). 
		   
    NOTE: More than one color component can be added to a node
    group. For example, "nodes n1 size 100 red green" would create
    nodes that receive both the red and the green
    component. Furthermore, the different components can be wighed
    differently: "nodes n1 size 100 red .5 green .5 blue -1" would
    give nodes that receive the sum of the red and green channels
    (half value of each) minus the blue channel. This would make these
    nodes operate as a yellon-blue opponent color process. Lastly, an
    additional parameter can be given to direct input to a given state
    variable. Thus "nodes n1 size 100 red 1 0" would direct the red
    channel to node input rather than node output. */
void mint_node_color( mint_nodes n, float *p );
 
#endif
