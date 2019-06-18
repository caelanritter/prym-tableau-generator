#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "linked-list.h"

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

#define g 22u //genus
#define r 6u //rank
#define k 3u //torsion

size_t global_max;

/* debug */
/* #define TEST_DEPTH 2u */
/* size_t depth = 1; */
/* size_t count = 0; */

typedef struct cell cell_t;
struct cell {
  size_t x;
  size_t y;
  size_t val;
};

/* "flocs" means "first locations"; it is the object passed to and
   from an iterative step in the decisional tree; it records the first
   (x,y)-position of each symbol under g and the max symbol that
   appears */
typedef struct flocs flocs_t;
struct flocs {
  size_t x[g];
  size_t y[g];
  size_t max;
};

flocs_t * iterate(node_t *, flocs_t *);
flocs_t * cycle_values(cell_t *, node_t *, flocs_t *, flocs_t *);
int is_subantidiagonal(node_t *);
int flocs_copy(flocs_t *, flocs_t **);
cell_t * cell_init(size_t, size_t, size_t);
int cell_copy(cell_t *, cell_t **);
void print_boundary(node_t *);
void flocs_print(flocs_t *);
flocs_t * flocs_emptyinit(void);

int main(void) {
  /* these initial assignments are made without loss of generality */
  node_t *head = NULL;
  node_create(&head, cell_init(2, 1, 2));
  node_append(head, cell_init(1, 1, 1));
  flocs_t *flocs = flocs_emptyinit();
  flocs->x[1] = 1;
  flocs->y[1] = 1;
  flocs->x[2] = 2;
  flocs->y[2] = 1;
  flocs->max = 2;
  global_max = g;
  
  clock_t t = clock();
  flocs_t *flocs_opt = iterate(head, flocs);
  t = clock() - t;
  
  printf("------ settings ------\ng: %d\tr: %d\tk: %d\n------- result -------\n", g, r, k);
  flocs_print(flocs_opt);
  printf("runtime: %f\n----------------------\n", 
	 (double)t/(double)CLOCKS_PER_SEC);
  
  node_shred(head);
  free(head->data);
  free(head);
  free(flocs);
  free(flocs_opt);
  return EXIT_SUCCESS;
}

/* -------------function definitions------------- */

/* takes as input the head node of a boundary and a corresponding
   flocs_t object and returns a flocs object which minimizes the max
   (number of symbols used) */
