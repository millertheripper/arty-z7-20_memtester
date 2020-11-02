/*****************************************************************************
* Application: Memory performance test
* Author: Martin Mueller
* Date: 25.05.2020
* Description:
*   - Compare memory access performance of CPU against FPGA HLS core
*   - Write sequential, read sequential and or read random access modes
*   - Memory performance of HLS core is dependent on type of AXI connection
*     and AXI with
*   - Memory performance of CPU is influenced by caches in random read mode
******************************************************************************/
/* Includes */
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

/* Enable test master device */
#define TEST_MEM_HLS
//#define TEST_MEM_CPU

#ifdef TEST_MEM_HLS
#include "xhls_mem_tester.h"
#endif
#include "xscutimer.h"
#include "xtime_l.h"
#include "xil_cache.h"
#include "xil_cache_l.h"
#include "stdlib.h"
#include "sleep.h"

/* Private function prototypes */
uint32_t test_mem_cpu(uint32_t mode, volatile uint32_t pattern, volatile uint32_t addr, uint32_t size);
#ifdef TEST_MEM_HLS
uint32_t test_mem_hls(uint32_t mode, uint32_t pattern, uint32_t addr, uint32_t size);
#endif
uint32_t mem_init_rand(uint32_t addr, uint32_t size, uint32_t seed);
uint32_t timer_init(void);
uint32_t printb(char *txt, void *vpb, unsigned long lb);

/* Globals */
#ifdef TEST_MEM_HLS
Xhls_mem_tester        	hls_inst;
Xhls_mem_tester_Config 	*hls_inst_config;
#endif
XScuTimer			    timer_inst;
XScuTimer_Config	    *timer_inst_config;

/* User customizations */
//#define TIMER_TEST
//#define DISABLE_CPU_CACHES /* Disable CPU caches for evaluating cache influence on memory performance */
#define TRANSFER_SIZE_MB	256  /* 256 MB max. */
#define TEST_PATTERN		0x12345678

#define TRANSFER_SIZE	    ((TRANSFER_SIZE_MB*1024*1024)/sizeof(uint32_t))
#define MEM_BASE 		    0x10000000 /* Use only upper 256 MB as we are running the program on the lower 256 MB */

#define TIMER_PRESCALER 4
#define TIMER_CLOCK (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/(2*(TIMER_PRESCALER+1)))

#define TEST_MODE_WRITE_SEQUENTIAL 	0
#define TEST_MODE_READ_SEQUENTIAL 	1
#define TEST_MODE_READ_RANDOM		2


int main(void)
{
	init_platform();

	/* Disable all CPU caches for better comparing memory access speed FPGA vs. CPU */
#ifdef DISABLE_CPU_CACHES
	Xil_L1ICacheDisable();
	Xil_L2CacheDisable();
	Xil_ICacheDisable();
#endif

	timer_init();

#ifdef TEST_MEM_HLS
    hls_inst_config = Xhls_mem_tester_LookupConfig(XPAR_hls_mem_tester_0_DEVICE_ID);
    if (Xhls_mem_tester_CfgInitialize(&hls_inst, hls_inst_config) == XST_SUCCESS) {
    	xil_printf("HLS core initialized!\r\n");
    }

    else {
    	xil_printf("#### Failed to initialize hls instance #### \r\n");
    }

    xil_printf("\r\n***** HLS write sequential test *******\r\n");
    test_mem_hls(TEST_MODE_WRITE_SEQUENTIAL, TEST_PATTERN, MEM_BASE, TRANSFER_SIZE);
    printb("Buffer contents: ", (uint32_t*)MEM_BASE + TRANSFER_SIZE - 128 , 128);

    xil_printf("\r\n***** HLS read sequential test *******\r\n");
    test_mem_hls(TEST_MODE_READ_SEQUENTIAL, TEST_PATTERN, MEM_BASE, TRANSFER_SIZE);

    xil_printf("\r\n***** HLS read random test *******\r\n");
    test_mem_hls(TEST_MODE_READ_RANDOM, TEST_PATTERN, MEM_BASE, TRANSFER_SIZE);
#endif

    #ifdef TEST_MEM_CPU
    xil_printf("\r\n***** CPU write sequential test *******\r\n");
    test_mem_cpu(TEST_MODE_WRITE_SEQUENTIAL, TEST_PATTERN, MEM_BASE, TRANSFER_SIZE);
    printb("Buffer contents: ", (uint32_t*)MEM_BASE + TRANSFER_SIZE - 128 , 128);

    xil_printf("\r\n***** CPU read sequential test *******\r\n");
    test_mem_cpu(TEST_MODE_READ_SEQUENTIAL, 0, MEM_BASE, TRANSFER_SIZE);

    xil_printf("\r\n***** CPU read random test *******\r\n");
    test_mem_cpu(TEST_MODE_READ_RANDOM, 0, MEM_BASE, TRANSFER_SIZE);


#endif

    printf("\r\nGoodbye.\n");
    cleanup_platform();
    return 0;
}

