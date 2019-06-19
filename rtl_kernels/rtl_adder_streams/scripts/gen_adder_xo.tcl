set xo_pathname  [lindex $::argv 0]

if {[file exists "kernel_2.xo"]} {
    file delete -force "kernel_2.xo"
}

package_xo -xo_path kernel_2.xo -kernel_name krnl_adder_stage_rtl -ip_directory src/ip/ip_2 -kernel_xml src/kernel_adder_stage.xml

set xo_path [file join [pwd] ${xo_pathname}]
if {[file exists "kernel_2.xo"]} {
    file copy kernel_2.xo ${xo_path}
} else {
    puts "ERROR: kernel_2.xo does not exist!\n"
    exit 1
}
