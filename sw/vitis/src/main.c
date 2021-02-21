/* INCLUDE */
#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xscutimer.h"
#include "xtime_l.h"
#include "xil_cache_l.h"
#include "xuartps.h"
#include "xaxicdma.h"
#include "xhls_mem_perf_tester.h"
#include "xgpio.h"

/* PRIVATE MACRO */
#define SIZE_8_MB		0x00800000
#define SIZE_64_MB		SIZE_8_MB*8
#define SIZE_128_MB		SIZE_8_MB*16
#define SIZE_256_MB		SIZE_8_MB*32

//#define MEM_MASE_0_MB	0x00000000
#define MEM_BASE_64_MB	0x04000000
#define MEM_BASE_128_MB	MEM_BASE_64_MB*2
#define MEM_BASE_192_MB	MEM_BASE_64_MB*3
#define MEM_BASE_256_MB MEM_BASE_64_MB*4
#define MEM_BASE_320_MB MEM_BASE_64_MB*5
#define MEM_BASE_384_MB	MEM_BASE_64_MB*6
#define MEM_BASE_448_MB	MEM_BASE_64_MB*7
//#define MEM_BASE_512_MB	MEM_BASE_64_MB*8

//#define TIMER_TEST
#define TIMER_PRESCALER 4
#define TIMER_CLOCK (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/(2*(TIMER_PRESCALER+1)))

/* PRIVATE FUNCTION PROTOTYPES */
static uint32_t timer_init(void);
static void timer_measure_start(void);
static uint32_t timer_measure_stop(void);
static void timer_print_speed(uint32_t tick_diff, uint32_t num_bytes);
static uint32_t printb(char *txt, void *vpb, unsigned long lb);

static void demo_print_menu(void);
static void demo_run(void);
static void cache_toggle_disable(uint32_t cache_type);
static void test_size_toggle(void);
static void mem_initialize(void);
static void test_mem_cpu(void);
uint64_t test_mem_cpu_random(void);
static void test_mem_dma(void);
uint64_t test_mem_hls_random(void);

/* PRIVATE DATA */
XScuTimer timer_inst;
XScuTimer_Config *timer_inst_config;

/* Cache controller config */
uint32_t l1_icache_disable = 0;
uint32_t l1_dcache_disable = 0;
uint32_t l2_cache_disable = 0;
uint32_t memory_is_initialized = 0;
uint32_t test_size = SIZE_8_MB;		    /* How many bytes should be used for test */
uint32_t start_addr = MEM_BASE_128_MB;	/* Start address must not be 0x0 as we execute code from 0x0 */
uint32_t shadow_addr = MEM_BASE_256_MB;	/* This is the address we will copy test data to */

/* MAIN CODE */
int main(void)
{
	init_platform();

	xil_printf("\x1B[H"); //Set cursor to top left of terminal
	xil_printf("\x1B[2J"); //Clear terminal

	timer_init();
	demo_run();

    cleanup_platform();
    return 0;
}