#ifdef TEST_MEM_HLS
uint32_t test_mem_hls(uint32_t mode, uint32_t pattern, uint32_t addr, uint32_t size)
{
	uint32_t timer_val_before = 0;
	uint32_t timer_val_after = 0;
	float time_ticks = 0;
	float mb_per_sec = 0;

	/* Initialize memory with random address values */
	if (mode == TEST_MODE_READ_RANDOM) {
	    mem_init_rand(addr, size, 0x1234567);
	    printb("Buffer contents: ", (uint32_t*)addr + TRANSFER_SIZE - 128 , 128);
	}

    /* Load function arguments to hls core */
    Xhls_mem_tester_Set_addr(&hls_inst, addr);
    Xhls_mem_tester_Set_size(&hls_inst, size);
    Xhls_mem_tester_Set_pattern(&hls_inst, pattern);
    Xhls_mem_tester_Set_mode(&hls_inst, mode);

    while (!Xhls_mem_tester_IsIdle(&hls_inst)) {};
    while (!Xhls_mem_tester_IsReady(&hls_inst)) {};

	printf("Start HLS transfer: Addr: 0x%08lx-0x%08lx, Size: %lu KB : ", addr, addr+size*sizeof(size), (size*sizeof(size))/1024);

	/* Collect timer value for performance measurement */
	XScuTimer_LoadTimer(&timer_inst, 0xFFFFFFFF);
    XScuTimer_RestartTimer(&timer_inst);
    timer_val_before = XScuTimer_GetCounterValue(&timer_inst);

    /* Start hls core operation and poll-wait until its finished */
    Xhls_mem_tester_Start(&hls_inst);
    while (!Xhls_mem_tester_IsDone(&hls_inst)) {};

	/* Collect timer value for performance measurement */
    timer_val_after = XScuTimer_GetCounterValue(&timer_inst);

    printf("Done.\r\n");

    time_ticks = (float)(timer_val_before-timer_val_after)/TIMER_CLOCK;
    mb_per_sec = ((float)(size*sizeof(size))/(1024*1024))/time_ticks;
    printf("Time: %f s, Speed: %.2f MB/s\r\n", time_ticks, mb_per_sec);

    return (Xhls_mem_tester_Get_return(&hls_inst));
}
#endif

uint32_t test_mem_cpu(uint32_t mode, volatile uint32_t pattern, volatile uint32_t addr, uint32_t size)
{
	uint32_t timer_val_before = 0;
	uint32_t timer_val_after = 0;
	float time_ticks = 0;
	float mb_per_sec = 0;
	volatile uint32_t *ptr = (volatile uint32_t *)addr;
	printf("Start CPU transfer: Addr: 0x%08lx-0x%08lx, Size: %lu KB : ", addr, addr+size*sizeof(size), (size*sizeof(size))/1024);

	/* Initialize memory with random address values */
	if (mode == TEST_MODE_READ_RANDOM) {
	    mem_init_rand(addr, size, 0x1234567);
	    printb("Buffer contents: ", (uint32_t*)addr + TRANSFER_SIZE - 128 , 128);
	}

	/* Collect timer value for performance measurement */
	XScuTimer_LoadTimer(&timer_inst, 0xFFFFFFFF);
    XScuTimer_RestartTimer(&timer_inst);

    timer_val_before = XScuTimer_GetCounterValue(&timer_inst);

    /* Write sequential */
    if (mode == TEST_MODE_WRITE_SEQUENTIAL) {
    	for (uint32_t i=0; i<size; i++) {
    		*ptr++ = pattern;
    	}
    }

    /* Read sequential */
    if (mode == TEST_MODE_READ_SEQUENTIAL) {
		for (uint32_t i=0; i<size; i++) {
			pattern = *ptr++;
		}
    }

    /* Read random */
    if (mode == TEST_MODE_READ_RANDOM) {
    	volatile uint32_t next = 0;

    	for (uint32_t i=0; i<size; i++) {
    		next = ptr[next];
    	}
    }

	/* Collect timer value for performance measurement */
    timer_val_after = XScuTimer_GetCounterValue(&timer_inst);

    printf("Done.\r\n");

    time_ticks = (float)(timer_val_before-timer_val_after)/TIMER_CLOCK;
    mb_per_sec = ((float)(size*sizeof(size))/(1024*1024))/time_ticks;
    printf("Time: %f s, Speed: %.2f MB/s\r\n", time_ticks, mb_per_sec);

	return 0;
}


uint32_t mem_init_rand(uint32_t addr, uint32_t size, uint32_t seed)
{
	srand(seed);

	xil_printf("\r\nInitialize memory with random values ");

	uint32_t *ptr = (uint32_t *)addr;

	for (uint32_t i=0; i<size; i++) {
		if (!(i%(1024*1024))) {
			xil_printf(".");
		}
		*ptr++ = (rand() % size);
	}

	xil_printf(" Done.\r\n");

	return 0;
}

uint32_t timer_init(void)
{
	timer_inst_config = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_DEVICE_ID);
	XScuTimer_CfgInitialize (&timer_inst, timer_inst_config, XPAR_XSCUTIMER_0_BASEADDR);
    XScuTimer_SetPrescaler(&timer_inst, TIMER_PRESCALER);
	XScuTimer_EnableAutoReload(&timer_inst);
	XScuTimer_LoadTimer(&timer_inst, 0xFFFFFFFF);
    XScuTimer_Start(&timer_inst);

#ifdef TIMER_TEST
	uint32_t i = 0;
    xil_printf("Timer Prescaler: %d\r\n", XScuTimer_GetPrescaler(&timer_inst));
    while (1) {
        if (XScuTimer_IsExpired(&timer_inst)) {
        	xil_printf("Expired after %u seconds!\r\n", i);
        	break;
        }
        sleep(1);
        xil_printf(".");
        i++;
    }

    xil_printf("\r\n");
#endif

	return 0;
}

uint32_t printb(char *txt, void *vpb, unsigned long lb)
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
