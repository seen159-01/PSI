#define _POSIX_C_SOURCE 200809L
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/SinglyLinkedList.h"
#include "../include/operations.h"

operation operations[] = {
    {"+", op_add},         {"-", op_sub},
    {"*", op_mul},         {"/", op_div},
    {"=", op_eq},          {"%", op_add},
    {"<", op_less},        {">", op_greater},
    {"!=", op_not},        {"&", op_add},
    {"$", op_add},         {"quit", op_quit},
    {"not", op_not},       {">=", op_greater_equal},
    {"<=", op_less_equal},
};

int op_len = sizeof(operations) / sizeof(operations[0]);

// this file defines the actual logic for the operations of + / * etc

pval *func_ptr_helper(op_func func) {
  pval *fp = malloc(sizeof(*fp));
  fp->type = PVAL_FUNCTION;
  fp->value.val_function = func;
  fp->refcnt = 1;
  return fp;

  // int op_len = sizeof(operations) / sizeof(operations[0]);
  // op_func func;
  // for (int i = 0; i < op_len; i++) {
  //     if (strcmp(op, operations[i].name) == 0){
  //         pval *pv = malloc(sizeof(*pv));
  //         pv->type = PVAL_FUNCTION;
  //         pv->value.val_function = operations[i].fn;
  //         pv->refcnt = 1;
  //         return pv;
  //     }
  // }
  // return NULL;
}

pval *op_less_equal(pval *args) {
  linked_node *curr;
  if (args->value.val_list->head == NULL) {
    return pval_bool(true);
  }
  curr = args->value.val_list->head;
  while (curr->next) {
    if (curr->data->type == curr->next->data->type &&
        curr->data->type == PVAL_NUMBER) {
      if (curr->data->value.val_number > curr->next->data->value.val_number) {
        return pval_bool(false);
      }
    }
    curr = curr->next;
  }
  return pval_bool(true);
}

pval *op_greater_equal(pval *args) {
  linked_node *curr;
  if (args->value.val_list->head == NULL) {
    return pval_bool(true);
  }
  curr = args->value.val_list->head;
  while (curr->next) {
    if (curr->data->type == curr->next->data->type &&
        curr->data->type == PVAL_NUMBER) {
      if (curr->data->value.val_number < curr->next->data->value.val_number) {
        return pval_bool(false);
      }
    }
    curr = curr->next;
  }
  return pval_bool(true);
}

pval *op_not(pval *args) {
  pval *pv = args->value.val_list->head->data;
  if (args->value.val_list->size != 1) {
    return pval_error(NULL, "arity-error: 'not' requires 1 arg");
  } else if (pv->type == PVAL_BOOL && pv->value.val_bool == false) {
    return pval_bool(true);
  } else {
    return pval_bool(false);
  }
}
pval *op_less(pval *args) {
  linked_node *curr;
  if (args->value.val_list->head == NULL) {
    return pval_bool(true);
  }
  curr = args->value.val_list->head;
  while (curr->next) {
    if (curr->data->type == curr->next->data->type &&
        curr->data->type == PVAL_NUMBER) {
      if (curr->data->value.val_number >= curr->next->data->value.val_number) {
        return pval_bool(false);
      }
    }
    curr = curr->next;
  }
  return pval_bool(true);
}
pval *op_greater(pval *args) {
  linked_node *curr;
  if (args->value.val_list->head == NULL) {
    return pval_bool(true);
  }
  curr = args->value.val_list->head;
  while (curr->next) {
    if (curr->data->type == curr->next->data->type &&
        curr->data->type == PVAL_NUMBER) {
      if (curr->data->value.val_number <= curr->next->data->value.val_number) {
        return pval_bool(false);
      }
    }
    curr = curr->next;
  }
  return pval_bool(true);
}

pval *op_add(pval *args) {
  if (args->type != PVAL_LIST)
    return pval_error(args, "type-error: expected list");

  int64_t result = 0;
  linked_node *curr = args->value.val_list->head;

  while (curr != NULL) {
    pval *elem = curr->data;
    if (elem->type != PVAL_NUMBER) {
      return pval_error(elem, "type-error: non-number in +");
    }
    result += elem->value.val_number;
    curr = curr->next;
  }

  return pval_number(result);
}

