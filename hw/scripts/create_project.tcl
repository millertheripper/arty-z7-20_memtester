source ../scripts/settings.tcl

create_project -force $project_name -part $part

set ::externals $argv
source ../scripts/add_sources.tcl
source ../scripts/add_bd.tcl

set_property top $top_module [current_fileset]
