#include <stdlib.h>
#include <time.h>
#include "roaring.c"
#include "our_rand.h"

#define THOUSAND 1000
#define MILLION THOUSAND*THOUSAND
#define BILLION THOUSAND*MILLION

roaring_bitmap_t* shuffle_in_a_billion(uint32_t numToSet) {
  roaring_bitmap_t *r1 = roaring_bitmap_create();
    
  // add 0 through numToSet-1
  roaring_bitmap_add_range(r1, 0, numToSet);

  // randomly shuffle those values throughout the billion fields
  // https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
  size_t i;
  for (i = BILLION - 1; i > 0; i--) {
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

void save(roaring_bitmap_t* r, char* filename) {
  roaring_bitmap_run_optimize(r);
  uint32_t expectedsize = roaring_bitmap_portable_size_in_bytes(r);
  char *serializedbytes = malloc(expectedsize);
  roaring_bitmap_portable_serialize(r, serializedbytes);
  FILE *write_ptr = fopen(filename,"wb");  // w for write, b for binary
  fwrite(serializedbytes,expectedsize,1,write_ptr); // write 10 bytes from our buffer
  fclose(write_ptr);
  free(serializedbytes);
}

roaring_bitmap_t* load(char* filename) {
  FILE *fileptr = fopen(filename,"rb");  // r for read, b for binary
  fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
  long filelen = ftell(fileptr);             // Get the current byte offset in the file
  rewind(fileptr);                      // Jump back to the beginning of the file

  char* buffer = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
  long unused = fread(buffer, filelen, 1, fileptr); // Read in the entire file
  (void)unused;
  fclose(fileptr); // Close the file

  roaring_bitmap_t *loaded = roaring_bitmap_portable_deserialize(buffer);
  free(buffer);
  return loaded;
}

const roaring_bitmap_t** load_multiple(int num, char **filenames) {
  const roaring_bitmap_t** toReturn = (const roaring_bitmap_t**)malloc(num*sizeof(roaring_bitmap_t*));
  for(int i = 0; i < num; ++i) {
    toReturn[i] = load(filenames[i]);
  }
  return toReturn;
}

int main(int argc, char **argv) {
  seed();
  if(argc > 1) {
    if(strcmp(argv[1],"create") == 0 && argc >= 3) {
      uint32_t values = atoi(argv[2]);
      printf("making bitset with %d set bits inside a total set of %d bits\n", values, BILLION);
      roaring_bitmap_t *created = shuffle_in_a_billion(values);

      char* outName = "created.roar";
      if(argc >= 4) {
        outName = argv[3];
      }
      save(created,outName);
      roaring_bitmap_free(created);
    } else if(strcmp(argv[1],"describe") == 0 && argc >= 3) {
      char* inName = argv[2];

      roaring_bitmap_t *loaded = load(inName);

      roaring_bitmap_printf_describe(loaded);
      printf("\n");

      roaring_bitmap_free(loaded);
    } else if(strcmp(argv[1],"or") == 0 && argc >= 4) {
      int num = argc - 2;
      const roaring_bitmap_t** allmybitmaps = load_multiple(num, argv + 2);
      roaring_bitmap_t *result = roaring_bitmap_or_many(num, allmybitmaps);
      save(result, "result.roar");
      roaring_bitmap_free(result);
      for(int i = 0; i < num; ++i) {
        roaring_bitmap_free((roaring_bitmap_t*)allmybitmaps[i]);
      }
      free(allmybitmaps);
    } else if(strcmp(argv[1],"and") == 0 && argc >= 4) {
      int num = argc - 2;
      const roaring_bitmap_t** allmybitmaps = load_multiple(num, argv + 2);
      roaring_bitmap_t* result = roaring_bitmap_and(allmybitmaps[0], allmybitmaps[1]);
      for(int i = 2; i < num; ++i) {
        roaring_bitmap_and_inplace(result, allmybitmaps[i]);
      }
      save(result, "result.roar");
      roaring_bitmap_free(result);
      for(int i = 0; i < num; ++i) {
        roaring_bitmap_free((roaring_bitmap_t*)allmybitmaps[i]);
      }
      free(allmybitmaps);
    }
  }
  return 0;
}
