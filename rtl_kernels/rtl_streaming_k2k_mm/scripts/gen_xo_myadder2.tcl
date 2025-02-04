#
# Copyright (C) 2019-2021 Xilinx, Inc
#
# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#
if { $::argc != 5 } {
    puts "ERROR: Program \"$::argv0\" requires 5 arguments!\n"
    puts "Usage: $::argv0 <xoname> <krnl_name> <target> <xpfm_path> <device>\n"
    exit
}

set xo_pathname  [lindex $::argv 0]
set krnl_name [lindex $::argv 1]
set target    [lindex $::argv 2]
set xpfm_path [lindex $::argv 3]
set device    [lindex $::argv 4]

set pinfo [file join [pwd] "pinfo.json"]
[exec $::env(XILINX_VITIS)/bin/platforminfo -p ${xpfm_path} --json=hardwarePlatform.devices[0].fpgaPart >> $pinfo]

set fid [open $pinfo r]
gets $fid line
regsub -all ":" $line - line
set fields [split $line "-"]
append l1 [lindex $fields 1] -
append l1 [lindex $fields 2] -
append l1 [lindex $fields 3] 
append l1 [lindex $fields 4] -
append l1 [lindex $fields 5]
set partname $l1
puts "partname = $l1\n"

if {[file exists "myadder2_ex"]} {
    file delete -force "myadder2_ex"
}
if {[file exists "project_2"]} {
    file delete -force "project_2"
}

create_project project_2 project_2 -part $partname
create_ip -name rtl_kernel_wizard -vendor xilinx.com -library ip -version 1.0 -module_name myadder2

set_property -dict [list CONFIG.Component_Name {myadder2} CONFIG.KERNEL_NAME {myadder2} CONFIG.KERNEL_CTRL {ap_ctrl_hs} CONFIG.NUM_INPUT_ARGS {0} CONFIG.NUM_M_AXI {0} CONFIG.NUM_AXIS {2} CONFIG.AXIS00_NAME {out} CONFIG.AXIS01_NAME {in}] [get_ips myadder2]

generate_target {instantiation_template} [get_files myadder2.xci]
set_property generate_synth_checkpoint false [get_files myadder2.xci]

open_example_project -force -in_process -dir [pwd] [get_ips myadder2]

# --------------------------------------------
# Start: RTL Kernel Packaging of Sources
#
source -notrace myadder2_ex/imports/package_kernel.tcl
# Packaging project
package_project myadder2_ex/myadder2 xilinx.com user myadder2
package_xo  -xo_path myadder2_ex/imports/myadder2.xo -kernel_name myadder2 -ip_directory myadder2_ex/myadder2 -kernel_xml myadder2_ex/imports/kernel.xml -kernel_files myadder2_ex/imports/myadder2_cmodel.cpp
# Complete: RTL Kernel Packaging of Sources
# --------------------------------------------

set xo_path [file join [pwd] ${xo_pathname}]
if {[file exists "myadder2_ex/imports/myadder2.xo"]} {
    file copy myadder2_ex/imports/myadder2.xo ${xo_path}
} else {
    puts "ERROR: myadder2_ex/imports/myadder2.xo does not exist!\n"
    exit 1
}
