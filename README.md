# Arty-Z7-20: Memory performance tester
This project demonstrates PS RAM speed, random vs. sequential access. Also it is compared how fast the PL is in comparison to the PS. PL access to PS RAM is done by a HLS IP core. Software is implemented as Vitis Bare Metal project.
- Sequential READ access from PS
- Random READ access from PS
- Sequential READ access from PL
- Random READ access from PL

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


