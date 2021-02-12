#include "hls_mem_tester.h"
#include <stdint.h>

const int m_axi_depth = MASTER_DEPTH;

int hls_mem_tester(volatile uint32_t *addr, uint32_t size)
{
#pragma HLS INTERFACE m_axi depth=m_axi_depth offset=slave port=addr bundle=MASTER
#pragma HLS INTERFACE s_axilite port=size bundle=SLAVE_ARG

#pragma HLS_PIPELINE
	/* Read mode random mode. Memory contents needs to be initialized with random address values
	 * in the range of size since we  can not generate random numbers with hls at this point */
	volatile uint32_t next = 0;

	for (uint32_t i = 0; i < size; i++) {
		next = addr[next];
	}

	return 0;
}
