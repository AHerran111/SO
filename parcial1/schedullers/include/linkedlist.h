#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>  // for size_t

// Node definition
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

// List definition
typedef struct LinkedList {
    Node *head;
    size_t size;
} LinkedList;

// Function declarations
LinkedList* list_create();
void list_destroy(LinkedList *list, void (*free_func)(void*));

int list_push_front(LinkedList *list, void *data);
int list_push_back(LinkedList *list, void *data);

void* list_pop_front(LinkedList *list);
void* list_pop_back(LinkedList *list);

void* list_get(LinkedList *list, size_t index);
int list_insert(LinkedList *list, size_t index, void *data);
void* list_remove(LinkedList *list, size_t index);

size_t list_size(LinkedList *list);

#endif
