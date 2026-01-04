#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/cell.h"
#include "../include/pval.h"

binding *binding_new(char *name, cell *c) {
  binding *b = malloc(sizeof(*b));
  b->name = strdup(name);
  cell_retain(c);
  b->cell = c;
  b->is_protected = false;
  return b;
}

void binding_free(binding **b) {
  if (!b || !*b)
    return;
  free((*b)->name);
  cell_release(&((*b)->cell));
  free(*b);
  *b = NULL;
}

void cell_retain(cell *c) {
  if (!c)
    return;
  c->refcnt++;
}

void cell_release(cell **c) {
  if (!c || !*c)
    return;
  if ((*c)->refcnt <= 0)
    return;

  (*c)->refcnt--;

  if ((*c)->refcnt == 0) {
    pval_release((*c)->value);
    free(*c);
    *c = NULL;
  }
}
cell *cell_new(pval *pv) {
  cell *out = malloc(sizeof(*out));
  out->value = pv;
  out->refcnt = 1;
  pval_retain(pv);
  return out;
}
void cell_set(cell *c, pval *pv) {
  if (!c)
    return;

  if (c->value != pv) {
    pval_release(c->value);
    pval_retain(pv);
  }

  c->value = pv;
}
pval *cell_get(cell *c) { return c->value; }

void cell_free(cell *c) {
  if (!c)
    return;
  pval_release(c->value);
  free(c);
}
