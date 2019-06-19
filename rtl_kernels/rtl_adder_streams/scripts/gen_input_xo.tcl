set xo_pathname  [lindex $::argv 0]

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
