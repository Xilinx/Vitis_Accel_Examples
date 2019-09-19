#!/usr/bin/tclsh
proc delete_directory {mydirectory} {
  set status [catch {file delete -force $mydirectory} result]
  if {$status != 0} {
    puts "WARNING: Cannot delete directory '$mydirectory' because it is being used. Ensure all files under it are closed."
    exit 1
  }
}
proc delete_file {myfile} {
  set status [catch {file delete -force $myfile} result]
  if {$status != 0} {
    puts "WARNING: Cannot delete file '$myfile' because it is being used."
    exit 1
  }
}
set save_dir [pwd] 
## the directory from where you run emulation
cd [file join [pwd] _vimage emulation]
if {[file exists sd_card]} {
    delete_directory sd_card
}
if {[file exists sd_card.img]} {
    delete_file sd_card.img
}
file mkdir sd_card
# Copy files specified in manifest
set fp [open sd_card.manifest r]
set data [join [split [read $fp] "\n"]]
foreach line $data {    
    file copy -force $line sd_card
}
file mkdir [file join "sd_card" "data" ]
file mkdir [file join "sd_card" "data" "emulation" ]
file copy -force [file join $env(XILINX_VITIS) "data" "emulation" "unified" ]  [file join "sd_card" "data" "emulation" "unified" ]
close $fp
cd $save_dir
