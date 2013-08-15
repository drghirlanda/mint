#ifndef NLIB_H
#define NLIB_H

#include "nodes.h"
#include <stddef.h>

/** \file nop.h 

    \brief Node operations.
    
    We provide a few update functions for nodes.
*/

/** Identity activation function: output = total input */
void mint_node_identity( mint_nodes n, int min, int max, float *p );

/** Sigmoidal non-linearity. Takes to parameters: param[0] is the
   output to zero input; param[1] is the maximum slope can be set. For
   speed, the nonlinearity is not a logistic function, i.e., we do not
   use an exponential. Rather, we piece two branches of hyperbola
   together.  */
void mint_node_sigmoid( mint_nodes n, int min, int max, float *p );

/** Leaky integrator with time constant T = param[0] and leak L =
    param[1]. The update is output += ( input - L * old_output) /
    T. One state variable is needed to remember the old output between
    updates. */
void mint_node_integrator( mint_nodes n, int min, int max, float *p );

/** A spiking neuron model from Izikevich EM, IEEE Transactions on
    Neural Networks 14:1569-1572 (2003). Requires 2 state variables, v
    and u, and 4 parameters a,b,c,d. The dynamics is:

    v += 0.04 * v*v + 5*v + 140*u + n[0][i];
    
    u += p[0]*( p[1]*v - u );
  
    If v reaches 30, then node output is set to 1 (a spike is
    generated), v is reset to p[2] and u is reset to u + p[3]. If v
    does not reach 1, the values of v and u are saved (internal state
    dynamics) and node output is set to zero. (The threshold value is
    30 as a models of thresholds of biological neurons, measured in
    mV.)

*/
void mint_node_izzy( mint_nodes n, int min, int max, float *p );

/** Provides tonically active neurons with normally distributed
   activity. Activity is independent of input and has given mean and
   standard deviation (parameters 0 and 1 of the update
   rule). Requires 0 state variables and 2 parameters. The defaults
   are mean of 0.1 and standard deviation of 0.01. */
void mint_node_tonic( mint_nodes n, int min, int max, float *p );

/** Adds random number with given mean and standard deviation
    (parameters 1 and 2) to the variable given in parameter 0. Note
    that noise added to input, or to a state variable, should come
    before the transfer function, while noise added to output should
    come after (See example/integrator). */
void mint_node_noise( mint_nodes n, int min, int max, float *p );

/** Restrict variabe p[0] to the range [ p[1], p[2] ]. The default
    behavior is to restrict node output between 0 and 1. */
void mint_node_bounded( mint_nodes n, int min, int max, float *p );

/** A counter counts the number of updates since the neuron output has
    been equal to or above a threshold value (parameter 0). The
    counter is stored in the state variable whose index is given in
    parameter 1. */
void mint_node_counter( mint_nodes n, int min, int max, float *p );

/** Poissonian source of spikes. Requires 0 state variables and 1
    parameter, the spike rate (in Hz, assuming every network update
    corresponds to 1 ms of real time). Default spike rate is 5 Hz. */
void mint_node_spikes( mint_nodes n, int min, int max, float *p );

#endif
