
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct pval pval;

typedef struct linked_node {
    struct linked_node *next;
    pval *data;
} linked_node;


typedef struct SinglyLinkedList {
    linked_node *head, **tailp;
    size_t size;
} SinglyLinkedList;

SinglyLinkedList *create_list(void);

void insert_node(SinglyLinkedList *list, linked_node *node);
void append_node(SinglyLinkedList *list, linked_node *node);
void remove_node(SinglyLinkedList *list, linked_node *node);
void free_node(linked_node **node);
void free_list(SinglyLinkedList **list);

linked_node *create_node(pval *data);

#endif