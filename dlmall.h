#include <stddef.h>

int length_of_freelist;

int *dalloc(size_t request) ;
void dfree(void *memory);
int sanity(void);
