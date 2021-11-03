# Arty-Z7-20: Memory performance tester
This project demonstrates PS RAM speed, random vs. sequential vs. DMA access. Software is implemented as Vitis Bare Metal project.
- memcpy to DRAM from PS
- random read access to DRAM from PS
- DMA transfer to DRAM from AXI DMA
- random read access to DRAM from HLS IP

This program also shows the influence of CPU L0 and L1 caches to the DRAM access speed. It also can be seen that the AXI PL to PS interface adds some amount of delay to each memory access as it is a bit slower than the CPU when doing random memory acesses.

```bash
**************************************************
*        Arty Z7 Memory Performance Tester       *
**************************************************
* Level 1 I Cache: DISABLED
* Level 1 D Cache: DISABLED
* Level 2 D Cache: DISABLED
* Test start  address: 0x08000000 (128MB)
* Test shadow address: 0x10000000 (256MB)
* Test data size: 8MB
* Memory initialized: NO
**************************************************

0 - Enable/Disable L1 Instruction Cache
1 - Enable/Disable L1 Data Cache
2 - Enable/Disable L2 Data Cache
3 - Change test data size
4 - Initialize memory with random data
5 - Run CPU memory performance test (memcpy)
6 - Run CPU memory performance test (random)
7 - Run FPGA (DMA) memory performance test (memcpy)
8 - Run FPGA (HLS) memory performance test (random)


Enter a selection: 5

test_mem_cpu (memcpy): src:0x08000000, dst:0x10000000, size: 8MB ...   Done.
  Time: 0.632053 s, Speed: 12.66 MB/s
  Validating ... OK.

Enter a selection: 6

test_mem_cpu (random): src:0x08000000, dst:0x10000000, size: 8MB ...
  Done. Checksum: 1128169227
  Time: 0.665186 s, Speed: 12.03 MB/s

Enter a selection: 7

test_mem_dma (memcpy): src:0x08000000, dst:0x10000000, size: 8MB ...   Done.
  Time: 0.009416 s, Speed: 849.58 MB/s
  Validating ... OK.

Enter a selection: 8

hls_mem_perf_tester: src:0x08000000, size: 8MB .........
  Done. Checksum: 1128169227
  Time: 0.800014 s, Speed: 10.00 MB/s
```

## Hardware Requirements:
- Digilent Arty-Z7-20
- MicroUSB Cable
- SD Card

## Software Requirements
- Linux host machine for make
- Vivado/Vitis 2020.2
- Terminal program (TeraTerm or minicom)

## Compile project:
------
Compile everything: 
```bash
make
```

Create and compile Vivado project: 
```bash
make fpga
```

Create and compile Vitis bare metal project, create boot binary
```bash
make vitis
```

## Version control:
------
Applying changes:
------
Vivado Block Design: From tcl shell:
```bash
write_bd_tcl [get_property DIRECTORY [current_project]]/../source/scripts/bd.tcl -include_layout -force
```

Vitis source files:
------
- ./sw/vitis/src


BSP directory structure: 
------
```bash
├── build                       <--- Build Outputs (FPGA and Boot binaries)
├── hw                          <--- All source files related to Vivado Design 
│   ├── build                   <--- Vivado Project  
│   ├── scripts                 <--- TCL scripts to recreate project in batch mode
│   │   └── settings.tcl        <--- Set FPGA type, project name, and number of processors for compilation 
│   └── source
│       ├── constraints         <--- Constraints location. Files will be imported during creation
│       ├── hdl                 <--- Put HDL IP blocks (non block design) here
│       ├── ip                  <--- Put IP blocks (used by ip integrator) here
│       └── scripts
│           └── bd.tcl          <--- TCL script to recreate the block design.
└── sw
    ├── vitis                   <--- Project folder containing bare metal application 
    │   ├── build               <--- Boot image is located here (BOOT.bin)
    │   ├── scripts             <--- TCL scripts for batch mode
    │   ├── src                 <--- Bare metal source code files
    │   └── ws                  <--- Vitis workspace
    └── xsa                     <--- Hardware description file, exported by vivado
```


