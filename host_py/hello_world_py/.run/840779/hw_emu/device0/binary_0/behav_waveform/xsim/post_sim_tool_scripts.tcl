if { [file exists profile.tcl] } {
  if { [catch {source -notrace profile.tcl } msg] } {
    puts $msg
  }
}
if { [info exists ::env(XILINX_SC_HW_EMU)] } {
  source $::env(XILINX_VITIS)/system_compiler/examples/sc_hw_emu_post_xsim.tcl
}