flocs_t * iterate(node_t *bdry_hd0, flocs_t *flocs0) {  
  node_t *bdry_cur0 = NULL, *bdry_nxt0 = NULL, *bdry_hd1 = NULL,
    *bdry_prv1 = NULL, *bdry_cur1 = NULL, *bdry_nxt1 = NULL, *bdry_xxt1 = NULL;
  cell_t *cell_cur0 = NULL, *cell_nxt0 = NULL, *c = NULL;
  flocs_t *flocs1 = NULL, *flocs_tmp = NULL, *flocs_opt = NULL;
  int dist = 0; //dist measures the distance along the boundary
  
  /* debug */
  /* depth++; */
  /* if (depth == TEST_DEPTH) { */
  /*   printf("Entering depth=%lu\n", depth); */
  /* } */
  /* printf("CALLED\n"); */
  /* print_boundary(bdry_hd0); */
  /* flocs_print(flocs0); */
  
  bdry_cur0 = bdry_hd0;
  bdry_nxt0 = bdry_hd0;
  node_jump(&bdry_nxt0);
  
  cell_cur0 = (cell_t *) node_get(bdry_cur0);
  cell_nxt0 = (cell_t *) node_get(bdry_nxt0);
  
  flocs_opt = flocs_emptyinit();
  flocs_opt->max = global_max;
  flocs_copy(flocs0, &flocs1);
  flocs1->max++;
  /* debug */
  /* printf("[\n"); */
  /* flocs_print(flocs1); */
  /* printf("]\n"); */

  /* the first cell */
  cell_copy(cell_cur0, &c);
  c->x++;
  c->val++;
  if (c->x < r + 1 && c->val < flocs_opt->max) {
    node_copy(bdry_hd0, &bdry_hd1, sizeof(cell_t), true);
    node_prepend(&bdry_hd1, c);
    if (cell_nxt0->x == cell_cur0->x) {
      node_remove(bdry_hd1);
    }
    flocs_tmp = cycle_values(c, bdry_hd1, flocs0, flocs1);
    if (flocs_tmp->max < flocs_opt->max) {
      free(flocs_opt);
      flocs_opt = flocs_tmp;
      global_max = flocs_opt->max;
    }
    else {
      free(flocs_tmp);
      flocs_tmp = NULL;
    }
  }
  else {
    free(c);
    c = NULL;
  }

  /* still the first cell */
  if (cell_nxt0->x < cell_cur0->x &&
      cell_nxt0->y > cell_cur0->y) {
    cell_copy(cell_cur0, &c);
    c->y++;
    c->val = max(cell_nxt0->val, c->val) + 1;
    if (c->x + c->y < r + 2 && c->val < flocs_opt->max) {

      node_copy(bdry_hd0, &bdry_hd1, sizeof(cell_t), true);
      bdry_xxt1 = bdry_hd1;
      bdry_cur1 = bdry_xxt1;
      node_jump(&bdry_xxt1);
      bdry_nxt1 = bdry_xxt1;
      node_jump(&bdry_xxt1);
      node_insert(bdry_cur1, c);

      if (bdry_xxt1 != NULL &&
	  ((cell_t *)node_get(bdry_xxt1))->x ==
	  ((cell_t *)node_get(bdry_nxt1))->x) {
	node_remove(bdry_cur1->next);
      }

      flocs_tmp = cycle_values(c, bdry_hd1, flocs0, flocs1);
      if (flocs_tmp->max < flocs_opt->max) {
	free(flocs_opt);
	flocs_opt = flocs_tmp;
	global_max = flocs_opt->max;
      }
      else {
	free(flocs_tmp);
	flocs_tmp = NULL;
      }
    }
    else {
      free(c);
      c = NULL;
    }
  }

  node_jump(&bdry_cur0);
  node_jump(&bdry_nxt0);
  cell_cur0 = node_get(bdry_cur0);
  cell_nxt0 = node_get(bdry_nxt0);
  dist++;

  if (cell_nxt0 != NULL) {
    /* the middle cells */
    while(bdry_nxt0->next != NULL) {
      if (cell_nxt0->x < cell_cur0->x &&
	  cell_nxt0->y > cell_cur0->y) {
	cell_copy(cell_cur0, &c);
	c->y++;
	c->val = max(cell_nxt0->val, c->val) + 1;
	if (c->x + c->y < r + 2 && c->val < flocs_opt->max) {

	  node_copy(bdry_hd0, &bdry_hd1, sizeof(cell_t), true);
	  bdry_xxt1 = bdry_hd1;
	  node_leap(&bdry_xxt1, dist-1);
	  bdry_prv1 = bdry_xxt1;
	  node_jump(&bdry_xxt1);
	  bdry_cur1 = bdry_xxt1;
	  node_jump(&bdry_xxt1);
	  bdry_nxt1 = bdry_xxt1;
	  node_jump(&bdry_xxt1);
	  node_insert(bdry_cur1, c);

	  if (((cell_t *)node_get(bdry_xxt1))->x ==
	      ((cell_t *)node_get(bdry_nxt1))->x) {
	    node_remove(bdry_cur1->next);
	  }
	  if (((cell_t *)node_get(bdry_prv1))->y ==
	      ((cell_t *)node_get(bdry_cur1))->y) {
	    node_remove(bdry_prv1);
	  }

	  flocs_tmp = cycle_values(c, bdry_hd1, flocs0, flocs1);
	  if (flocs_tmp->max < flocs_opt->max) {
	    free(flocs_opt);
	    flocs_opt = flocs_tmp;
	    global_max = flocs_opt->max;
	  }
	  else {
	    free(flocs_tmp);
	    flocs_tmp = NULL;
	  }
	}
	else {
	  free(c);
	  c = NULL;
	}
      }
      node_jump(&bdry_cur0);
      node_jump(&bdry_nxt0);
      cell_cur0 = node_get(bdry_cur0);
      cell_nxt0 = node_get(bdry_nxt0);
      dist++;
    }
  
    /* the penultimate cell */
    if (cell_nxt0->x < cell_cur0->x &&
	cell_nxt0->y > cell_cur0->y) {
      cell_copy(cell_cur0, &c);
      c->y++;
      c->val = max(cell_nxt0->val, c->val) + 1;
      if (c->x + c->y < r + 2 && c->val < flocs_opt->max) {
      
	node_copy(bdry_hd0, &bdry_hd1, sizeof(cell_t), true);
	bdry_nxt1 = bdry_hd1;
	node_leap(&bdry_nxt1, dist - 1);
	bdry_prv1 = bdry_nxt1;
	node_jump(&bdry_nxt1);
	bdry_cur1 = bdry_nxt1;
	node_jump(&bdry_nxt1);
	node_insert(bdry_cur1, c);

	if (((cell_t *)node_get(bdry_prv1))->y ==
	    ((cell_t *)node_get(bdry_cur1))->y) {
	  node_remove(bdry_prv1);
	}

	flocs_tmp = cycle_values(c, bdry_hd1, flocs0, flocs1);
	if (flocs_tmp->max < flocs_opt->max) {
	  free(flocs_opt);
	  flocs_opt = flocs_tmp;
	  global_max = flocs_opt->max;
	}
	else {
	  free(flocs_tmp);
	  flocs_tmp = NULL;
	}
      }
      else {
	free(c);
	c = NULL;
      }
    }
    
    node_jump(&bdry_cur0);
    cell_cur0 = node_get(bdry_cur0);
    dist++;
  }
  
  /* the last cell */
  cell_copy(cell_cur0, &c);
  c->y++;
  c->val++;
  if (c->y < r + 1 && c->val < flocs_opt->max) {
    node_copy(bdry_hd0, &bdry_hd1, sizeof(cell_t), true);
    bdry_cur1 = bdry_hd1;
    node_leap(&bdry_cur1, dist - 1);
    bdry_prv1 = bdry_cur1;
    node_jump(&bdry_cur1);
    node_insert(bdry_cur1, c);
    
    if (((cell_t *)node_get(bdry_prv1))->y ==
	((cell_t *)node_get(bdry_cur1))->y) {
      node_remove(bdry_prv1);
    }
    
    flocs_tmp = cycle_values(c, bdry_hd1, flocs0, flocs1);
    if (flocs_tmp->max < flocs_opt->max) {
      free(flocs_opt);
      flocs_opt = flocs_tmp;
      global_max = flocs_opt->max;
    }
    else {
      free(flocs_tmp);
      flocs_tmp = NULL;
    }
  }
  else {
    free(c);
    c = NULL;
  }

  node_shred(bdry_hd1);
  if (bdry_hd1 != NULL) {
    free(bdry_hd1->data);
    free(bdry_hd1);
    bdry_hd1 = NULL;
  }
  /* this line breaks everything; why??? */
  if (flocs1 != flocs_opt) {
    free(flocs1);
    flocs1 = NULL;
  }
  
  /* debug */
  /* count++; */
  /* depth--; */
  /* if (depth == TEST_DEPTH) { */
  /*   printf("Returning to depth=%lu; nodes searched=%lu\n", depth, count); */
  /*   count = 0; */
  /* } */
  /* printf("RETURNING\n"); */
  /* flocs_print(flocs0); */
  /* flocs_print(flocs_opt); */

  /* if flocs_opt was not changed, all the above cases failed (and
     cycle_values() was never called): this occured either because the
     boundary coincides with the sub-antidiagonal, in which case we
     return flocs0, or because global_max is low enough that we can't
     add any more cells, in which case we return flocs_opt; if neither
     is true, we return flocs_opt */
  if (flocs_opt->x[1] == 0 && is_subantidiagonal(bdry_hd0)) {
    //printf("RETURNED FLOCS0\n----------------------\n");
    free(flocs_opt);
    return flocs0;
  }
  else {
    //printf("RETURNED FLOCS_OPT\n----------------------\n");
    return flocs_opt;
  }
}

