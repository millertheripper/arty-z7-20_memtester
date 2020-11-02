#include <stdio.h>
#include <stdlib.h>
#include "hls_mem_tester.h"

uint32_t test_data[MASTER_DEPTH];

int printb(char *txt, void *vpb, unsigned long lb)
{
	unsigned char *pb = vpb;
	unsigned long i = 0;
	static unsigned char dbg_buf[128];
	unsigned char *dbg_p = dbg_buf;
	const char *res = "0123456789abcdef";

	printf("%s buf 0x%08lx len %ld\n",
			txt ? txt : "", (unsigned long)pb, lb);

	while (i < lb) {
		*dbg_p++ = ' ';
		*dbg_p++ = res[(*pb >> 4) & 0x0f];
		*dbg_p++ = res[*pb & 0x0f];
		pb++;
		i++;
		if ((i % 8) == 0) {
			*dbg_p++ = ' ';
			*dbg_p++ = ' ';
		}
		if (((i % 16) == 0) || (i == lb)) {
			*dbg_p++ = '\n';
			*dbg_p = '\0';
			printf("%s", dbg_buf);
			dbg_p = dbg_buf;
		}
	}

	return (i);
}

int mem_init_rand(int *addr, int size, unsigned int seed)
{
	int *ptr = addr;
	srand(seed);

	printf("Initialize memory with random values ");

	for (int i=0; i<size; i++) {
		if (!(i%(256))) {
			printf(".");
		}
		*ptr++ = (rand() % size);
	}

	printf(" Done.\r\n");

	return 0;
}

int main(void)
{
	/* Write test */
	hls_mem_tester(0, 0xdeadbeef, test_data, sizeof(test_data)/sizeof(uint32_t));
	printb("Test data content", test_data, sizeof(test_data)/sizeof(uint32_t));

	/* Read sequential test */
	hls_mem_tester(1, 0xdeadbeef, test_data, sizeof(test_data)/sizeof(uint32_t));

	/* Read random test */
	mem_init_rand(test_data, sizeof(test_data)/sizeof(uint32_t), 0x13371337);

	printb("Test data content", test_data, sizeof(test_data)/sizeof(uint32_t));

	hls_mem_tester(2, 0xdeadbeef, test_data, sizeof(test_data)/sizeof(uint32_t));
	return 0;
}





