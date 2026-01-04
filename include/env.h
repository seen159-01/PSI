#include <stdlib.h>
#include <stdbool.h>
#ifndef ENV_H
#define ENV_H

typedef struct pval pval;
typedef struct binding binding;
typedef struct cell cell;




typedef struct hashtable {
    size_t capacity;
    binding **bindings;

} hashtable;

typedef struct env {
    hashtable *table;
    struct env *parent;
} env;

extern env *env_global;

env *init_global(void);
env *env_new(env *parent);


hashtable *_init_hashtable(void);
unsigned int hash(char *key);
bool hash_probe_insert(hashtable *ht, binding *b);
bool hash_probe_delete(hashtable *ht, char *in);
binding *hash_find(hashtable *ht, char *name);

#endif