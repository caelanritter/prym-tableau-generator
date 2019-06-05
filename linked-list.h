#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

typedef enum { false, true } bool;

typedef struct node node_t;
struct node {
  void *data;
  node_t *next;
};

int node_create(node_t **, void *);
int node_jump(node_t **);
int node_leap(node_t **, size_t);
int node_prepend(node_t **, void *);
int node_append(node_t *, void *);
int node_insert(node_t *, void *);
int node_adjoin(node_t *, node_t *, bool);
int node_remove(node_t *);
int node_shred(node_t *);
int node_presnip(node_t **);
int node_erase(node_t *);
int node_set(node_t *, void *, bool);
void * node_get(node_t *);
int node_copy(node_t *, node_t **, size_t, bool);

#endif  /*  LINKED_LIST_H  */
