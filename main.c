
/* todo: add error checking to everything I've neglected; add support
   for non-generic case; add the option of setting the swap size to
   greater than 1 */
   
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#ifndef max
#define max(a, b) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
#define min(a, b) ( ((a) < (b)) ? (a) : (b) )
#endif

uint16_t gcd_aux(uint16_t a, uint16_t b) {
  if (b != 0) { return gcd_aux(b, a % b); }
  else { return a; }
}
uint16_t gcd(uint16_t a, uint16_t b) {
  if (a >= b) { return gcd_aux(a, b); }
  else { return gcd_aux(b, a); }
}

#define r_DEFAULT 2u;
#define k_DEFAULT 3u;

/* r is the rank; k is the torsion */
uint8_t r;
uint8_t k;
/* max is the largest symbol in the tableau given that it is a minimal
   tableau with no symbols skipped */
uint8_t max;

uint8_t **tab;
uint8_t *diag;

uint64_t tableau_count;
uint64_t valence_sum;

int build_tableau();
int compute_valence();

/* debug */
int print_tableau() {
  uint8_t x, y;
  for (y = r - 1; 1; y--) {
    for (x = 0; x < r - y; x++) {
      printf("%u ", tab[x][y]);
    }
    printf("\n");
    if (y == 0) { break; }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  /* r is rank; k is torsion */
  if (argc == 3) {
    r = (uint8_t) strtol(argv[1], NULL, 10);
    k = (uint8_t) strtol(argv[2], NULL, 10);
    if (k < 2) {
      k = 2 * r - 1;
    }
  }
  else {
    r = r_DEFAULT;
    k = k_DEFAULT;
  }

  /* calculate max */
  if (k > 2 * r - 2) {
    max = r * (r + 1) / 2;
  }
  else if (k % 2 == 0) {
    max = (2 * (2 * r + 1) * k - k * k) / 8;
  }
  else {
    max = (4 * r * (k + 1) - k * k + 1) / 8;
  }
  
  /* calculate tableau_count_total; this requires special attention
     because we cannot simply evaluate the factorial in the numerator
     first, or else it would overflow */
  uint64_t tableau_count_total = 0;
  
  /* set up the tableau */
  tab = malloc(r * sizeof(uint8_t *));
  int i;
  for (i = 0; i < r; i++) {
    tab[i] = calloc(r - i, sizeof(uint8_t));
  }
  tab[0][0] = 1;
  tab[1][0] = 2;

  /* diag[0] stores the highest symbol that appears in the tableau,
     while diag[i] stores the diagonal mod k of the ith symbol */
  diag = calloc(max + 1, sizeof(uint8_t));
  diag[0] = 2;
  diag[1] = 0;
  diag[2] = 1;

  tableau_count = 0;
  valence_sum = 0;

  /* run the recursive to generate each possible tableau */
  clock_t runtime = clock();
  build_tableau();
  runtime = clock() - runtime;

  /* print results and exit */
  printf("----- parameters -----\n");
  printf("r = %u\nk = %u\n", r, k);
  printf("------ results -------\n");
  printf("max = %u\ntableau_count = %lu/%lu\nvalence_sum = %lu\n",
	 max, tableau_count, tableau_count_total, valence_sum);
  printf("runtime = %f\n", (double)runtime / (double)CLOCKS_PER_SEC);
  printf("----------------------\n");
  return EXIT_SUCCESS;
}

/* ------------ function definitions ------------ */

int build_tableau() {
  uint8_t x, y;
  /* CAUTION: this conditional is valid only in the generic case */
  if (diag[0] == max) {
    tableau_count++;
    compute_valence();
    return 1;
  }
  for (x = 0; x < r; x++) {
    for (y = 0; y < r - x; y++) {
      if (tab[x][y] == 0) {
	if (x == 0 || y == 0 || (tab[x-1][y] != 0 && tab[x][y-1] != 0)) {
	  diag[0]++;
	  tab[x][y] = diag[0];
	  build_tableau();
	  tab[x][y] = 0;
	  diag[0]--;
	}
	if (y != 0) {
	  break;
	}
	else {
	  return 0;
	}
      }
    }
  }
  return -1;
}

int compute_valence() {
  uint8_t x, y, i;
  for (x = 0; x < r; x++) {
    for (y = 0; y < r - x; y++) {
      for (i = 0; i <= max; i++) {
	if ((x == 0 || i >= tab[x-1][y]) &&
	    (y == 0 || i >= tab[x][y-1]) &&
	    (x + y == r - 1 || (i < tab[x+1][y] && i < tab[x][y+1]))) {
	  valence_sum++;
	}
      }
    }
  }
  return 0;
}
