#ifndef MINT_THREADS_H

struct mint_network;

/** A network init op to set multithreading on various network
    operations.

    Parameters: 0: Maximum number of threads to use (default 1). 

                1: Whether to multi-thread network spreading, i.e.,
                   assign to different threads weight operations that
                   can be performed in parallel. 

                2: Whether to multi-thread matrix-vector
                   multiplications, assigning different strips of the
                   matrix to different threads. 

NOTES: 1) Multi-threading network spreading does nothing when using a
network operate op (such as asynchronous operation) that does not use
spreading. 2) Enabling multithreading on matrix-vector multiplication
overrides changes in the operate op of weights. That is, it will
perform good old-fashioned matrix vector multiplications to calculate
node input, regardless of whether you have written your own fancy
function to perform weight matrix operations. */
void mint_network_init_threads( struct mint_network *net, float *p );

#endif /* MINT_THREAD_H */

