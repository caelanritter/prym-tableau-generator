
#include <string.h>

#include "linked-list.h"

/* accepts a pointer to a null pointer and points it at a single node
   with the given data */
int node_create(node_t **head_p, void *data) {
  if (*head_p != NULL) { return -1; }
  *head_p = malloc(sizeof(node_t));
  if (*head_p == NULL) { return -1; }
  (*head_p)->data = data;
  (*head_p)->next = NULL;
  return 0;
}

/* jumps to the next node in the linked list, and returns -1 if the
   current node is NULL */
int node_jump(node_t **current_p) {
  if (*current_p == NULL) { return -1; }
  *current_p = (*current_p)->next;
  return 0;
}

/* jumps to the dth node relative to the current node, and returns -1
   if the leap went off the end of the linked list */
int node_leap(node_t **current_p, size_t d) {
  size_t i;
  int ret_code = 0;
  for (i = 0; i < d; i++) {
    ret_code += node_jump(current_p);
  }
  if (ret_code < -1) { ret_code = -1; }
  return ret_code;
}

/* prepends a node with the given data to the linked list and jumps
   the head node back to this new node */
int node_prepend(node_t **head0_p, void *data) {
  node_t *head1 = NULL;
  if (node_create(&head1, data) == -1) { return -1; }
  head1->next = *head0_p;
  *head0_p = head1;
  return 0;
}

/* appends a node with the given data to the end of the linked list
   without changing the current node */
int node_append(node_t *current0, void *data) {
  node_t *tail = NULL, *current1 = current0;
  if (node_create(&tail, data) == -1) { return -1; }
  while (current1->next != NULL) {
    node_jump(&current1);
  }
  current1->next = tail;
  return 0;
}

int node_insert(node_t *current, void *data) {
  node_t *next0 = NULL, *next1 = current->next;
  if (node_create(&next0, data) == -1) { return -1; }
  next0->next = next1;
  current->next = next0;
  return 0;
}
int node_adjoin(node_t *tail0, node_t *head1, bool force_shred) {
  if (tail0->next == NULL) {
    tail0->next = head1;
    return 0;
  }
  else if (force_shred) {
    node_shred(tail0);
    tail0->next = head1;
    return 1;
  }
  else { return -1; }
}

/* removes the node following the given one */
int node_remove(node_t *previous) {
  if (previous == NULL) { return -1; }
  node_t *current = previous->next;
  if (current == NULL) { return -1; }
  node_t *next = current->next;
  free(current->data);
  free(current);
  previous->next = next;
  return 0;
}

/* frees all nodes and their corresponding data after the current
   node */
int node_shred(node_t *current) {
  if (current == NULL) { return -1; }
  node_t *next = current->next;
  current->next = NULL;
  current = next;
  while (current != NULL) {
    next = current->next;
    free(current->data);
    free(current);
    current = next;
  }
  return 0;
}

/* removes the head node and switches the head node pointer to the
   next node in the list */
int node_presnip(node_t **head0_p) {
  node_t *head1 = (*head0_p)->next;
  free((*head0_p)->data);
  free(*head0_p);
  *head0_p = head1;
  return 0;
}

/* erases the data corresponding to the given node */
int node_erase(node_t *current) {
  free(current->data);
  current->data = NULL;
  return 0;
}

int node_set(node_t *current, void *data, bool force_erase) {
  if (current->data != NULL) {
    if (!force_erase) { return -1; }
    free(current->data);
  }
  current->data = data;
  return 0;
}

void * node_get(node_t *current) {
  if (current == NULL) { return NULL; }
  return current->data;
}

int node_copy(node_t *source, node_t **dest_p,
	      size_t data_size, bool force_shred) {
  if (*dest_p != NULL) {
    if (!force_shred) { return -1; }
    node_shred(*dest_p);
    free((*dest_p)->data);
    free(*dest_p);
    *dest_p = NULL;
  }

  if (source == NULL) { return -1; }
  node_t *current0 = source;
  void *data1;
  if ((data1 = malloc(data_size)) == NULL) { return -1; }
  memcpy(data1, current0->data, data_size);
  if (node_create(dest_p, data1) == -1) { return -1; }
  node_t *current1 = *dest_p;
  node_jump(&current0);
  
  while (current0 != NULL) {
    if ((data1 = malloc(data_size)) == NULL) { return -1; }
    memcpy(data1, current0->data, data_size);
    if (node_append(current1, data1) == -1) { return -1; }
    node_jump(&current0);
    node_jump(&current1);
  }
  return 0;
}
