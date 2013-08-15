/* $Id $ */

/* reference-counted memory blocks */

#ifndef MINT_MEMBLK_H
#define MINT_MEMBLK_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef void *mint_memblk;

mint_memblk mint_memblk_new( size_t bytes );

void mint_memblk_cpy( mint_memblk , const mint_memblk );

mint_memblk mint_memblk_dup( const mint_memblk );

void mint_memblk_del( mint_memblk );

#endif
