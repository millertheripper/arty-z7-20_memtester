#include <stdio.h>
#include <stdlib.h>
#include "hls_mem_tester.h"

uint32_t test_data[MASTER_DEPTH];

int mem_init_rand(uint32_t *addr, uint32_t size, uint32_t seed)
{
	uint32_t *ptr = addr;
	srand(seed);

	printf("Initialize memory with random values ");

	for (uint32_t i=0; i<size; i++) {
		*ptr++ = (rand() % size);
	}

	printf(" Done.\r\n");

	return 0;
}

int main(void)
{
	/* Read random test */
	mem_init_rand(test_data, sizeof(test_data)/sizeof(uint32_t), 0x13371337);
	hls_mem_tester(test_data, sizeof(test_data)/sizeof(uint32_t));
	return 0;
}





