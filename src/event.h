#ifndef MINT_EVENT_H
#define MINT_EVENT_H

struct mint_op;
struct mint_network;

int mint_event_new( int i, struct mint_op *op );

int mint_event_del( int i );

int mint_event_size( void );

void mint_poll_event( struct mint_network *net );

#endif