/* for a given cell, it runs iterate() with the augmented boundary and
   flocs on each valid symbol */
flocs_t * cycle_values(cell_t *c, node_t *bdry_hd1,
		       flocs_t *flocs0, flocs_t *flocs1) {
  flocs_t *flocs_tmp = NULL, *flocs_opt = NULL;
  flocs_opt = flocs_emptyinit();
  flocs_opt->max = global_max;
  for ( ; c->val <= flocs0->max; c->val++) {
    if ((c->x - c->y - flocs0->x[c->val] + flocs0->y[c->val]) % k == 0) {
      flocs_tmp = iterate(bdry_hd1, flocs0);
      if (flocs_tmp->max < flocs_opt->max) {
	free(flocs_opt);
	flocs_opt = flocs_tmp;
	global_max = flocs_opt->max;
      }
      else {
	free(flocs_tmp);
	flocs_tmp = NULL;
      }
    }
  }
  if (c->val < global_max) {
    flocs1->x[c->val] = c->x;
    flocs1->y[c->val] = c->y;
    flocs_tmp = iterate(bdry_hd1, flocs1);
    if (flocs_tmp->max < flocs_opt->max) {
      free(flocs_opt);
      flocs_opt = flocs_tmp;
      global_max = flocs_opt->max;
    }
    else {
      free(flocs_tmp);
      flocs_tmp = NULL;
    }
  }
  return flocs_opt;
}

