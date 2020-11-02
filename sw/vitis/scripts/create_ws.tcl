setws .
app create -name fsbl -hw ./xsa/[lindex $argv 0].xsa -os standalone -proc ps7_cortexa9_0  -template {Zynq FSBL}
app create -name app -platform [lindex $argv 0] -os standalone -proc ps7_cortexa9_0  -template {Empty Application}
app config -name fsbl define-compiler-symbols {FSBL_DEBUG_INFO}
app config -name fsbl define-compiler-symbols {FSBL_DEBUG}
importsources -name app -path [lindex $argv 1] -soft-link
app config -add -name app include-path [lindex $argv 1]
app config -set -name app linker-script [lindex $argv 1]/lscript.ld
app build -name fsbl
app build -name app
exit