static void demo_print_menu(void)
{
	xil_printf("\n\r");
	xil_printf("\n\r");
	xil_printf("**************************************************\n\r");
	xil_printf("*        Arty Z7 Memory Performance Tester       *\n\r");
	xil_printf("**************************************************\n\r");
	xil_printf("* Level 1 I Cache: %s\n\r", l1_icache_disable ? "DISABLED" : "ENABLED");
	xil_printf("* Level 1 D Cache: %s\n\r", l1_dcache_disable ? "DISABLED" : "ENABLED");
	xil_printf("* Level 2 D Cache: %s\n\r", l2_cache_disable ? "DISABLED" : "ENABLED");
	xil_printf("* Test start  address: 0x%08X (%uMB)\n\r", (int)start_addr, (int)start_addr/(1024*1024));
	xil_printf("* Test shadow address: 0x%08X (%uMB)\n\r", (int)shadow_addr, (int)shadow_addr/(1024*1024));
	xil_printf("* Test data size: %uMB\n\r", (int)(test_size/(1024*1024)));
	xil_printf("* Memory initialized: %s\n\r", memory_is_initialized ? "YES" : "NO");
	xil_printf("**************************************************\n\r");
	xil_printf("\n\r");
	xil_printf("0 - Enable/Disable L1 Instruction Cache\n\r");
	xil_printf("1 - Enable/Disable L1 Data Cache\n\r");
	xil_printf("2 - Enable/Disable L2 Data Cache\n\r");
	xil_printf("3 - Change test data size\n\r");
	xil_printf("4 - Initialize memory with random data\n\r");
	xil_printf("5 - Run CPU memory performance test (memcpy)\n\r");
	xil_printf("6 - Run CPU memory performance test (random)\n\r");
	xil_printf("7 - Run FPGA (DMA) memory performance test (memcpy)\n\r");
	xil_printf("8 - Run FPGA (HLS) memory performance test (random)\n\r");
	xil_printf("\n\r");
	xil_printf("\n\r");
	xil_printf("Enter a selection: ");
	fflush(stdout);
}

static void test_size_toggle(void)
{
	static uint32_t select_size = 0;

	memory_is_initialized = 0;

	switch(++select_size) {
	case 0:
		test_size = SIZE_8_MB;
		break;
	case 1:
		test_size = SIZE_64_MB;
		break;
	default:
		select_size = 0;
		test_size = SIZE_8_MB;
		break;
	}
}

static void cache_toggle_disable(uint32_t cache_type)
{
	switch (cache_type) {
	case 0:
		if (l1_icache_disable == 0) {
			l1_icache_disable = 1;
			Xil_L1ICacheDisable();
		}
		else {
			l1_icache_disable = 0;
			Xil_L1ICacheEnable();
		}
		break;
	case 1:
		if (l1_dcache_disable == 0) {
			l1_dcache_disable = 1;
			Xil_L1DCacheDisable();
		}
		else {
			l1_dcache_disable = 0;
			Xil_L1DCacheEnable();
		}
		break;
	case 2:
		if (l2_cache_disable == 0) {
			l2_cache_disable = 1;
			Xil_L2CacheDisable();
		}
		else {
			l2_cache_disable = 0;
			Xil_L2CacheEnable();
		}
		break;
	default:
		break;
	}
}

static void demo_run(void)
{
	char input = 0;

	demo_print_menu();

	while (1) {
		input = XUartPs_RecvByte(XPAR_PS7_UART_0_BASEADDR);
		xil_printf("%c\r\n", input);

		switch (input) {
		case '0':
			cache_toggle_disable(0);
			break;
		case '1':
			cache_toggle_disable(1);
			break;
		case '2':
			cache_toggle_disable(2);
			break;
		case '3':
			test_size_toggle();
			break;
		case '4':
			mem_initialize();
			break;
		case '5':
			test_mem_cpu();
			break;
		case '6':
			test_mem_cpu_random();
			break;
		case '7':
			test_mem_dma();
			break;
		case '8':
			test_mem_hls_random();
			break;
		default:
			break;
		}

		demo_print_menu();
	}
}

static void mem_initialize(void)
{
	srand(XScuTimer_GetCounterValue(&timer_inst));
	xil_printf("\r\nmem_initialize: src:0x%08lx, size: %luMB\r\n", start_addr, test_size/(1024*1024));

	/* Clear memory to 0 */
	memset((void *)start_addr, 0x00000000, test_size);
	memset((void *)shadow_addr, 0x00000000, test_size);
	Xil_DCacheFlushRange((UINTPTR)start_addr, SIZE_256_MB);

	volatile uint32_t *ptr = (uint32_t *)start_addr;
	for (uint32_t i=0; i<(test_size/sizeof(uint32_t)); i++) {
		uint32_t val = rand()%(test_size/sizeof(uint32_t));
		//uint32_t val = i+1;
		*ptr++ = val;
		//xil_printf("val(%08u)=%08u\r\n", (uint)i, (uint)val);
	}

	Xil_DCacheFlushRange((UINTPTR)start_addr, test_size);
    xil_printf("  Done.\r\n");
    memory_is_initialized = 1;
}

