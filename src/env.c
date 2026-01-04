#include "../include/env.h"

// #include <ctype.h>
#include <stdbool.h>
// #include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/cell.h"
#include "../include/operations.h"
#include "../include/env.h"

// #include "../include/pval.h"


#define HASH_SIZE 256


static void _populate_built_ins(void){
    hashtable *ht = env_global->table;
    for(int i = 0; i < op_len; i++){
        pval *pv = func_ptr_helper(operations[i].fn);
        binding *b = binding_new(operations[i].name, cell_new(pv));
        b->is_protected = true;
        hash_probe_insert(ht, b);
    }
}

env *env_global = NULL;

env *init_global(void){
    env_global = malloc(sizeof(*env_global));
    env_global->parent = NULL;
    env_global->table = _init_hashtable();
    _populate_built_ins();
    return env_global;
}

env *env_new(env *parent){
    env *e = malloc(sizeof(*e));
    e->parent = parent;
    e->table = _init_hashtable();
    return e;
}
