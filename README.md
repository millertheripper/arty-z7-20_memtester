# Arty-Z7-20 Memtester
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
- Vivado/Vitis 2020.1
- Terminal program (TeraTerm or minicom)

## How to build project
- make

# Revision control
## From Vivado tcl shell: 
- write_bd_tcl [get_property DIRECTORY [current_project]]/../source/scripts/bd.tcl -include_layout -force

## Vitis source files:
- ./sw/vitis/src