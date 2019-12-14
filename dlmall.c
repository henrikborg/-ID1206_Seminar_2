#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "dlmall.h"

struct head {
  uint16_t bfree;     // 2 bytes, the status of block before
  uint16_t bsize;     // 2 bytes, the size of block before
  uint16_t free;      // 2 bytes, the status of block
  uint16_t size;      // 2 bytes, the size (max 2^16 i.e. 64 KiB)

  struct head *next;   // 8 bytes pointer
  struct head *prev;  // 8 bytes pinter
};

#define TRUE 1
#define FALSE 0
#define HEAD (sizeof(struct head))
#define MIN(size) (((size)>(8))?(size):(8))
#define LIMIT(size) (MIN(0) + HEAD + size)
#define MAGIC(memory) ((struct head*) memory - 1)
#define HIDE(block) (void*)((struct head*)block + 1)
#define ALIGN 8
#define ARENA (64 * 1024)

struct head *after(struct head *block) {
  return (struct head*) ((char*)block + HEAD + block->size);
}

struct head *before(struct head *block) {
  return (struct head*)((char*)block - block->bsize - HEAD);
}

struct head *split(struct head *block, int size) {
  int rsize = block->size - size - HEAD;  // the size of the next block
  block->size  = rsize;

  struct head *splt = after(block);//(struct head*)((char*)block + HEAD + rsize);
  splt->bsize = rsize;        // size of previous block
  splt->bfree = block->free;  // keep the status od preious block
  splt->size = size;          // size of this block
  splt->free = block->free;//TRUE;          // status of this block must be a free block

  struct head *aft = after(splt);//(struct head*)((char*)splt + size + HEAD); //after(splt);
  aft->bsize = size;         // update next block with the size of this block
  aft->bfree = splt->free;

  splt->next = aft;
  aft->prev = splt;
  block->next = splt;
  splt->prev = block;

  return splt;
}

struct head *arena = NULL;

/* Creating a new block */
struct head *new() {
  if(arena != NULL) {
    printf("one arena already allocated\n");
    return NULL;
  }

  // using mmap, but could have used sbrk
  struct head *new = mmap(NULL, ARENA,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS,
                          -1, 0);

  if(MAP_FAILED == new) {
    printf("nmap failed: error %d\n", errno);
    return NULL;
  }

  /* make room for head and dummy */
  int size = ARENA - 2 * HEAD;

  new->bfree = FALSE; // dummy
  new->bsize = 0;     // dummy
  new->free = TRUE;
  new->size = size;

  struct head *sentinel = after(new);

  /* only touch the status fields */
  sentinel->bfree = TRUE;//new->free;
  sentinel->bsize = size;//new->size;
  sentinel->free = FALSE;
  sentinel->size = 0;
  //sentinel->prev = new;
  //sentinel->next = NULL;

  /* this is the only arena we have */
  arena = (struct head*)new;

  return new;
}

/* The free list */
struct head *flist;

/* Detach a block from the free list */
void detach(struct head *block) {
  // In the middle of the list
  if(NULL != block->prev) {
    if(NULL != block->next) {
      block->prev->next = block->next;
      block->next->prev = block->prev;
    }
  }

  // Last in list
  if(NULL == block->next) {
    block->prev->next = NULL;
    block->prev = NULL;
  }

  // First in list
  if(NULL == block->prev) {
    flist = block->next;
    flist->prev = NULL;
    block->next = NULL;
  }

  /*if(NULL != block->next) {
    block->next->prev = block->prev;
  }

  if(NULL != block->prev) {
    block->prev->next = block->next;
  } else {
    ;//flist = block->next;//block->next->prev = NULL;
  }*/

  struct head *bfr = before(block);
  struct head *aft = after(block);
  block->free = FALSE;
  aft->bfree = block->free;
//  block->prev = NULL;
  //block->next = NULL;

  /*if(block != arena) {
    block->bfree = bfr->free;
  } else {
    block->bfree = FALSE;
  }*/
}

/****************************************************************
Determine a suitable size that is an even multiple of ALIGN and not
smaller than the minimum size
****************************************************************/
int adjust(int size) {
  int adjusted_size;

  for(adjusted_size = MIN(0); adjusted_size < size; adjusted_size += ALIGN) {
    ;
  }

  return adjusted_size;
}

/****************************************************************
Finf suitable block and give it
If necessary split the found block
****************************************************************/
struct head* find(int size) {
  if(NULL == arena) {
    flist = new();
  }

  for(struct head* block = flist; NULL != block; block = block->next) {
    struct head *aft = after(block);

    if(block->size >= LIMIT(size)) {
      struct head *splt;
      splt = split(block, size);
      detach(splt);

      return splt;
    } else {
      detach(block);

      return block;
    }
  }
}

int blocks_taken = 0;

int *dalloc(size_t request) {
  if(request < 0) {
    return NULL;
  }

  int size = adjust(request);
  struct head *taken = (struct head*)find(size);

  if(NULL == taken)
    return NULL;
  else {
    blocks_taken++;
		//printf("dalloc - blocks_taken %d\n", blocks_taken);
    printf("alloc mem %p\n", taken);
    return HIDE(taken);
  }
}

void insert(struct head *block) {
  block->next = flist;//NULL;//flist;//after(block);
  block->prev = NULL;//flist;//before(block);
  
  /*struct head *bfr = before(block);
  struct head *aft = after(block);
  aft->bfree = block->free;*/

  /*(NULL != flist) {
    flist->prev = block;
    //block->next->prev = block;
  }*/

  flist = block;
}

