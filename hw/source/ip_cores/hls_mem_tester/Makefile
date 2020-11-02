include ./../../../../Makefile.inc

SHELL := /bin/bash
VIVADO := source /opt/Xilinx/Vivado/$(VERSION)/settings64.sh
VITIS := vitis_hls
PROJECT_NAME := hls_mem_tester

.PHONY: all
all: clean hls

.PHONY: clean
clean: 
	@rm -rf $(PROJECT_NAME) *.str *.log *.jou .Xil 

.PHONY: hls
hls:
	$(VIVADO) && $(VITIS) -f ./script.tcl
	echo "Finished building HLS project"
	
