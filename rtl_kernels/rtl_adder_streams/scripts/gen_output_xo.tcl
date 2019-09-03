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

if {[file exists "kernel_3.xo"]} {
    file delete -force "kernel_3.xo"
}

package_xo -xo_path kernel_3.xo -kernel_name krnl_output_stage_rtl -ip_directory src/ip/ip_3 -kernel_xml src/kernel_output_stage.xml

set xo_path [file join [pwd] ${xo_pathname}]
if {[file exists "kernel_3.xo"]} {
    file copy kernel_3.xo ${xo_path}
} else {
    puts "ERROR: kernel_3.xo does not exist!\n"
    exit 1
}
