#include <math.h>
#include <stdlib.h>

#define MAX 4000
#define MIN 8

int request(int min, int max) {
	/* k is log(MAX/MIN */
	double k = log(((double) max) / min);

	/* r is [0..K] */
	double r = ((double)(rand() % (int)(k*10000))) / 10000;

	/* size is [o..MAX] */
	int size = (int)((double)max / exp(r));

	return size;
}

int request_size() {
	return request(1, sizeof(int) * 256);
}

int request_memory_slot(int max) {
	return request(0, max);
}
