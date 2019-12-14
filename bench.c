#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "rand.h"
#include "dlmall.h"

#define ROUNDS 1
#define LOOPS 3	
#define MAX_MEMORY_SLOTS 100

int main() {
	printf("free+dalloc\tlength of freelist\n");
	void* memory_slots[MAX_MEMORY_SLOTS];
	/* clear memory_slots */
	for(int i = 0; i < MAX_MEMORY_SLOTS; i++) {
		memory_slots[i] = 0;
	}

	for(int j = 0; j < ROUNDS; j++) {
		int i = 0;
		for(; i < LOOPS; i++) {
			int memory_slot = request_memory_slot(MAX_MEMORY_SLOTS);
			if(memory_slots[memory_slot]) {
				dfree(memory_slots[memory_slot]);
				memory_slots[memory_slot] = 0;
			} else {
				void* mem = dalloc((size_t)request_size());
				if(NULL == mem) {
					fprintf(stderr, "Out of memory after %d requests\n", i*j+i);
					return 1;
				} else {
					memory_slots[memory_slot] = mem;
				}
			}
//sanity();
		}
		printf("%d\t %d\n", i*j+i, length_of_freelist);
	}

	printf("end\n");

	sanity();
	return 0;
}
