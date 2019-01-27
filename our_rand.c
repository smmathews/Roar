#include "our_rand.h"

#define _XOPEN_SOURCE
#include <stdlib.h>
#include <time.h>

void seed() {
	srand48(time(0));
}

size_t randInRange(size_t i) {
	return (unsigned int) (drand48()*(i+1));
}
