#ifndef MINT_RANDOM_H
#define MINT_RANDOM_H

/** \file random.h

   \brief Pseudo-random number generation 
*/

/** Initalize generator */
void mint_random_seed( unsigned long );

/** Uniformly distributed numbers in [0,1[ */
float mint_random( void );

/** Uniformly distributed numbers in [min,max[ */
float mint_random_uniform( float min, float max );

/** Normally distributed numbers with given mean and standard deviation */
float mint_random_normal( float mean, float stdev );

/** Random integers between min and max (max excluded) */
int mint_random_int( int min, int max );

/** Rearrange the n integers in perm to create a random permutation. */
void mint_random_permutation( int *perm, int n );

/** Returns the number of successes in n Bernoulli trials (coin
   tosses) each with a probability of success of p */
int mint_random_binomial( float p, int n );

/** Store in r the results of choosing n times from len alternatives,
    where alternative i has probaiblity p[i]; the p array can contain
    relative probabilities. If p==0, uniform probabilities are used. */
void mint_random_multinomial( float *p, int *r, int len, int n );

/** Stores in the array subset n random integers between min and max,
    sorted in ascending order. The subset array must have already been
    allocated. */
void mint_random_subset( int *subset, int n, 
			 int min, int max );

#endif

