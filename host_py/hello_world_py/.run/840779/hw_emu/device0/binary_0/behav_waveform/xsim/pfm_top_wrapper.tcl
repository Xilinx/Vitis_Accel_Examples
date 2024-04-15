set curr_wave [current_wave_config]
if { [string length $curr_wave] == 0 } {
  if { [llength [get_objects]] > 0} {
    add_wave /
    set_property needs_save false [current_wave_config]
  } else {
     send_msg_id Add_Wave-1 WARNING "No top level signals found. Simulator will start without a wave window. If you want to open a wave window go to 'File->New Waveform Configuration' or type 'create_wave_config' in the TCL console."
  }
}

if { [file exists vitis_params.tcl] } {
  source vitis_params.tcl
}

if { [info exists ::env(USER_PRE_SIM_SCRIPT)] } {
  if { [catch {source $::env(USER_PRE_SIM_SCRIPT)} msg] } {
    puts $msg
  }
}

if { [info exists ::env(VITIS_WAVEFORM)] } {
  if { [file exists $::env(VITIS_WAVEFORM)] == 1} {
    open_wave_config $::env(VITIS_WAVEFORM)
  }
}

if { [file exists pre_sim_tool_scripts.tcl] } {
  source pre_sim_tool_scripts.tcl
}

puts "We are running simulator for infinite time. Added some default signals in the waveform. You can pause simulation and add signals and then resume the simulaion again."
puts ""
puts "Stopping at breakpoint in simulator also stops the host code execution"
puts ""
if { [info exists ::env(VITIS_LAUNCH_WAVEFORM_GUI) ] } {
  run 1ns
} else {
  run all
}

if { [file exists post_sim_tool_scripts.tcl] } {
  source post_sim_tool_scripts.tcl
}

if { [info exists ::env(VITIS_LAUNCH_WAVEFORM_BATCH) ] } {
  if { [info exists ::env(USER_POST_SIM_SCRIPT) ] } {
    if { [catch {source $::env(USER_POST_SIM_SCRIPT)} msg] } {
      puts $msg
    }
  }
  quit
}