void dfree(void *memory) {

  if(NULL != memory) {
    struct head *block = MAGIC(memory);
    struct head *aft = after(block);
    struct head *bfr = before(block);

    block->free = TRUE;
    aft->bfree = block->free;

    /*if(block != arena) {
      block->bfree = bfr->free;
    }*/

    /*if(NULL != block->next) {
      block->next->prev = block;
    }*/
    //block->prev = aft->prev;
    //aft->prev = block;
    /*if(NULL != block->prev) {
      block->prev->next = block;
    }*/

    /* update freelist */
    insert(block);
    blocks_taken--;
		//printf("dfree  - blocks_taken %d\n", blocks_taken);
    printf("free mem %p\n", memory);
    //block->prev = NULL;
    //flist->free = TRUE;
  }

  return;
}

struct freelist* sanity(int print_ok, int print_error, int print_result_ok, int print_result_error) {
  struct freelist *freelist_info = (struct freelist*)malloc(sizeof(struct freelist));
  
  freelist_info->sanity_freelist  = TRUE;
  freelist_info->sanity_arena     = TRUE;
  freelist_info->no_of_blocks_in_freelist         = 0;
  freelist_info->no_of_blocks_in_arena            = 0;
  freelist_info->total_size_of_blocks_in_freelist  = 0;
  struct head *next_block;
  int i = 0;

  /*** check the freelist ***/
  if(print_error || print_ok)
    printf("Checking freelist - flist = %p\n", flist);

  struct head *block = flist;
  intptr_t start = (intptr_t)block;
  //while(1) {
  for(;i<blocks_taken && block != NULL;){
    //struct head *block = flist;

    /* check that the block is free (status is TRUE) */
    if(TRUE != block->free) {
      if(print_error) printf("  Block is not marked as free, is %d, %d, %p\n", block->free, i, block);
      freelist_info->sanity_freelist = FALSE;
    } else {
      if(print_ok) printf("  OK block marked as free %d %p\n", i, block);
    }
    /* check that next block's previous pointer is pointing to this one */
    if(NULL != block->next) {
      //next_block = block->next;
      if(block->next->prev != block) {
        if(print_error) printf("  Bad previous pointer %d - block %p block->next->prev %p\n",\
               i, block, block->next->prev);
        freelist_info->sanity_freelist = FALSE;
      } else {
        if(print_ok) printf("  OK previous pointer %d - block %p block->next->prev %p\n",\
                i, block, block->next->prev);
      }
    }
    /* check the size of this block */
    if(adjust(block->size) != block->size) {
      if(print_error) printf("  Bad block size %d\n", i);
      freelist_info->sanity_freelist = FALSE;
    } else {
      if(print_ok) printf("  OK block size - block %d\n", i);
    }
    freelist_info->total_size_of_blocks_in_freelist += block->size;

    i++;
    freelist_info->no_of_blocks_in_freelist = i;
    
    block = block->next;
    //printf("start %ld\t block %p\t flist %p\t size %d\t freelist_info->no_of_blocks_in_freelist %d\n", start, block, flist, block->size, freelist_info->no_of_blocks_in_freelist);
    if((NULL == block) || /*(block == next_block) ||*/ (block == flist) || (0 == block->size) || (start == (intptr_t)block)) {
      break;
    }
  }

  /*** check the ARENA-list ***/
  if(print_error || print_ok)
    printf("Checking ARENA-list\n");
  i = 0;
  for(struct head *block = arena; NULL != block->next; block = after(block), i++) {
    next_block = after(block);

    /* if block is taken, check it knows the status and size of previous block */
    if(FALSE == block->free) {
      ;
    }
    /* check that the next block know the size of this block */
    if(next_block->bsize != block->size) {
      if(print_error) printf("  Bad bsize of block %d\n", i);
      freelist_info->sanity_arena = FALSE;
    }
    if(next_block->bfree != block->free) {
      if(print_error) printf("  Bad bfree of block %d\n", i);
      freelist_info->sanity_arena = FALSE;
    }
    /* check the size of this block */
    if(adjust(block->size) != block->size) {
      if(print_error) printf("  Bad block size {adjust(ALIGN)] %d\n", i);
      freelist_info->sanity_arena = FALSE;
    }

    freelist_info->no_of_blocks_in_arena = i;
  }

	if(TRUE == freelist_info->sanity_arena && TRUE == freelist_info->sanity_freelist) {
	  if(print_result_ok) printf("Sanity checked - OK\n");
  }
	else{
	  if(print_result_error) printf("Sanity checked - ERROR\n");
  }

  return freelist_info;
}

/****************** TODO ***************************************************************
DONE - Only split - Never merge

Check that we can reach end of freelist

Make sure that all blocks in the ARENA list knows the size and status of previous block

The flist contains only links between blocks
***************************************************************************************/

//int length_of_freelist() {
  //int no_of_blocks_in_freelist = 0;
  //struct head *next_block;
  //struct head *block = flist;

  ///*** check the freelist ***/
  //printf("Checking freelist - flist = %p\n", flist);

  //if(NULL != flist) {
    //while(1) {
      //no_of_blocks_in_freelist++;
      
      //block = next_block;
      //if((NULL == block) || (block == next_block) || (block == flist) || (0 == block->size)) {
        //break;
      //}
    //}
  //}
//}

//int mean_size_of_free_blocks() {
  //int mean_size_of_blocks_in_freelist = 0;
  //struct head *next_block;
  //struct head *block = flist;
  
  //if(NULL != flist) {
    //while(1) {
      //mean_size_of_blocks_in_freelist;
  
    ///* check the size of this block */
    //if(adjust(block->size) != block->size) {
      //printf("  Bad block size %d\n", i);
      //return_value = FALSE;
    //} else {
      //printf("  OK block size - block %d\n", i);
    //}
//}
