source ../scripts/settings.tcl

open_project ${project_name}.xpr

set runs [get_runs *synth_1]
foreach run $runs {
        puts "Resetting $run..."
        reset_run $run
}
set work_dir [get_property DIRECTORY [current_project]]
generate_target all -force [get_files ${work_dir}/${project_name}.srcs/sources_1/bd/design_1/design_1.bd]
update_compile_order -fileset sources_1

set_property AUTO_INCREMENTAL_CHECKPOINT 1 [get_runs synth_1]
set_property STEPS.SYNTH_DESIGN.ARGS.RETIMING true [get_runs synth_1]

launch_runs synth_1 -jobs $jobs
puts "NOTE: You can watch the progress with the following command: \
              tail -F build/${project_name}.runs/*/runme.log"
wait_on_run synth_1

set runs [get_runs *impl_1]
foreach run $runs {
        puts "Resetting $run..."
        reset_run $run
}

set_property AUTO_INCREMENTAL_CHECKPOINT 1 [get_runs impl_1]

launch_runs impl_1 -jobs $jobs -to_step write_bitstream
puts "NOTE: You can watch the progress with the following command: \
              tail -F build/${project_name}.runs/*/runme.log"
wait_on_run impl_1

#file copy -force ${work_dir}/${project_name}.runs/impl_1/design_1_wrapper.sysdef ${work_dir}/../../sw/design_1_wrapper.hdf
write_hw_platform -fixed -force -include_bit -file ${work_dir}/../../sw/xsa/design_1_wrapper.xsa
