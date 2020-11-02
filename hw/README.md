# Vivado Project Template

## Creating a new project with this template

1. Copy this entire directory to a new location and modify the file in `scripts/settings.tcl` so that the `project_name` and `part` values reflect the correct values for your new project.
2. If you have any existing sources, IPs, block designs, or constraints, copy them to their respective directory in the `source` directory..
3. Run `make` in order to generate the initial Vivado project. Further development on the project can be carried out with Vivado by opening the project in the `build` directory.

If your project does *not* make use of a block design, or the block design is not the top module, change the `top_module` variable in the `scripts/settings.tcl` file to reflect the name of the correct top level module.

## Building a Bitstream

To generate a Bitstream, simply enter `make`. The project will automatically be generated, synthesised, and implemented.

## Adding external HDL files

## Save changes
- Updating Block Diagram from Vivado TCL shell: 
- `write_bd_tcl [get_property DIRECTORY [current_project]]/../source/scripts/bd.tcl -include_layout -force`
