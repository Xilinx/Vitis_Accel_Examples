set xo_pathname  [lindex $::argv 0]

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
