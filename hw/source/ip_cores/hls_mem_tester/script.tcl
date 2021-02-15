############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project hls_mem_tester
set_top hls_mem_tester
add_files hls_mem_tester.cpp
add_files hls_mem_tester.hpp
add_files -tb hls_mem_tester_tb.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vivado 
config_interface  -default_slave_interface s_axilite -m_axi_addr64=0 -m_axi_alignment_byte_size 0 -m_axi_auto_max_ports=0 -m_axi_latency 0 -m_axi_max_bitwidth 1024 -m_axi_max_read_burst_length 16 -m_axi_max_widen_bitwidth 0 -m_axi_max_write_burst_length 16 -m_axi_min_bitwidth 8 -m_axi_num_read_outstanding 16 -m_axi_num_write_outstanding 16 -m_axi_offset slave -register_io off -s_axilite_data64=0
set_part {xc7z020-clg400-1}
create_clock -period 8 -name default
set_clock_uncertainty 12.5%
#source "./hls_mem_tester/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
exit
