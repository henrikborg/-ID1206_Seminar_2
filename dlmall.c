#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <sys/mman.h>

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
  splt->free = TRUE;          // status of this block must be a free block

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
  sentinel->bfree = new->free;
  sentinel->bsize = new->size;
  sentinel->free = FALSE;
  sentinel->size = 0;
  sentinel->prev = new;
  sentinel->next = NULL;

  /* this is the only anera we have */
  arena = (struct head*)new;

  return new;
}

struct head *flist;

void detach(struct head *block) {
  if(NULL != block->next) {
    block->next->prev = block->prev;
  }

  if(NULL != block->prev) {
    block->prev->next = block->next;
  } else {
    block->next->prev = NULL;
  }

  struct head *bfr = before(block);
  struct head *aft = after(block);
  block->free = FALSE;
  aft->bfree = block->free;
//  block->prev = NULL;

  if(block != arena) {
    block->bfree = bfr->free;
  } else {
    block->bfree = FALSE;
  }
}

void insert(struct head *block) {
  block->next = flist;//after(block);
  block->prev = flist;//before(block);

  if(NULL != flist) {
    block->next->prev = block;
  }

  flist = block;
  //flist->free = TRUE;
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

int length_of_freelist = 0;

int *dalloc(size_t request) {
  if(request < 0) {
    return NULL;
  }

  int size = adjust(request);
  struct head *taken = (struct head*)find(size);

  if(NULL == taken)
    return NULL;
  else {
    length_of_freelist++;
    return HIDE(taken);
  }
}

void dfree(void *memory) {
  struct head *block = MAGIC(memory);

  if(NULL != memory) {
    struct head *aft = after(block);
    struct head *bfr = before(block);
    block->free = TRUE;
    if(block != arena) {
      block->bfree = bfr->free;
    }
    aft->bfree = block->free;
    /*block->prev = aft->prev;
    aft->prev = block;
    if(NULL != block->prev) {
      block->prev->next = block;
    }*/

    /* update freelist */
    insert(block);
    length_of_freelist--;
    //block->prev = NULL;
    //flist->free = TRUE;
  }

  return;
}

int sanity(void) {
  int return_value = TRUE;
  struct head *next_block;
  int i = 0;

  /* check the freelist */
  printf("Checking freelist - flist = %p\n", flist);

  struct head *block = flist;
  while(1) {
    //struct head *block = flist;

    /* check that the block is free (status is TRUE) */
    if(TRUE != block->free) {
      printf("  Block is not marked as free, is %d, %d, %p\n", block->free, i, block);
      return_value = FALSE;
    } else {
      printf("  OK block marked as free %d %p\n", i, block);
    }
    /* check that next block's previous pointer is pointing to this one */
    if(NULL != block->next) {
      next_block = block->next;
      if(block->next->prev != block) {
        printf("  Bad previous pointer %d - block %p block->next->prev %p\n",\
               i, block, block->next->prev);
        return_value = FALSE;
      } else {
        printf("  OK previous pointer %d - block %p block->next->prev %p\n",\
                i, block, block->next->prev);
      }
    }
    /* check the size of this block */
    if(adjust(block->size) != block->size) {
      printf("  Bad block size %d\n", i);
      return_value = FALSE;
    } else {
      printf("  OK block size %d\n", i);
    }

    block = next_block;
    if((NULL == block) || (block == next_block) || (block == flist) || (0 == block->size)) {
      break;
    }
    i++;
  }

  /* check the ARENA-list */
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
      printf("  Bad bsize of block %d\n", i);
      return_value = FALSE;
    }
    if(next_block->bfree != block->free) {
      printf("  Bad bfree of block %d\n", i);
      return_value = FALSE;
    }
    /* check the size of this block */
    if(adjust(block->size) != block->size) {
      printf("  Bad block size {adjust(ALIGN)] %d\n", i);
      return_value = FALSE;
    }
  }

  printf("Sanity checked\n");

  return return_value;
}

/****************** TODO ***************************************************************
DONE - Only split - Never merge

Check that we can reach end of freelist

Make sure that all blocks in the ARENA list knows the size and status of previous block

The flist contains only links between blocks
***************************************************************************************/
