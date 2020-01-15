#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "rand.h"
#include "dlmall.h"
#include <sys/times.h>
#include <time.h>

#define ROUNDS 100
#define LOOPS 10
#define MAX_MEMORY_SLOTS 30000	// An array of memory blocks to remember wich ones we have taken and can free them. No lost pointers!
#define SIZE 16

clock_t small_header(int max_memory_slots) {
  const char *string_to_write = "STRING TO WRITE";
  
	char* memory_slots[MAX_MEMORY_SLOTS];
	/* clear memory_slots */
	for(int i = 0; i < max_memory_slots; i++) {
		memory_slots[i] = 0;
	}
  
  clock_t start, stop;
  start = clock();
  
  for(int j = 0; j < LOOPS; j++) {
    // Allocate 1000 blocks of each 16 bytes
    for(int i = 0; i < max_memory_slots; i++) {
      void* mem = dalloc(SIZE);
      if(NULL == mem) {
        fprintf(stderr, "Out of memory after %d requests\n", i);
        //return 1;
        continue;
      } else {
        memory_slots[i] = mem;
        //printf("alloc size %d\n", size);
      }
    }

    // Write to all of these memory blocks a couple of thosand times
    for(int i = 0; i < max_memory_slots; i++) {
      sprintf(memory_slots[i], string_to_write);
      //printf("memory_slot[%d] %s\n", i, memory_slots[i]);
    }
    //printf("Loop %d\n", j);

    /* free allocated memory */
    for(int i = 0; i < max_memory_slots; i++) {
      if(memory_slots[i])
        dfree(memory_slots[i]);
    }
  }
  
  stop = clock();

  return stop - start;
}

int main() {
  struct freelist *freelist_info;

  clock_t new, smallest = 0;

  for(int j = 100; j < MAX_MEMORY_SLOTS; j *= 2) {
    smallest = small_header(j);
    for(int i = 0; i < 1000; i++) {
      new = small_header(j);
      if(new < smallest)
        smallest = new;
    }
    double ms = (int)smallest * 1000;
    ms /= CLOCKS_PER_SEC;
    //printf("Execution time %ld (ticks) %f (ms) %ld\n", smallest, ((double_t)smallest * 1000) / CLOCKS_PER_SEC, CLOCKS_PER_SEC);
    printf("%d %f\n", j, ((double_t)smallest * 1000) / CLOCKS_PER_SEC);
  }
  //printf("start %ld  stop %ld  total %Lf\n", start.tms_utime, stop.tms_utime, ((long double)(stop.tms_utime - start.tms_utime))/CLOCKS_PER_SEC);

	/*printf("free+dalloc\n"\
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
		printf("%d\t", j*LOOPS+i);
    printf(" %d\t", freelist_info->no_of_blocks_in_freelist);
    printf(" %d\t", freelist_info->no_of_blocks_in_arena);
    printf(" %d\n", freelist_info->total_size_of_blocks_in_freelist / freelist_info->no_of_blocks_in_freelist);
		//printf("%d\t %d\n", i*j+i, blocks_taken);
	}

	printf("== DONE ==\n");*/

	//sanity(0,1,1,1,0);
	return 0;
}
