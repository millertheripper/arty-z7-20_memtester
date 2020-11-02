include ./../../Makefile.inc

SHELL := /bin/bash
VIVADO := source /opt/Xilinx/Vivado/$(VERSION)/settings64.sh
XSCT := xsct
WS_DIR := $(shell pwd)/ws
SRC_DIR := $(shell pwd)/src
TMP_IMG_DIR := $(WS_DIR)/tmp
XSA_DIR := $(WS_DIR)/../../xsa

# Name of the hardware platform that gets imported from xsa file
HW_PLATFORM_NAME := design_1_wrapper

.PHONY: all
all: clean ws bootimg

.PHONY: clean
clean: 
	rm -rf ws *.str *.log .Xil build Packages

.PHONY: ws
ws:
	mkdir ws build
	cp -r $(XSA_DIR) ws
	cd ws && $(VIVADO) && $(XSCT) ./../scripts/create_ws.tcl $(HW_PLATFORM_NAME) $(SRC_DIR)

.PHONY: bootimg
bootimg:
	rm -rf ws/tmp
	cd ws && mkdir tmp
	$(VIVADO) && bash -c "sh scripts/create_bif.sh $(WS_DIR) $(HW_PLATFORM_NAME) $(TMP_IMG_DIR)"
	@cp $(TMP_IMG_DIR)/BOOT.bin $(TMP_IMG_DIR)/sw.bif ./build
	@rm -rf ws/tmp
	@echo ""
	@echo "INFO: Bootimage was copied to $(shell pwd)/build"
	