pval *op_sub(pval *args) {
  if (args->type != PVAL_LIST) {
    return pval_error(args, "type-error: expected list");
  }

  linked_node *curr = args->value.val_list->head;
  int64_t result = 0;

  while (curr != NULL) {
    pval *elem = curr->data;
    if (elem->type != PVAL_NUMBER) {
      return pval_error(elem, "type-error: non-number in -");
    }
    if (args->value.val_list->size == 1) // make neg if single elem list
    {
      return pval_number(elem->value.val_number * -1);
    }
    if (elem == args->value.val_list->head->data) {
      result = elem->value.val_number;
      curr = curr->next;
      continue;
    }

    else {
      result -= elem->value.val_number;
    }
    curr = curr->next;
  }

  return pval_number(result);
}
pval *op_mul(pval *args) {
  if (args->type != PVAL_LIST) {
    return pval_error(args, "type-error: expected list");
  }

  linked_node *curr = args->value.val_list->head;
  int64_t prod = 0;

  while (curr != NULL) {
    pval *elem = curr->data;
    if (elem->type != PVAL_NUMBER) {
      return pval_error(elem, "type-error: non-number in *");
    }
    if (elem == args->value.val_list->head->data) {
      prod = elem->value.val_number;
    } else {
      prod *= elem->value.val_number;
    }
    curr = curr->next;
  }
  return pval_number(prod);
}

pval *op_div(pval *args) {
  if (args->type != PVAL_LIST) {
    return pval_error(args, "type-error: expected list");
  }

  if (args->value.val_list->size < 2) {
    return pval_error(args, "arity-error: requires >= 2 args");
  }

  linked_node *curr = args->value.val_list->head;
  float result = 0;

  while (curr != NULL) {
    pval *elem = curr->data;
    if (elem->type != PVAL_NUMBER) {
      return pval_error(elem, "type-error: non-number in /");
    }
    if (elem == args->value.val_list->head->data) {
      result = elem->value.val_number;
      if (!curr->next) {
        return pval_error(NULL, "arity-error: requires >= 2 args");
      }
    } else if (elem->value.val_number == 0) {
      return pval_error(elem, "arithmetic-error: division by zero");
    } else {
      result = (double)result / (double)elem->value.val_number;
      result = (int)floor(result);
    }
    curr = curr->next;
  }
  return pval_number(result);
}

pval *op_eq(pval *args) {
  if (args->type != PVAL_LIST)
    return pval_error(args, "type-error: expected list");

  linked_node *head = args->value.val_list->head;
  if (!head || !head->next)
    return pval_bool(true);

  pval *first = head->data;
  pval_type eq_type = first->type;
  linked_node *curr = head->next;

  while (curr) {
    pval *elem = curr->data;
    if (elem->type != eq_type)
      return pval_bool(false);

    switch (eq_type) {
    case PVAL_NUMBER:
      if (elem->value.val_number != first->value.val_number)
        return pval_bool(false);
      break;
    case PVAL_BOOL:
      if (elem->value.val_bool != first->value.val_bool)
        return pval_bool(false);
      break;
    case PVAL_SYMBOL:
      if (strcmp(elem->value.val_symbol, first->value.val_symbol) != 0)
        return pval_bool(false);
      break;
    default:
      return pval_error(elem, "type-error: unsupported type in =");
    }

    curr = curr->next;
  }

  return pval_bool(true);
}

pval *op_quit(pval *args) {
  (void)args;
  printf("Quitting...\n");
  exit(0);
}

pval *insert_op(pval *args) { // takes a symbol and converts into a func pval
  int op_index;
  if ((op_index = symbol_check(args->value.val_symbol)) != -1) {
    op_func func = operations[op_index].fn;
    free(args->value.val_symbol);
    args->value.val_symbol = NULL;
    args->type = PVAL_FUNCTION;
    args->value.val_function = func;
    return args;
  } else {
    return NULL;
  }
}

int symbol_check(char *in) {
  int op_len = sizeof(operations) / sizeof(operations[0]);
  for (int i = 0; i < op_len; i++) {
    if (strcmp(in, operations[i].name) == 0) {
      return i;
    }
  }
  return -1;
}
