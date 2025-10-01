#include <stdio.h>
#include <stdlib.h>
#include <linkedlist.h>

LinkedList* list_create() {
    LinkedList *list = malloc(sizeof(LinkedList));
    if (!list) return NULL;
    list->head = NULL;
    list->size = 0;
    return list;
}

void list_destroy(LinkedList *list, void (*free_func)(void*)) {
    if (!list) return;
    Node *cur = list->head;
    while (cur) {
        Node *next = cur->next;
        if (free_func) free_func(cur->data);
        free(cur);
        cur = next;
    }
    free(list);
}

int list_push_front(LinkedList *list, void *data) {
    Node *node = malloc(sizeof(Node));
    if (!node) return -1;
    node->data = data;
    node->next = list->head;
    list->head = node;
    list->size++;
    return 0;
}

int list_push_back(LinkedList *list, void *data) {
    Node *node = malloc(sizeof(Node));
    if (!node) return -1;
    node->data = data;
    node->next = NULL;

    if (!list->head) {
        list->head = node;
    } else {
        Node *cur = list->head;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }
    list->size++;
    return 0;
}

void* list_pop_front(LinkedList *list) {
    if (!list || !list->head) return NULL;
    Node *node = list->head;
    void *data = node->data;
    list->head = node->next;
    free(node);
    list->size--;
    return data;
}

void* list_pop_back(LinkedList *list) {
    if (!list || !list->head) return NULL;
    Node *cur = list->head;
    Node *prev = NULL;

    while (cur->next) {
        prev = cur;
        cur = cur->next;
    }

    void *data = cur->data;
    if (prev) prev->next = NULL;
    else list->head = NULL;

    free(cur);
    list->size--;
    return data;
}

void* list_get(LinkedList *list, size_t index) {
    if (!list || index >= list->size) return NULL;
    Node *cur = list->head;
    for (size_t i = 0; i < index; i++) cur = cur->next;
    return cur->data;
}

int list_insert(LinkedList *list, size_t index, void *data) {
    if (!list || index > list->size) return -1;

    if (index == 0) return list_push_front(list, data);

    Node *node = malloc(sizeof(Node));
    if (!node) return -1;
    node->data = data;

    Node *cur = list->head;
    for (size_t i = 0; i < index - 1; i++) cur = cur->next;

    node->next = cur->next;
    cur->next = node;

    list->size++;
    return 0;
}

void* list_remove(LinkedList *list, size_t index) {
    if (!list || index >= list->size) return NULL;

    if (index == 0) return list_pop_front(list);

    Node *cur = list->head;
    for (size_t i = 0; i < index - 1; i++) cur = cur->next;

    Node *target = cur->next;
    void *data = target->data;

    cur->next = target->next;
    free(target);
    list->size--;
    return data;
}

size_t list_size(LinkedList *list) {
    return list ? list->size : 0;
}
