set bd_src ../source/scripts/bd.tcl
if [file exists $bd_src] {
	source $bd_src
	set work_dir [get_property DIRECTORY [current_project]]
	make_wrapper -files [get_files ${work_dir}/${project_name}.srcs/sources_1/bd/design_1/design_1.bd] -top
	add_files -norecurse ${work_dir}/${project_name}.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v
	update_compile_order -fileset sources_1
	## Uncomment this to disable out-of-context synthesis
	# set_property synth_checkpoint_mode None [get_files ${work_dir}/${project_name}.srcs/sources_1/bd/design_1/design_1.bd]
}
