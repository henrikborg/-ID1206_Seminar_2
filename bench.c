#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "rand.h"
#include "dlmall.h"

#define ROUNDS 100
#define LOOPS 10
#define MAX_MEMORY_SLOTS 1000	// An array of memmory blocks to remember wich ones we have taken and can free them. No lost pointers!

int main() {
  struct freelist *freelist_info;
  
	void* memory_slots[MAX_MEMORY_SLOTS];
	/* clear memory_slots */
	for(int i = 0; i < MAX_MEMORY_SLOTS; i++) {
		memory_slots[i] = 0;
	}

	printf("free+dalloc\n"\
          "\tno of free blocks\n"\
          "\t\tno of blocks in arena\n"\
          "\t\t\tmean value of blocks in free list\n");
  //printf("flist - bench enter - %p\n", flist);
	for(int j = 0; j < ROUNDS; j++) {
		int i = 0;
		for(; i < LOOPS; i++) {
			int memory_slot = request_memory_slot(MAX_MEMORY_SLOTS);
			//printf("requested memory slot %d\n", memory_slot);
			if(memory_slots[memory_slot]) {
        //printf("Bench dfree\n");
        //printf("flist - bench before free - %p\n", flist);
				dfree(memory_slots[memory_slot]);
        //printf("flist - bench after  free - %p\n", flist);
				memory_slots[memory_slot] = 0;
			} else {
        int size = request_size();
        //printf("Bench dalloc\n");
        //printf("flist - bench before alloc - %p\n", flist);
				void* mem = dalloc((size_t)size);
        //printf("flist - bench after  alloc - %p\n", flist);
				if(NULL == mem) {
					fprintf(stderr, "Out of memory after %d requests\n", j*ROUNDS+i);
					//return 1;
          continue;
				} else {
					memory_slots[memory_slot] = mem;
          //printf("alloc size %d\n", size);
				}
			}
      //sanity(0,1,0,0,1);
      //printf(" %d\n", freelist_info->no_of_blocks_in_freelist);
		}
    //printf("flist - bench exit - %p\n", flist);
    freelist_info = sanity(0,1,0,0,0);
		printf("%d\t", j*ROUNDS+i);
    printf(" %d\t", freelist_info->no_of_blocks_in_freelist);
    printf(" %d\t", freelist_info->no_of_blocks_in_arena);
    printf(" %d\n", freelist_info->total_size_of_blocks_in_freelist / freelist_info->no_of_blocks_in_freelist);
		//printf("%d\t %d\n", i*j+i, blocks_taken);
	}

	printf("== DONE ==\n");

	sanity(0,1,1,1,0);
	return 0;
}
