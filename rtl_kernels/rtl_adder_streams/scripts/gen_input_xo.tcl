if { $::argc != 5 } {
    puts "ERROR: Program \"$::argv0\" requires 4 arguments!\n"
    puts "Usage: $::argv0 <xoname> <krnl_name> <target> <xpfm_path> <device>\n"
    exit
}

set xo_pathname  [lindex $::argv 0]
set krnl_name [lindex $::argv 1]
set target    [lindex $::argv 2]
set xpfm_path [lindex $::argv 3]
set device    [lindex $::argv 4]

if {[file exists "kernel_1.xo"]} {
    file delete -force "kernel_1.xo"
}

package_xo -xo_path kernel_1.xo -kernel_name krnl_input_stage_rtl -ip_directory src/ip/ip_1 -kernel_xml src/kernel_input_stage.xml

set xo_path [file join [pwd] ${xo_pathname}]
if {[file exists "kernel_1.xo"]} {
    file copy kernel_1.xo ${xo_path}
} else {
    puts "ERROR: kernel_1.xo does not exist!\n"
    exit 1
}
