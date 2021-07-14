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

if {[file exists "myadder1_ex"]} {
    file delete -force "myadder1_ex"
}
if {[file exists "project_1"]} {
    file delete -force "project_1"
}

create_project project_1 project_1 -part $partname
create_ip -name rtl_kernel_wizard -vendor xilinx.com -library ip -version 1.0 -module_name myadder1

set_property -dict [list CONFIG.Component_Name {myadder1} CONFIG.KERNEL_NAME {myadder1} CONFIG.KERNEL_CTRL {ap_ctrl_hs} CONFIG.NUM_INPUT_ARGS {0} CONFIG.NUM_M_AXI {0} CONFIG.NUM_AXIS {2} CONFIG.AXIS00_NAME {out} CONFIG.AXIS01_NAME {in}] [get_ips myadder1]

generate_target {instantiation_template} [get_files myadder1.xci]
set_property generate_synth_checkpoint false [get_files myadder1.xci]

open_example_project -force -in_process -dir [pwd] [get_ips myadder1]

# --------------------------------------------
# Start: RTL Kernel Packaging of Sources
#
source -notrace myadder1_ex/imports/package_kernel.tcl
# Packaging project
package_project myadder1_ex/myadder1 xilinx.com user myadder1
package_xo  -xo_path myadder1_ex/imports/myadder1.xo -kernel_name myadder1 -ip_directory myadder1_ex/myadder1 -kernel_xml myadder1_ex/imports/kernel.xml -kernel_files myadder1_ex/imports/myadder1_cmodel.cpp
# Complete: RTL Kernel Packaging of Sources
# --------------------------------------------

set xo_path [file join [pwd] ${xo_pathname}]
if {[file exists "myadder1_ex/imports/myadder1.xo"]} {
    file copy myadder1_ex/imports/myadder1.xo ${xo_path}
} else {
    puts "ERROR: myadder1_ex/imports/myadder1.xo does not exist!\n"
    exit 1
}