int is_subantidiagonal(node_t * current) {
  while(current != NULL) {
    cell_t *c = (cell_t *)node_get(current);
    if (c->x + c->y < r + 1) { return 0; }
    node_jump(&current);
  }
  return 1;
}

cell_t * cell_init(size_t x, size_t y, size_t val) {
  cell_t *c = malloc(sizeof(cell_t));
  c->x = x;
  c->y = y;
  c->val = val;
  return c;
}

int cell_copy(cell_t *src, cell_t **dest) {
  if ((*dest = malloc(sizeof(cell_t))) == NULL) { return -1; }
  (*dest)->x = src->x;
  (*dest)->y = src->y;
  (*dest)->val = src->val;
  return 0;
}

int flocs_copy(flocs_t *src, flocs_t **dest) {
  *dest = flocs_emptyinit();
  size_t i;
  for (i = 1; i <= src->max; i++) {
    (*dest)->x[i] = src->x[i];
    (*dest)->y[i] = src->y[i];
  }
  (*dest)->max = src->max;
  return 0;
}

void print_boundary(node_t *current) {
  cell_t *c;
  while (current != NULL) {
    c = (cell_t *) node_get(current);
    printf("x=%ld\ty=%ld\tval=%ld\n", c->x, c->y, c->val);
    node_jump(&current);
  }
  printf("----------------------\n");
}

void flocs_print(flocs_t *flocs) {
  size_t i;
  printf("max: %ld\n", flocs->max);
  for (i = 1; i <= flocs->max; i++) {
    if (i < 10) { printf("first %ld  " "at (%ld, %ld)\n", i, flocs->x[i], flocs->y[i]); }
    else        { printf("first %ld "  "at (%ld, %ld)\n", i, flocs->x[i], flocs->y[i]); }
  }
}

flocs_t * flocs_emptyinit() {
  flocs_t *flocs = malloc(sizeof(flocs_t));
  flocs->max = g;
  size_t i;
  for (i = 0; i < g; i++) {
    flocs->x[i] = 0;
    flocs->y[i] = 0;
  }
  return flocs;
}
