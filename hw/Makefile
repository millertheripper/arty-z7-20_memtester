include ../Makefile.inc
VIVADO := /opt/Xilinx/Vivado/$(VERSION)/bin
BUILD := $(shell pwd)/build
VIVADO_BIN := $(VIVADO)/vivado
VIVADO_CMD := cd $(BUILD) && $(VIVADO_BIN) -nolog -nojournal -mode batch -source

BITSTREAM := $(BUILD)/$(NAME).bit
PROJECT := $(BUILD)/$(NAME).xpr
SCRIPTS := $(shell pwd)/scripts

SRC := $(shell pwd)/source
SCRIPT_SRC := $(SRC)/scripts
CONSTRAINT_SRC := $(SRC)/constraints
HDL_SRC := $(SRC)/hdl
IP_SRC := $(SRC)/ip
MISC_SRC := $(SRC)/misc

HDL_SRCS := \
	$(wildcard $(HDL_SRC)/*.vhd) \
	$(wildcard $(HDL_SRC)/*.v)
CONSTRAINT_SRCS := $(wildcard $(CONSTRAINT_SRC)/*.xdc)
SCRIPT_SRCS := $(wildcard $(SCRIPT_SRC)/*.tcl)
IP_SRCS := $(wildcard $(IP_SRC)/*)
MISC_SRCS := $(wildcard $(MISC_SRC)/*)

### Add external HDL files here. They should be relative to the src directory
EXTERNAL_HDL :=

EXTERNAL_SRCS := $(addprefix $(shell pwd)/../src/,$(EXTERNAL_HDL))

ALL_SRCS := \
	$(CONSTRAINT_SRCS) \
	$(SCRIPT_SRCS) \
	$(HDL_SRCS) \
	$(IP_SRCS) \
	$(MISC_SRCS) \
	$(EXTERNAL_SRCS)

# You can edit the Vivado build settings in the scripts/settings.tcl file

.PHONY: all
all: $(BITSTREAM)

$(BITSTREAM): $(SCRIPTS)/implement.tcl $(PROJECT)
	@echo
	@echo "Synthesising and implementing design"
	@echo
	@$(VIVADO_CMD) $(SCRIPTS)/implement.tcl

$(PROJECT): $(SCRIPTS)/create_project.tcl \
	    $(SCRIPTS)/add_bd.tcl $(SCRIPTS)/settings.tcl \
	    $(SCRIPTS)/add_sources.tcl \
	    $(ALL_SRCS)
	@echo
	@echo "Creating project"
	@echo
	@mkdir -p $(BUILD)
	@$(VIVADO_CMD) $(SCRIPTS)/create_project.tcl -tclargs $(EXTERNAL_SRCS)

.PHONY: clean
clean:
	@echo "Deleting contents of the build directory"
	@rm -rf $(BUILD) *.str *.jou *.log .Xil
	@mkdir -p $(BUILD)