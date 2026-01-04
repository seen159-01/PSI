#include<stdio.h>
#include<stddef.h>
#include<stdlib.h>
#include "../include/pval.h"
#include "../include/SinglyLinkedList.h"

SinglyLinkedList *create_list(void) {
    SinglyLinkedList *list = malloc(sizeof(*list));
    list->head = NULL;
    list->tailp = &list->head;
    list->size = 0;
    return list;
}

void insert_node(SinglyLinkedList *list, linked_node *node) {
    node->next = list->head;
    list->head = node;
    list->size++;
    if (list->tailp == &list->head) {
        list->tailp = &node->next;
    }
}

void append_node(SinglyLinkedList *list, linked_node *node) {
    *list->tailp = node;
    list->tailp = &node->next;
    node->next = NULL;
    list->size++;
}

void remove_node(SinglyLinkedList *list, linked_node *node) {
    linked_node *curr, **pnp = &list->head;
    while ((curr = *pnp) != NULL) {
        if (curr == node) {
            *pnp = node->next;
            if (list->tailp == &node->next) {
                list->tailp = pnp;
            }
            list->size --;
            node->next = NULL;
            break;
        }
        pnp = &curr->next;
    }
}

void free_node(linked_node **n) {
    pval_release((*n)->data);  
    free(*n);
    *n = NULL;
}

void free_list(SinglyLinkedList **list_ptr) { 
    if(list_ptr == NULL || (*list_ptr) == NULL ) return;

    SinglyLinkedList *list = *list_ptr;

    linked_node *curr = list->head;
    while (curr) {
        linked_node *next = curr->next;
        free_node(&curr);
        curr = next;
    }
    free(list);
    *list_ptr = NULL;
}

linked_node *create_node(pval* data)
{
    linked_node *node = malloc(sizeof(*node));
    pval_retain(data);
    node->next = NULL;
    node->data = data;
    return node;
}

// int main(void)
// {
//     SinglyLinkedList *list = createList();
//     LinkedNode *node1 = createNode(1);
//     LinkedNode *node2 = createNode(2);
//     LinkedNode *node3 = createNode(3);
//     LinkedNode *node4 = createNode(4);
//     printList(list);
//     insertNode(list, node1);
//     printList(list);
//     removeNode(list, node1);
//     printList(list);

//     appendNode(list, node3);
//     printList(list);

//     insertNode(list, node2);
//     insertNode(list, node1);
//     appendNode(list, node4);
//     printList(list);

//     free(node1);
//     free(node2);
//     free(node3);
//     free(node4);
//     free(list);
//     return 0;
// }