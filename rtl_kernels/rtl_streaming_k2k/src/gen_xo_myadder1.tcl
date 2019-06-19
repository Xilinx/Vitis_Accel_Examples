# /*******************************************************************************
# Copyright (c) 2019, Xilinx, Inc.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# 
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software
# without specific prior written permission.
# 
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# *******************************************************************************/
if { $::argc != 2 } {
	puts "ERROR: Application \"$::argv0\" requires 2 arguments!\n"
	puts "Usage: $::argv0 <xpfm_path> <xo_pathname>\n"
	exit
}

set xpfm_path    [lindex $::argv 0]
set xo_pathname  [lindex $::argv 1]

set pinfo [file join [pwd] "pinfo.json"]
if {[file exists ${xpfm_path}]} {
    exec $::env(XILINX_SDX)/bin/platforminfo -j $pinfo -p ${xpfm_path}
}

if {[file exists "myadder1_ex"]} {
    file delete -force "myadder1_ex"
}
if {[file exists "project_1"]} {
    file delete -force "project_1"
}

if {![file exists $pinfo]} {
    puts "ERROR: $pinfo does not exist!"
}

set fid [open $pinfo r]
set fpgapart "fpgaPart"
while { ! [eof $fid] } {
    gets $fid line
    if { [regexp {([^:[:space:]]+): (.*),$} $line match left right] } {
	regsub -all {\"} $left {} left
	regsub -all {\"} $right {} right
	if { $left eq $fpgapart } {
	    if { [string match {*:*} $right] } {
                set fields [split $right ":"]
	        set f0 [lindex $fields 0]
	        set f1 [lindex $fields 1]
	        set f2 [lindex $fields 2]
	        set f3 [lindex $fields 3]
	        set f4 [lindex $fields 4]
	        set partname "${f1}-${f2}${f3}-${f4}"
 	        puts "partname = $partname\n"
	        break
	    }
        }
    }
}
close $fid

create_project project_1 project_1 -part $partname
create_ip -name sdx_kernel_wizard -vendor xilinx.com -library ip -version 1.0 -module_name myadder1

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
package_xo  -xo_path myadder1_ex/sdx_imports/myadder1.xo -kernel_name myadder1 -ip_directory myadder1_ex/myadder1 -kernel_xml myadder1_ex/imports/kernel.xml -kernel_files myadder1_ex/imports/myadder1_cmodel.cpp
# Complete: RTL Kernel Packaging of Sources
# --------------------------------------------

set xo_path [file join [pwd] ${xo_pathname}]
if {[file exists "myadder1_ex/sdx_imports/myadder1.xo"]} {
    file copy myadder1_ex/sdx_imports/myadder1.xo ${xo_path}
} else {
    puts "ERROR: myadder1_ex/sdx_imports/myadder1.xo does not exist!\n"
    exit 1
}
