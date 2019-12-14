#include "dlmall.h"
#include <stdio.h>

#define test_size_1 16
#define test_size_2 2
#define test_size_3 16

int main(int argc, int argv) {
  int *test1 = (int*)dalloc(sizeof(int) * test_size_1);
  if(NULL != test1) {
    printf("SUCCESS on allocating test1 = %p\n", test1);
  } else {
    printf("FAILED on allocating test1\n");
  }

  int *test2 = (int*)dalloc(test_size_2); /* test allocating less then min */
  if(NULL != test2) {
    printf("SUCCESS on allocating test2 = %p\n", test2);
  } else {
    printf("FAILED on allocating test2\n");
  }

  if(test1 != test2) {
    printf("SUCCESS test1 != test2\n");
  } else {
    printf("FAILED test1 == test2\n");
  }

  int size = test2 - test1;
  if(size < 0) {
    size = -size;
  }
  if((sizeof(int) * 2) == size) {
    printf("SUCCESS size of test1 is %d\n", size);
  } else {
    printf("FAILURE size of test1 is %d\n", size);
  }

  printf("Free test1\n");
  dfree(test1);

  int *test3 = (int*)dalloc(sizeof(int) * test_size_3); // less then what we just did free on */
  if(NULL != test3) {
    printf("SUCCESS on allocating test3 = %p\n", test3);
  } else {
    printf("FAILED on allocating test3\n");
  }

  /* do we reuse previous freed memory for test1 */
  if(test3 == test1) {
    printf("SUCCESS test3 == test1\n");
  } else {
    printf("FAILED test3 != test1\n");
  }

  if(test2 != test3) {
    printf("SUCCESS test2 != test3\n");
  } else {
    printf("FAILED test2 == test3\n");
  }

  printf("Free test2\n");
  dfree(test2);
  printf("Free test3\n");
  dfree(test3);

  if(sanity()) {
    printf("SANITY OK\n");
  } else {
    printf("BAD SANITY\n");
  }
  printf("length_of_freelist %d\n", length_of_freelist);
  return 0;
}
