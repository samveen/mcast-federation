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
    size_t size;
    peerinfo_t **table;
};

struct peerinfo_s {
	message_t *value;
	struct peerinfo_s *next;
        
	in_addr_t key; // IPv4 Forever 
};

/* Create a new hashtable. */
hashtable_t *ht_create( size_t size ) {

    hashtable_t *hashtable = NULL;
    size_t i;

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

/* Hash the key into the correct hash bin . */
size_t ht_hash( hashtable_t *hashtable, in_addr_t key ) {

    return key % hashtable->size; // TODO: Smarter hashing as this is an IP address
}

/* Create a key-value pair. */
peerinfo_t *ht_newpair(in_addr_t key, const message_t * const value ) {
    peerinfo_t *newpair;

    if( ( newpair = malloc( sizeof( peerinfo_t ) ) ) == NULL ) {
        return NULL;
    }

    newpair->key = key;
    newpair->value = deep_copy(value); // always use a local deep copy
    newpair->next = NULL;

    return newpair;
}

void ht_crush(peerinfo_t *pair) {
    deep_free(pair->value);  // We always use local copies
    free(pair);
}

/* Insert a key-value pair into a hash table. */
void ht_set(hashtable_t *hashtable, in_addr_t key, const message_t * const value) {
    size_t bin = 0;
    peerinfo_t *newpair = NULL;
    peerinfo_t *next = NULL;
    peerinfo_t *last = NULL;

    bin = ht_hash( hashtable, key );

    next = hashtable->table[ bin ];

    while( next != NULL && key > next->key ) {
        last = next;
        next = next->next;
    }

    /* There's already a pair.  Let's replace that value. */
    if( next != NULL && key == next->key ) {

        deep_free(next->value); // We always use local copies
        next->value = deep_copy(value);

        /* Nope, couldn't find it. Time to grow a pair. */
    } else {
        newpair = ht_newpair( key, value );

        /* We're at the start of the linked list in this bin. */
        if( next == hashtable->table[ bin ] ) { // BOTH are NULL
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
const message_t * ht_get( hashtable_t *hashtable, in_addr_t key) {
    size_t bin = 0;
    peerinfo_t *pair;

    bin = ht_hash( hashtable, key );

    /* Step through the bin, looking for our value. */
    pair = hashtable->table[ bin ];
    while( pair != NULL && key > pair->key ) {
        pair = pair->next;
    }

    /* Did we actually find anything? */
    if( pair == NULL || key != pair->key ) {
        return NULL;
    } else {
        return pair->value;
    }

}

/* Remove an entry from a hash table. (by key) */
void ht_remove(hashtable_t * hashtable, in_addr_t key) {
    size_t bin = 0;
    peerinfo_t *pair = NULL;
    peerinfo_t *last = NULL;

    bin = ht_hash( hashtable, key );

    pair = hashtable->table[ bin ];

    while( pair != NULL && (key > pair->key)) {
        last = pair;
        pair = pair->next;
    }

    /* If there's the entry, remove the entry. */
    if( pair != NULL && key == pair->key) {
        if(hashtable->table[bin] == pair)
            hashtable->table[bin] = pair->next;
        else
            last->next = pair->next;

        /* Crush the pair we grew earlier. */
        ht_crush(pair);
    }
}

in_addr_t get_next_key(hashtable_iterator_t* it) {
    in_addr_t key;
    if(it->current_entry!=NULL) {
        key=it->current_entry->key;
        it->current_entry=it->current_entry->next;
        return(key);
    } else if (it->current_bin < it->hashtable->size) {
        ++(it->current_bin);
        it->current_entry=it->hashtable->table[it->current_bin];
        return get_next_key(it);
    } else
        return 0;
}

hashtable_iterator_t * get_iterator(hashtable_t* hashtable) {
    hashtable_iterator_t *it;
    if( ( it = malloc(sizeof(hashtable_iterator_t)) ) == NULL ) {
        return NULL;
    }
    it->hashtable=hashtable;
    it->current_bin=0;
    it->current_entry=it->hashtable->table[it->current_bin];
    it->next=get_next_key;

    return (it);
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

