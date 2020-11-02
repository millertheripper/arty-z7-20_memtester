#include <stdint.h>

int hls_mem_tester(uint32_t mode, volatile uint32_t pattern, volatile uint32_t *addr, uint32_t size);

#define MASTER_DEPTH 1024
