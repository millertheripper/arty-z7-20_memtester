set sources ../source

add_file -fileset constrs_1 $sources/constraints -force
add_file -fileset sources_1 $sources/hdl -force
add_file $sources/misc -force
if {$::externals != ""} {
	add_file -fileset sources_1 -force -norecurse $::externals
}
set ip_srcs [glob -directory $sources/ip -nocomplain -- "*.xci"]
foreach ip_src $ip_srcs {
        import_ip $ip_src
}
set_property "ip_repo_paths" "[file normalize "$sources/ip_cores"]" [get_filesets sources_1]
update_ip_catalog -rebuild
