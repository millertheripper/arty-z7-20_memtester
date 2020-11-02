############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project hls_mem_tester
set_top hls_mem_tester
add_files hls_mem_tester.c
add_files hls_mem_tester.h
add_files -tb hls_mem_tester_tb.c -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vitis
set_part {xc7z020-clg400-1}
create_clock -period 8 -name default
set_clock_uncertainty 12.5%
#source "./hls_mem_tester/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
exit
