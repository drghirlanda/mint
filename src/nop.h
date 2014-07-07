#ifndef NLIB_H
#define NLIB_H

#include "nodes.h"
#include <stddef.h>

/** \file nop.h 

    \brief This file contains node operations, that is, functions that
    operate on `mint_nodes` to update their state.
    
    These functions typically have one or more parameters to configure
    their behavior. These parameters are of two kinds:

    1. Parameters pertaining to the node update proper, such as the
    slope of a logistic function or the time constant of a leaky
    integrator.

    2. Parameters indicating on which state variables the function
    operates. For example, the `noise` operation can be used to add
    random numbers to the input of a `mint_nodes` variable, to the
    output, or to any other state variable.

*/

/** Logistic tansfer function, implementing the transformation output =
    1 / ( 1 + exp(-slope*( input - offset ) ) ).

    - Name: `logistic'
    - State variables required: none 
    - Parameters: 
      + 0: output when input is zero (default 0.1)
      + 1: slope (default 1)
      + 2: input state variable (default 0)
      + 3: output state variable (default 1)
*/
void mint_node_logistic( mint_nodes n, int min, int max, float *p );

/** Leaky integrator. Given a time constant T and a leak parameter L,
    the update performed is: `output += ( input - L*old_output) / T`.

    - Name: `integrator`
    - State variables required: none
    - Parameters: 
      + 0: Time constant (T in equation above, default 1)
      + 1: Leak (L in equation above, default 0)
      + 2: Index of input variable (default 0)
      + 3: Index of output variable (default 1)
*/
void mint_node_integrator( mint_nodes n, int min, int max, float *p );

/** A spiking neuron model from Izikevich EM, IEEE Transactions on
    Neural Networks 14:1569-1572 (2003). Requires 2 state variables,
    noted v and u below, and 4 parameters, noted a,b,c,d. The dynamics
    is:

    v += 0.04 * v*v + 5*v + 140*u + input;
    
    u += a*( b*v - u );
  
    where p is the parameter array. If v reaches 30, then the `output`
    variable is set to 1 (a spike is generated), v is reset to the
    value c and u is reset to u + d. If v does not reach 30, the
    values of v and u are saved as internal states and output is set
    to zero. (The threshold value is 30 as a model of thresholds of
    biological neurons, measured in mV.)

    - Name: `izzy`
    - State variables required: 2, to store the values of v and u
    - Parameters: 
      + 0-3: The a,b, c, and d parameters in the equations above. 
        The defaults are 0.02, 0.2, -65, 8, corresponding to a 
	pyramidal neuron in the mammalian cortex
      + 4: Index of input variable (default 0)
      + 5: Index of output variable (default 1)
      + 6: Index of state variable used to store v values 
        (>=2, default 2)
      + 7: Index of state variable used to store u values
        (>=2, default 3) 
*/
void mint_node_izzy( mint_nodes n, int min, int max, float *p );

/** Adds a normally distributed number to a state variable. Note that,
    because operations are executed in the order they are listed,
    noise added to input, or to a state variable, should come before
    the transfer function, while noise added to output should come
    after (See example/integrator).

    - Name: `noise`
    - State variables required: none
    - Parameters: 
      + 0: Mean of normal distribution (default 0)
      + 1: Standard deviation of normal distribution (default
        0.01)
      + 2: State variable to add noise to

     NOTE: You can use this op to simulate tonically active neurons,
     just use it with a positive mean activity and the desired
     standard deviation (0 works). */
void mint_node_noise( mint_nodes n, int min, int max, float *p );

/** Restrict a node state variable to a given range. The default
    behavior is to restrict node output between 0 and 1. 

    - Name: `bounded`
    - State variables required: none
    - Parameters: 
      + 0: Minimum value (default 0)
      + 1: Maximum value (default 1)
      + 2: State variable to bound (default 1, i.e., node output)
*/
void mint_node_bounded( mint_nodes n, int min, int max, float *p );

/** A counter counts the number of updates ("time steps") since a
    state variable (default: node output) has been equal to or above a
    threshold value.

    - Name: `counter`
    - State variables required: 1, to store the counter
    - Parameters: 
      + 0: Threshold value (default 1)
      + 1: State variable to monitor (default 1, i.e., node output)
      + 2: State variable that stores the counter
*/
void mint_node_counter( mint_nodes n, int min, int max, float *p );

/** Poissonian source of spikes. Sets a state variable to 1 or 0 at
    random, with a given mean value (in Hz).

    - Name: `spikes`
    - State variables required: none
    - Parameters: 
      + 0: Mean spike rate in Hz (assuming every network update
        corresponds to 1 ms of real time (default 5 Hz)
      + 1: Index of state variable to set (default 1, i.e., node 
        output)
*/
void mint_node_spikes( mint_nodes n, int min, int max, float *p );

/** This is operation merely stores a number that other operations can
    query, and which is used to effectively endow `mint_nodes` with
    the geometry of a rectangular array with a given number of
    rows. This is used, for example, when pasting images onto
    `mint_nodes` or when building weight matrices that take the
    spatial arrangement of nodes into account.

    - Name: rows
    - State variables required: none
    - Parameters: 
      + 0: Number of rows. Must divide node size exactly as
        the number of column is calculated as size/rows. 
	There is no default
*/
void mint_node_rows( mint_nodes n, int min, int max, float *p );

/** Specify the number of state variables to be created for a nodes
    object.

    - Name: 'states'
    - State variables required: none
    - Parameters: 
      + 0: The number of states. There is no default
*/
void mint_node_states( mint_nodes n, int min, int max, float *p );

/** Specify the size (number of nodes) of a `mint_nodes` object. 

    - Name: `size`
    - State variables required: none
    - Parameters: 
      + 0: The number of nodes. There is no default
*/
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

/** This operation simulates habituation, and can be used on any state
    variable. A "habituation" variable, h, is created and evolves
    according to

      T Delta h = a * y - h

    where T is a time constant and a the maximum level of habituation
    (see parameters below) and y is the current value of the state
    variable for which habituation is set up. The value of h is then
    subtracted from this state variable. The resulting dynamics is
    that habituation increases when y is high, which ends up
    detracting from y itself. One use of this operation is to
    habituate node output to simulate neuron `fatigue`.

    - Name: `habituation`
    - State variables required: 1, to store the habituation level
      (value of h in the above equation). Obviously, if you want to
      habituate a state variable that is neither  node input nor 
      output, that state variable must exist...
    - Parameters: 
      + 0: Time constant (T in equation above, default 1)
      + 1: Maximum habituation level, between 0 and 1 (default 0)
      + 2: State variable storing the habituation level (default 2)
      + 3: State variable that habituates (default 1, i.e., node output)
 */
void mint_node_habituation( mint_nodes n, int min, int max, float *p );
 
void mint_node_identity( mint_nodes n, int min, int max, float *p );

#endif
