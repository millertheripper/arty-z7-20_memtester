#include <stdio.h>
#include <stdlib.h>
#include "hls_mem_tester.hpp"

uint32_t test_data[MASTER_DEPTH];

int mem_init_rand(uint32_t *addr, uint32_t size, uint32_t seed)
{
	uint64_t checksum = 0;
	uint32_t *ptr = addr;
	srand(seed);

	printf("Initialize memory with random values ");

	for (uint32_t i=0; i<size; i++) {
		*ptr++ = (rand()%size);
	}

	printf(" Done.\r\n");

	return 0;
}

uint64_t checksum_calc(uint32_t *addr, uint32_t size)
{
	uint32_t next = 0;
	uint64_t sum = 0;

	for (uint32_t i=0; i<size; i++) {
		next = addr[next];
		sum += next;
	}

	return sum;
}

int main(void)
{
	/* Read random test */
	mem_init_rand(test_data, sizeof(test_data)/sizeof(uint32_t), 0x13371337);
	uint64_t checksum_test = checksum_calc(test_data, sizeof(test_data)/sizeof(uint32_t));
	printf("Checksum test: %llu\r\n", checksum_test);
	uint64_t checksum_hls = hls_mem_tester(test_data, sizeof(test_data)/sizeof(uint32_t));
	printf("Checksum hls : %llu\r\n", checksum_hls);
	if (checksum_test != checksum_hls) {
		printf("Test: FAILED!\r\n");
	}
	else {
		printf("Test: PASSED!\r\n");
	}

	return 0;
}





