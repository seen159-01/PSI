#ifndef CELL_H
#define CELL_H

typedef struct pval pval;
typedef struct cell cell;


typedef struct binding{
    char *name;
    cell *cell;
    bool is_protected;
} binding;

typedef struct cell{
    pval *value;
    int refcnt;
}cell;





binding *binding_new(char *name, cell *c);
void binding_free(binding **b);
void cell_retain(cell *c);
void cell_release(cell **c);
cell *cell_new(pval *initial);
void cell_set(cell *c, pval *value);
pval *cell_get(cell *c);
void cell_free(cell *c);


#endif