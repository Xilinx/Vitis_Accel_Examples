if { [file exists preprocess_profile.tcl] } {
  if { [catch {source -notrace preprocess_profile.tcl} msg] } {
    puts $msg 
  }
}
if { [info exists ::env(XILINX_SC_HW_EMU)] } {
  source $::env(XILINX_VITIS)/system_compiler/examples/sc_hw_emu_pre_xsim.tcl
}
