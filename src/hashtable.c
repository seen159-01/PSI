#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/env.h"
#include "../include/cell.h"
// #include "../include/pval.h"

static binding *TOMBSTONE = (binding*)1;

#define HASH_SIZE 256


hashtable *_init_hashtable(void){
    hashtable *ht = malloc(sizeof(hashtable));
    ht->capacity = HASH_SIZE;
    ht->bindings = calloc(HASH_SIZE, sizeof(binding *)); // alllocate hash-size number of pointers. Using calloc cuz it'll set 'em to null.
    return ht;
}

unsigned int hash(char *key){ //djb2
    size_t h = 5381;
    int c;

    while ((c = *key++)) {
        h = ((h << 5) + h) + c;  
    }
    return h;
}

binding *hash_find(hashtable *ht, char *name) {
    int idx = hash(name) % ht->capacity;
    int offset = idx;

    while (true) {
        binding *b = ht->bindings[offset];
        if (b == NULL) {
            return NULL;
        } else if (b != TOMBSTONE && (strcmp(b->name, name)) == 0) {
            return b;
        }

        offset = (offset + 1) % ht->capacity;
    }
}

bool hash_probe_insert(hashtable *ht, binding *b) {
    
    int idx = (hash(b->name)) % ht->capacity;
    binding *bc;
    int offset = idx;
    int ts = -1;

    while (true) {
        bc = ht->bindings[offset];

        if (bc == NULL) {
            if(ts != -1){
                ht->bindings[ts] = b;
            } else{
                ht->bindings[offset] = b;
            }
            return true;

        } else if (bc == TOMBSTONE) {
            if (ts == -1) {
                ts = offset;
            }
        } else if (strcmp(bc->name, b->name) == 0) {
            cell_retain(b->cell);
            cell_release(&(bc->cell));
            bc->cell = b->cell;
            binding_free(&b);
            return true;
        }
        offset = (offset + 1) % ht->capacity;
    }

    return false;
}

bool hash_probe_delete(hashtable *ht, char *in){
    binding *bc;
    int offset = hash(in) % ht->capacity;

    while((bc = ht->bindings[offset]) != NULL){
        if (bc == TOMBSTONE) {
            offset = (offset + 1) % ht->capacity;
            continue;
        } else if(strcmp(bc->name, in) == 0){
            binding_free(&ht->bindings[offset]);
            ht->bindings[offset] = TOMBSTONE;
            return true;
        }  
        offset = (offset + 1) % ht->capacity;
    }
    return false;
}