uint64_t test_mem_hls_random(void)
{
	XHls_mem_perf_tester hls_inst;
	XHls_mem_perf_tester_Config *hls_conf;
	XGpio gpio_inst;


	/* Reset the HLS IP with GPIO pin */
	XGpio_Initialize(&gpio_inst, XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_DiscreteWrite(&gpio_inst, 1, 0x1);
	usleep(1000*100);
	XGpio_DiscreteWrite(&gpio_inst, 1, 0x0);
	usleep(1000*100);

	if (!memory_is_initialized) {
		mem_initialize();
	}

	xil_printf("\r\nhls_mem_perf_tester: src:0x%08lx, size: %luMB .", start_addr, test_size/(1024*1024));

	/* Initialize the HLS core with start address and test size */
	hls_conf = XHls_mem_perf_tester_LookupConfig(XPAR_HLS_MEM_PERF_TESTER_0_DEVICE_ID);
	XHls_mem_perf_tester_CfgInitialize(&hls_inst, hls_conf);
	XHls_mem_perf_tester_Set_addr(&hls_inst, start_addr);
	XHls_mem_perf_tester_Set_size(&hls_inst, test_size);

	timer_measure_start();
	XHls_mem_perf_tester_Start(&hls_inst);
	/* Since we not use the interrupt signal line, poll for the HLS core to be ready */
	while (!XHls_mem_perf_tester_IsDone(&hls_inst)) {
		usleep(1000*100);
		xil_printf(".");
	}

	uint64_t checksum = XHls_mem_perf_tester_Get_return(&hls_inst);
    xil_printf("\r\n  Done. Checksum: %u\r\n", (uint)checksum);
	timer_print_speed(timer_measure_stop(), test_size);
	xil_printf("\r\n");
	return checksum;
}

static void test_mem_dma(void)
{
	XAxiCdma dma_inst;
	XAxiCdma_Config *dma_conf;
	uint32_t dma_status;

	if (!memory_is_initialized) {
		mem_initialize();
	}

	dma_conf = XAxiCdma_LookupConfig(XPAR_AXI_CDMA_0_DEVICE_ID);
	XAxiCdma_CfgInitialize(&dma_inst, dma_conf, XPAR_AXI_CDMA_0_BASEADDR);

	xil_printf("\r\ntest_mem_dma (memcpy): src:0x%08lx, dst:0x%08lx, size: %luMB ... ", start_addr, shadow_addr, test_size/(1024*1024));

	/* Start the DMA transfer, abort in case of error */
	timer_measure_start();

	/* The DMA can only transfer max. 64 MB, otherwise we will get an error */
	dma_status = XAxiCdma_SimpleTransfer(&dma_inst, (uintptr_t)start_addr, (uintptr_t)shadow_addr, test_size-1, NULL, NULL);

	if (dma_status != XST_SUCCESS) {
		xil_printf("\r\n  DMA error. Aborting.\r\n");
		return;
	}

	/* Since we did not provide a callback function, poll for the DMA transfer to be ready */
	while (XAxiCdma_IsBusy(&dma_inst));
    xil_printf("  Done.\r\n");
	timer_print_speed(timer_measure_stop(), test_size-1);

	xil_printf("  Validating ... ");
	if (memcmp((void *)start_addr, (void *)shadow_addr, test_size-1) == 0) {
		xil_printf("OK.\r\n");
	}
	else {
		printb("START Address", (void *)(start_addr+test_size-128), 128);
		printb("START Address", (void *)(shadow_addr+test_size-128), 128);
		xil_printf("FAIL.\r\n");
	}
	xil_printf("\r\n");
}

static void test_mem_cpu(void)
{
	if (!memory_is_initialized) {
		mem_initialize();
	}

	xil_printf("\r\ntest_mem_cpu (memcpy): src:0x%08lx, dst:0x%08lx, size: %luMB ... ", start_addr, shadow_addr, test_size/(1024*1024));
	timer_measure_start();

	/* Copy data from start to shadow address */
	volatile uint32_t *src = (uint32_t *)start_addr;
	volatile uint32_t *dst = (uint32_t *)shadow_addr;

	for (uint32_t i=0; i<(test_size/sizeof(uint32_t)); i++) {
		dst[i] = src[i];
	}

	Xil_DCacheFlushRange((UINTPTR)start_addr, test_size);
    xil_printf("  Done.\r\n");
	timer_print_speed(timer_measure_stop(), test_size);
	xil_printf("  Validating ... ");

	if (memcmp((void *)start_addr, (void *)shadow_addr, test_size) == 0) {
		xil_printf("OK.\r\n");
	}
	else {
		printb("START Address", (void *)(start_addr+test_size-128), 128);
		printb("START Address", (void *)(shadow_addr+test_size-128), 128);
		xil_printf("FAIL.\r\n");
	}
	xil_printf("\r\n");
}

uint64_t test_mem_cpu_random(void)
{
	if (!memory_is_initialized) {
		mem_initialize();
	}

	xil_printf("\r\ntest_mem_cpu (random): src:0x%08lx, dst:0x%08lx, size: %luMB ...\r\n", start_addr, shadow_addr, test_size/(1024*1024));
	timer_measure_start();

	volatile uint32_t *ptr = (uint32_t *)start_addr;
	volatile uint32_t next = 0;
	uint32_t i = 0;
	uint64_t checksum = 0;
	for (i=0; i<(test_size/sizeof(uint32_t)); i++) {
		next = ptr[next];
		checksum = checksum+next;
		//xil_printf("ptr[%08u]=%08u, checksum=%08u\r\n", next, ptr[next], checksum);
	}

	Xil_DCacheFlushRange((UINTPTR)start_addr, test_size);
    xil_printf("  Done. Checksum: %u\r\n", (uint)checksum);
	timer_print_speed(timer_measure_stop(), test_size);
	xil_printf("\r\n");
	return checksum;
}

static uint32_t timer_init(void)
{
	timer_inst_config = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_DEVICE_ID);
	XScuTimer_CfgInitialize (&timer_inst, timer_inst_config, XPAR_XSCUTIMER_0_BASEADDR);
    XScuTimer_SetPrescaler(&timer_inst, TIMER_PRESCALER);
	XScuTimer_EnableAutoReload(&timer_inst);
	XScuTimer_LoadTimer(&timer_inst, 0xFFFFFFFF);
    XScuTimer_Start(&timer_inst);

#ifdef TIMER_TEST
	uint32_t i = 0;
    xil_printf("Timer Test: Prescaler: %d\r\n", XScuTimer_GetPrescaler(&timer_inst));
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

static void timer_measure_start(void)
{
	/* Collect timer value for performance measurement */
	XScuTimer_LoadTimer(&timer_inst, 0xFFFFFFFF);
    XScuTimer_RestartTimer(&timer_inst);
}

static uint32_t timer_measure_stop(void)
{
    return (0xFFFFFFFF - XScuTimer_GetCounterValue(&timer_inst));
}

static void timer_print_speed(uint32_t tick_diff, uint32_t num_bytes)
{
	float time_diff = 0;
	float mb_per_sec = 0;

	time_diff = (float)(tick_diff)/TIMER_CLOCK;
    mb_per_sec = ((float)(num_bytes)/(1024*1024))/time_diff;
    printf("  Time: %f s, Speed: %.2f MB/s\r\n", time_diff, mb_per_sec);
}

static uint32_t printb(char *txt, void *vpb, unsigned long lb)
{
	unsigned char *pb = vpb;
	unsigned long i = 0;
	static unsigned char dbg_buf[128];
	unsigned char *dbg_p = dbg_buf;
	const char *res = "0123456789abcdef";

	xil_printf("%s buf 0x%08lx len %ld\n",
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


