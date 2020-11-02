#include "hls_mem_tester.h"
#include <stdint.h>

const int m_axi_depth = MASTER_DEPTH;

int hls_mem_tester(uint32_t mode, volatile uint32_t pattern, volatile uint32_t *addr, uint32_t size)
{
#pragma HLS INTERFACE m_axi depth=m_axi_depth offset=slave port=addr bundle=MASTER
#pragma HLS INTERFACE s_axilite port=pattern bundle=SLAVE_ARG
#pragma HLS INTERFACE s_axilite port=mode bundle=SLAVE_ARG
#pragma HLS INTERFACE s_axilite port=size bundle=SLAVE_ARG
#pragma HLS INTERFACE s_axilite port=return bundle=SLAVE_CTRL

	/* Write sequential mode */
	if (mode == 0) {
		for (int i = 0; i < size; i++) {
			#pragma HLS_PIPELINE
			addr[i] = pattern;
		}
	}

	/* Read sequential mode */
	if (mode == 1) {
		for (int i = 0; i < size; i++) {
			#pragma HLS_PIPELINE
			pattern = addr[i];
		}
	}

	/* Read mode random mode. Memory contents needs to be initialized with random address values
	 * in the range of size since we  can not generate random numbers with hls at this point */
	if (mode == 2) {
		volatile uint32_t next = 0;

		for (int i = 0; i < size; i++) {
			#pragma HLS_PIPELINE
			next = addr[next];
		}
	}

	else {
		return (-1);
	}

	return (0);
}
