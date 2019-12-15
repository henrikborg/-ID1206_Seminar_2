#include <stddef.h>

int blocks_taken;
int length_of_freelist;

int *dalloc(size_t request) ;
void dfree(void *memory);
extern struct head* flist;
struct freelist* sanity(int print_ok, int print_error, int print_result_ok, int print_result_error);

struct freelist {
  int sanity_freelist;
  int sanity_arena;
  int no_of_blocks_in_freelist;
  int no_of_blocks_in_arena;
  int total_size_of_blocks_in_freelist;
};
