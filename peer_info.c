/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * Upstream: https://gist.github.com/tonious/1377667, Copyright 2011, Tony Thompson <tonious@github>
 *
 * peer_info.c
 *
 * Naive Hashtable implementation to store peer info
 */

#include <limits.h>

#include "common.h"

#include "peer_info.h"

#include "message.h"

struct hashtable_s {
	int size;
	peerinfo_t **table;	
};

typedef struct hashtable_s hashtable_t;


/* Create a new hashtable. */
hashtable_t *ht_create( int size ) {

	hashtable_t *hashtable = NULL;
	int i;

	if( size < 1 ) return NULL;

	/* Allocate the table itself. */
	if( ( hashtable = malloc( sizeof( hashtable_t ) ) ) == NULL ) {
		return NULL;
	}

	/* Allocate pointers to the head nodes. */
	if( ( hashtable->table = malloc( sizeof( peerinfo_t * ) * size ) ) == NULL ) {
		return NULL;
	}
	for( i = 0; i < size; i++ ) {
		hashtable->table[i] = NULL;
	}

	hashtable->size = size;

	return hashtable;	
}

/* Hash a string for a particular hash table. */
int ht_hash( hashtable_t *hashtable, in_addr_t key ) {

	return key % hashtable->size; // TODO: Smarter hashing as this is an IP address
}

/* Create a key-value pair. */
peerinfo_t *ht_newpair(in_addr_t key, message_t *value ) {
	peerinfo_t *newpair;

	if( ( newpair = malloc( sizeof( peerinfo_t ) ) ) == NULL ) {
		return NULL;
	}

	newpair->key = key;
	newpair->value = value; // TODO: create a local copy instead?
	newpair->next = NULL;

	return newpair;
}

/* Insert a key-value pair into a hash table. */
void ht_set(hashtable_t *hashtable, in_addr_t key, message_t *value ) {
	int bin = 0;
	peerinfo_t *newpair = NULL;
	peerinfo_t *next = NULL;
	peerinfo_t *last = NULL;

	bin = ht_hash( hashtable, key );

	next = hashtable->table[ bin ];

	while( next != NULL && next->key != 0  && key > next->key ) {
		last = next;
		next = next->next;
	}

	/* There's already a pair.  Let's replace that string. */
	if( next != NULL && next->key != 0 && key == next->key ) {

		free(next->value);
		next->value = value;

	/* Nope, could't find it. Time to grow a pair. */
	} else {
		newpair = ht_newpair( key, value );

		/* We're at the start of the linked list in this bin. */
		if( next == hashtable->table[ bin ] ) {
			newpair->next = next;
			hashtable->table[ bin ] = newpair;
	
		/* We're at the end of the linked list in this bin. */
		} else if ( next == NULL ) {
			last->next = newpair;
	
		/* We're in the middle of the list. */
		} else  {
			newpair->next = next;
			last->next = newpair;
		}
	}
}

/* Retrieve a key-value pair from a hash table. */
message_t *ht_get( hashtable_t *hashtable, in_addr_t key ) {
	int bin = 0;
	peerinfo_t *pair;

	bin = ht_hash( hashtable, key );

	/* Step through the bin, looking for our value. */
	pair = hashtable->table[ bin ];
	while( pair != NULL && pair->key != 0 && key > pair->key ) {
		pair = pair->next;
	}

	/* Did we actually find anything? */
	if( pair == NULL || pair->key == 0 || key > pair->key ) {
		return NULL;
	} else {
		return pair->value;
	}
	
}


/*int main( int argc, char **argv ) {

	hashtable_t *hashtable = ht_create( 65536 );

	ht_set( hashtable, "key1", "inky" );
	ht_set( hashtable, "key2", "pinky" );
	ht_set( hashtable, "key3", "blinky" );
	ht_set( hashtable, "key4", "floyd" );

	printf( "%s\n", ht_get( hashtable, "key1" ) );
	printf( "%s\n", ht_get( hashtable, "key2" ) );
	printf( "%s\n", ht_get( hashtable, "key3" ) );
	printf( "%s\n", ht_get( hashtable, "key4" ) );

	return 0;
}*/
