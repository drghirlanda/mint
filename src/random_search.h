struct mint_network;

/** \file random_search.h

    \brief Simple genetic algorithm */

/** Run a simple genetic algorithm starting with a given network. At
    each iteration, one mutated network is created, the performance of
    both the original and the mutated network is assessed, and the
    better network is retained for next iteration. The best network
    will be available as net1 after the function returns.

    \param net1 The initial network 

    \param perf Function that performs network evaluation and returns
    the result as a float. The function attempts to maximize this
    function. 

    \param mut Function that mutates a network. 

    \param target The algorithm will stop if this value of performance
    is reached 

    \param max The algorithm will stop after max iterations

    \return The performance value attained.
*/
float 
mint_random_search( struct mint_network *net1,  /* initial network */
		    float (*perf)( struct mint_network * ), /* perf function */
		    void (*mut)( struct mint_network * ),   /* mutation rule */
		    float target,             /* target performance */
		    long max                  /* max iterations */
		    );

