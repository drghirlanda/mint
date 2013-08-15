#include "link.h"
#include "utils.h"
#include <stdlib.h>

struct mint_link {
  int nfrom;         /* source group of linked variable */
  mint_link_type type; /* type of link, mean or sum */
  int param;           /* which update param is linked */
  int delay;           /* transmission delay */
  float *value;        /* value buffer (of size "delay") */
};

struct mint_link *mint_link_new( int nfrom,
				 int nto
				 mint_link_type type,
				 int param,
				 int delay ) {
  struct mint_link *l;
  /* FIX these checks are simplistic because it is possible for values
     >0 to be invalid */
  mint_check( param>=0, "param<0" );
  mint_check( delay>=0, "delay<0" );
  mint_check( nsource>=0, "source<0" );
  l = malloc( sizeof(struct mint_link) );
  l->nsource = nsource;
  l->type = mint_link_type;
}

struct mint_link *void mint_link_dup( struct mint_link *l1 ) {
  struct mint_link *l2;
  l2 = mint_link_new( l1->nsource, l1->type, l1->delay,

void mint_link_save( struct mint_link *, FILE *f );

struct mint_link *mint_link_load( FILE *f );

float mint_link_update( struct mint_link *, struct mint_network * );

int mint_link_get_source( struct mint_link * );

void mint_link_set_source( struct mint_link *, int );

mint_link_type mint_link_get_type( struct mint_link * );

void mint_link_set_type( struct mint_link *, mint_link_type );

int mint_link_get_delay( struct mint_link * );

void mint_link_set_delay( struct mint_link *, int );

