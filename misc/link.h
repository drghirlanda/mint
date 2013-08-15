#include <stdio.h>

enum mint_link_type {
  mean = 0,
  sum = 1
};

struct mint_link;

struct mint_link *mint_link_new( int nsource,
				 mint_link_type type,
				 int param,
				 int delay,
				 float *value );

void mint_link_dup( struct mint_link *dst, struct mint_link *src );

void mint_link_save( struct mint_link *, FILE *f );

struct mint_link *mint_link_load( FILE *f );

float mint_link_update( struct mint_link *, struct mint_network * );

int mint_link_get_source( struct mint_link * );

void mint_link_set_source( struct mint_link *, int );

mint_link_type mint_link_get_type( struct mint_link * );

void mint_link_set_type( struct mint_link *, mint_link_type );

int mint_link_get_delay( struct mint_link * );

void mint_link_set_delay( struct mint_link *, int );

