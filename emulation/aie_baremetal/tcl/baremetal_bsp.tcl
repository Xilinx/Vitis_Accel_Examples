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

set XSA_NAME_FIXED [lindex $argv 0]
set DOMAIN standalone 
set PROC psv_cortexa72_0
set OS standalone
set APP_NAME standalone

set curdir [pwd]
set WORKSPACE "$curdir/workspace_fixed"
puts "XSA is $XSA_NAME_FIXED"

# Deleting the previously created workspace
if {[file isdirectory $WORKSPACE]} {
    puts "Deleting $WORKSPACE"
    file delete -force $WORKSPACE
}

#setting workspace
setws $WORKSPACE

set fail_bsp 0
set pass_bsp 0
set fail_app 0
set pass_app 0

# This is getting called for BSP sources creation
app create -name $APP_NAME -hw $curdir/$XSA_NAME_FIXED.xsa -proc $PROC -os $OS -template "Hello World"
puts "Workspace created "
#Building the BSP sources for lib creation
app build -name $APP_NAME
#app build -name $APP_NAME

#checking for BSP successful/not
if {[file exists $WORKSPACE/$XSA_NAME_FIXED/export/$XSA_NAME_FIXED/sw/$XSA_NAME_FIXED/boot/$XSA_NAME_FIXED.bif] } {
    incr pass_bsp
    puts $pass_bsp
    puts "$OS domain created successfully for $XSA_NAME_FIXED"
} else {
    incr fail_bsp
    puts $fail_bsp
    puts "$OS domain creation failed for $XSA_NAME_FIXED"
}

#checking for APP is successful/not	
if {[file exists $WORKSPACE/$APP_NAME/Debug/$APP_NAME.elf]} {
    incr pass_app
    puts "application project is created successfully:"
} else {
    incr fail_app
    puts "application project creation failed"
}

puts "fail bsp $fail_bsp"
puts "fail app $fail_app"


if { $fail_app > 0 || $fail_bsp >0 } {
    puts "Test Failed"
    exit 1
}
puts "Test Passed"
exit 0
