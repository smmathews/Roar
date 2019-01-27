#include <stdlib.h>
#include <time.h>
#include "roaring.c"
#include "our_rand.h"

#define THOUSAND 1000
#define MILLION THOUSAND*THOUSAND
#define BILLION THOUSAND*MILLION
#define katy_perry_followers 107*MILLION
#define total_twitter 1*BILLION

roaring_bitmap_t* shuffle_in_twitter(uint32_t numToSet) {
  roaring_bitmap_t *r1 = roaring_bitmap_create();
    
  // add 0 through numToSet-1
  roaring_bitmap_add_range(r1, 0, numToSet);

  // randomly shuffle those values throughout the total billion twitter accounts
  // https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
  size_t i;
  for (i = total_twitter - 1; i > 0; i--) {
    size_t j = randInRange(i);
    bool jb = roaring_bitmap_contains(r1, j);
    bool ib = roaring_bitmap_contains(r1, i);
    if(jb && !ib) {
      roaring_bitmap_add(r1, i);
      roaring_bitmap_remove(r1, j);
    } else if(ib && !jb) {
      roaring_bitmap_add(r1, j);
      roaring_bitmap_remove(r1, i);
    }
  }

  return r1;
}

int main() {
  seed();
  printf("making katy perry\n");
  roaring_bitmap_t *katy_perry = shuffle_in_twitter(katy_perry_followers);
  roaring_bitmap_free(katy_perry);
  return 0;
}
