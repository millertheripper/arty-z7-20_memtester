include Makefile.inc

HLS_PRJ_DIR := $(shell pwd)/hw/source/ip_cores/hls_mem_tester

.PHONY: all
all: hls fpga vitis

.PHONY: hls
hls:
	$(MAKE) -C $(HLS_PRJ_DIR)

.PHONY: vitis
vitis:
	$(MAKE) -C sw/vitis

.PHONY: fpga
fpga:
	$(MAKE) -C hw

.PHONY: clean
clean:
	$(MAKE) -C $(HLS_PRJ_DIR) clean
	$(MAKE) -C hw clean
	$(MAKE) -C sw/vitis clean
	@rm -rf *.str *.log *.jou .Xil