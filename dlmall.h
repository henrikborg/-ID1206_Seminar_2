#include <stddef.h>

int blocks_taken;
int length_of_freelist;

int *dalloc(size_t request) ;
void dfree(void *memory);
struct freelist* sanity(void);

struct freelist {
  int sanity_freelist;
  int sanity_arena;
  int no_of_blocks_in_freelist;
  int mean_size_of_blocks_in_freelist;
};
