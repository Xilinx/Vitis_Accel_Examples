#
# File: profile.tcl
# Description: Script to analyze HW emulation results
# Created: 2024-04-15 10:42:03
#

global ofp
global ofp2

# Log results (file 1 for summary)
proc putsLog {str} {
  global ofp

#  puts $str
  puts $ofp $str
}

# Log results (file 2 for trace)
proc putsLog2 {str} {
  global ofp2

#  puts $str
  puts $ofp2 $str
}

# Get end time in ps (by default)
proc getEndTime {} {
  return [lindex [now] 0]
}

# Check if signal has too many transitions to be logged/processed efficiently
# Check for transitions in given time range near the start and check again near the end
# Also, calculate total time at a given target value in given time range from startTime
proc checkTransitions {signal timeRange startTimeUs endTimeUs max targetValue {requireEnd 0}} {
  set retList {}
  if {[get_objects -quiet $signal] == {}} {
    lappend retList 0
    return $retList
  }
  # Check for transitions in given time range at the given startTimeUs 
  set firstEndTimeUs [expr $startTimeUs + $timeRange]
  append startTimeUs us
  append firstEndTimeUs us

  set tranx [get_transitions $signal -startTime $startTimeUs -endTime $firstEndTimeUs]

  set numTranx [llength $tranx] 

  if { $numTranx < $max } {
    if { $endTimeUs < $timeRange } {
      lappend retList 0
      return $retList
    }
    # Check again for transitions in given time range at the given endTimeUs 
    set lastStartTimeUs [expr $endTimeUs - $timeRange]
    append endTimeUs us
    append lastStartTimeUs us

    set tranx2 [get_transitions $signal -startTime $lastStartTimeUs -endTime $endTimeUs]

    set numTranx2 [llength $tranx2] 

    if { $numTranx2 < $max } {
      lappend retList 0
      return $retList
    }
    puts "WARNING: $signal has $numTranx2 transitions in $lastStartTimeUs to $endTimeUs. Too many transitions to process efficiently."
  }

  puts "WARNING: $signal has $numTranx transitions in $startTimeUs to $firstEndTimeUs. Too many transitions to process efficiently."
  lappend retList 1
  # Calculate total time at given targetValue in the given time slice
  set otherValue [expr 1 - $targetValue]
  set factor 6.0
  set startTimeUsec 0
  set totalTimeUsec 0

  for { set i 0 } { $i < $numTranx } { incr i } {
    set tr [lindex $tranx $i]
    set currTime  [lindex $tr 0]
    set currUnit  [lindex $tr 1]
    set currValue [lindex $tr 2]
    if {[string first ns $currUnit] == 0} {
      set factor 3.0
    } elseif {[string first ps $currUnit] == 0} {
      set factor 6.0
    } elseif {[string first fs $currUnit] == 0} {
      set factor 9.0
    } elseif {[string first us $currUnit] == 0} {
      set factor 0.0
    }

    set currTimeUsec [expr $currTime / pow(10.0, $factor)]
    if {$currValue == $targetValue} {
      set startTimeUsec $currTimeUsec
    } elseif { ($currValue == $otherValue) && ($startTimeUsec > 0) && ($currTimeUsec > $startTimeUsec) } {
      set totalTimeUsec [expr $totalTimeUsec + ($currTimeUsec - $startTimeUsec)]
      set startTimeUsec 0
    }
  }

  if {$totalTimeUsec == 0 && $requireEnd} {
    set endTimeUsec [ expr ([getEndTime] / pow(10.0, 6.0))]
    set totalTimeUsec [expr ($endTimeUsec - $startTimeUsec)]
  }
  lappend retList $totalTimeUsec
  return $retList
}

# Get a list with first start time, last end time and total time of signal at a specified value
proc getTimeUsec {signal targetValue {requireEnd 0}} {
  set retList {}
  if {[get_objects -quiet $signal] == {}} {
    lappend retList 0 0 0
    return $retList
  }
  set tranx [get_transitions $signal -start 0 -end [getEndTime]]

  set otherValue [expr 1 - $targetValue]
  set startTimeUsec 0
  set endTimeUsec   0
  set totalTimeUsec 0
  set factor 6.0

  for { set i 0 } { $i < [llength $tranx] } { incr i } {
    set tr [lindex $tranx $i]
    set currTime  [lindex $tr 0]
    set currUnit  [lindex $tr 1]
    set currValue [lindex $tr 2]
    if {[string first ns $currUnit] == 0} {
      set factor 3.0
    } elseif {[string first ps $currUnit] == 0} {
      set factor 6.0
    } elseif {[string first fs $currUnit] == 0} {
      set factor 9.0
    } elseif {[string first us $currUnit] == 0} {
      set factor 0.0
    }

    set currTimeUsec [expr $currTime / pow(10.0, $factor)]
    if {$currValue == $targetValue} {
      set startTimeUsec $currTimeUsec
      if { [llength $retList] == 0 } {
        lappend retList $startTimeUsec
      }
    } elseif { ($currValue == $otherValue) && ($startTimeUsec > 0) && ($currTimeUsec > $startTimeUsec) } {
      set totalTimeUsec [expr $totalTimeUsec + ($currTimeUsec - $startTimeUsec)]
      set endTimeUsec   $currTimeUsec
      set startTimeUsec 0
    }
  }

  if {$totalTimeUsec == 0 && $requireEnd} {
    set endTimeUsec [ expr ([getEndTime] / pow(10.0, 6.0))]
    set totalTimeUsec [expr ($endTimeUsec - $startTimeUsec)]
  }
  if { [llength $retList] == 0 } {
    lappend retList $startTimeUsec
  }
  lappend retList $endTimeUsec
  lappend retList $totalTimeUsec
  return $retList
}

# Get events of signal at a specified value
proc getTraceEvents {kernelName cuName moduleName signal targetValue {requireEnd 0}} {
  if {[get_objects -quiet $signal] == {}} {
    return
  }
  set tranx [get_transitions $signal -start 0 -end [getEndTime]]

  set otherValue [expr 1 - $targetValue]
  set startTimeMsec 0
  set factor 9.0
  set currDeviceName xilinx_u250_gen3x16_xdma_4_1_202210_1-0
  set currBinaryName vadd.link

  for { set i 0 } { $i < [llength $tranx] } { incr i } {
    set tr [lindex $tranx $i]
    set currTime  [lindex $tr 0]
    set currUnit  [lindex $tr 1]
    set currValue [lindex $tr 2]
    if {[string first ns $currUnit] == 0} {
      set factor 6.0
    } elseif {[string first ps $currUnit] == 0} {
      set factor 9.0
    } elseif {[string first fs $currUnit] == 0} {
      set factor 12.0
    } elseif {[string first us $currUnit] == 0} {
      set factor 3.0
    }

    set currTimeMsec [expr $currTime / pow(10.0, $factor)]
    if {$currValue == $targetValue} {
      set startTimeMsec $currTimeMsec
    } elseif {$currValue == $otherValue && $startTimeMsec > 0} {
      set endTimeMsec $currTimeMsec
      set traceStr [format "%s,%s,%s,%s,%s,%s,0,%.6f,%.6f,0" $currDeviceName $currBinaryName $kernelName $cuName $moduleName $moduleName $startTimeMsec $endTimeMsec]
      putsLog2 $traceStr
      set startTimeMsec 0
    }
  }
}

# Get events of signal at a specified value
proc getTraceEvents2 {kernelName cuName moduleName type index signal} {
  if {[get_objects -quiet $signal] == {}} {
    puts "Unable to find signal: $signal"
    return
  }
  set tranx [get_transitions $signal]

  set factor 9.0
  set newValue 0
  if {$type == "row"} {
      set newValue 65535
  }
  set startTimeMsec 0
  set currDeviceName xilinx_u250_gen3x16_xdma_4_1_202210_1-0
  set currBinaryName vadd.link
  set hasData 0 

  for { set i 0 } { $i < [llength $tranx] } { incr i } {
    set tr [lindex $tranx $i]
    set currTime   [lindex $tr 0]
    if {$currTime == 0} {
      continue
    }
    set currUnit   [lindex $tr 1]
    if {[string first ns $currUnit] == 0} {
      set factor 6.0
    } elseif {[string first ps $currUnit] == 0} {
      set factor 9.0
    } elseif {[string first fs $currUnit] == 0} {
      set factor 12.0
    } elseif {[string first us $currUnit] == 0} {
      set factor 3.0
    }
    set timeMsec [expr $currTime / pow(10.0, $factor)]

    set traceStr [format "%s,%s,%s,%s,%s,%s,%d,%.6f,%.6f,%d" $currDeviceName $currBinaryName $kernelName $cuName $moduleName $type $index $startTimeMsec $timeMsec $newValue]
    putsLog2 $traceStr

    set currValue [lindex $tr 2]
    set currValueInt [expr 0x$currValue]
    if {$type == "row"} {
      set newValue $currValueInt
    } else {
      set newValue [expr {($currValueInt == 65535) ? 0 : $currValueInt}]
    }
    set startTimeMsec $timeMsec
    set hasData 1 
  }

  # Force last value to be 0
  if {$hasData == 1} { 
    set endValue 0
    if {$type == "row"} { set endValue 65535 }
    set traceStr [format "%s,%s,%s,%s,%s,%s,%d,%.6f,%.6f,%d" $currDeviceName $currBinaryName $kernelName $cuName $moduleName $type $index  $startTimeMsec $startTimeMsec $endValue]
    putsLog2 $traceStr
  }
}

set outFilename profile_kernels.csv
set ofp [open $outFilename w]
set outFilename2 timeline_kernels.csv
set ofp2 [open $outFilename2 w]

#
# Compute Unit Stalls
#
set computeUnits { vadd_1 }
set cuFullPaths { /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ }

putsLog "Compute Units: Running Time and Stalls"
putsLog "Compute Unit, Running Time (us), Intra-Kernel Dataflow Stalls (%), External Memory Stalls (%), External Stream Stalls (%)"

for { set i 0 } { $i < [llength $computeUnits] } { incr i } {
  set cuName [lindex $computeUnits $i]
  set cuFullPath [lindex $cuFullPaths $i]

  # Runtime
  set signal $cuFullPath/ap_idle
  set cuTimes     [getTimeUsec $signal 0 1]
  set cuStartTime [lindex $cuTimes 0]
  set cuEndTime   [lindex $cuTimes 1]
  set cuTime      [lindex $cuTimes 2]

  # Stall times
  set signal $cuFullPath/ap_int_blocking_n
  set timeRange 20
  set checkTransRetList  [checkTransitions $signal $timeRange $cuStartTime $cuEndTime 1000 0]
  set isHighNumTranx     [lindex $checkTransRetList 0]
  set intStallPercent 0
  if { $isHighNumTranx == 0 } {
    set intStallTime [lindex [getTimeUsec $signal 0] 2]
    set intStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $intStallTime) / $cuTime, 100.0)]
  } else {
    puts "WARNING: $signal has too many transitions to process efficiently. So, intra-kernel dataflow stall percent is approximate."
    set intStallTimeUsec [lindex $checkTransRetList 1]
    set intStallPercent [expr (100.0 * $intStallTimeUsec)/$timeRange]
  }

  set signal $cuFullPath/ap_ext_blocking_n
  set extStallTime [lindex [getTimeUsec $signal 0] 2]
  set extStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $extStallTime) / $cuTime, 100.0)]

  set signal $cuFullPath/ap_str_blocking_n
  set strStallTime [lindex [getTimeUsec $signal 0] 2]
  set strStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $strStallTime) / $cuTime, 100.0)]

  set logStr [format "%s,%.3f,%.3f,%.3f,%.3f" $cuName $cuTime $intStallPercent $extStallPercent $strStallPercent]
  putsLog $logStr
}
putsLog ""

#
# Function Stalls
#
set computeUnits { vadd_1 vadd_1 vadd_1 vadd_1 vadd_1 vadd_1 vadd_1 vadd_1 vadd_1 }
set cuFullPaths { /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ }
set modules { entry_proc_U0 Block_entry_split_proc_U0 load_input_U0 load_input_U0/grp_load_input_Pipeline_mem_rd_fu_86 load_input_1_U0 compute_add_U0 compute_add_U0/grp_compute_add_Pipeline_execute_fu_52 store_result_U0 store_result_U0/grp_store_result_Pipeline_mem_wr_fu_77 }
set realModules { entry_proc Block_entry_split_proc load_input load_input/load_input_Pipeline_mem_rd load_input_1 compute_add compute_add/compute_add_Pipeline_execute store_result store_result/store_result_Pipeline_mem_wr }

putsLog "Functions: Running Time and Stalls"
putsLog "Compute Unit, Function, Running Time (us), Intra-Kernel Dataflow Stalls (%), External Memory Stalls (%), External Stream Stalls (%)"

for { set i 0 } { $i < [llength $computeUnits] } { incr i } {
  set cuName [lindex $computeUnits $i]
  set cuFullPath [lindex $cuFullPaths $i]
  set moduleName [lindex $modules $i]
  set moduleRealName [lindex $realModules $i]

  # Runtime
  # NOTE: not all modules have ap_start/ap_idle
  set cuSignal $cuFullPath/ap_idle
  set moduleSignal $cuFullPath/$moduleName/ap_idle
  if {[get_objects -quiet $moduleSignal] == {}} {
    set moduleSignal $cuSignal
  }
  set cuTimes [getTimeUsec $cuSignal 0 1]
  set cuStartTime [lindex $cuTimes 0]
  set cuEndTime   [lindex $cuTimes 1]
  set cuTime      [lindex $cuTimes 2]

  set moduleTime [lindex [getTimeUsec $moduleSignal 0 1] 2]

  # Stall times
  set signal $cuFullPath/$moduleName/ap_int_blocking_n
  set timeRange 20
  set checkTransRetList  [checkTransitions $signal $timeRange $cuStartTime $cuEndTime 1000 0]
  set isHighNumTranx     [lindex $checkTransRetList 0]
  set intStallPercent 0
  if { $isHighNumTranx == 0 } {
    set intStallTime [lindex [getTimeUsec $signal 0] 2]
    set intStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $intStallTime) / $cuTime, 100.0)]
  } else {
    puts "WARNING: $signal has too many transitions to process efficiently. So, intra-kernel dataflow stall percent is approximate."
    set intStallTimeUsec [lindex $checkTransRetList 1]
    set intStallPercent [expr (100.0 * $intStallTimeUsec)/$timeRange]
  }

  set signal $cuFullPath/$moduleName/ap_ext_blocking_n
  set extStallTime [lindex [getTimeUsec $signal 0] 2]
  set extStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $extStallTime) / $cuTime, 100.0)]

  set signal $cuFullPath/$moduleName/ap_str_blocking_n
  set strStallTime [lindex [getTimeUsec $signal 0] 2]
  set strStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $strStallTime) / $cuTime, 100.0)]

  set logStr [format "%s,%s,%.3f,%.3f,%.3f,%.3f" $cuName $moduleName $moduleTime $intStallPercent $extStallPercent $strStallPercent]
  putsLog $logStr
}
putsLog ""

puts "Waiting for protocol analyzers to complete..."
sim_wait_for_processing
puts "Done!"

#
# Compute Unit Ports
#
set computeUnits { vadd_1 vadd_1 }
set cuFullPaths      { /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/ }
set ports { m_axi_gmem0 m_axi_gmem1 }
set readBusySignals { /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/m_axi_gmem0.readActive /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/m_axi_gmem1.readActive }
set writeBusySignals { /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/m_axi_gmem0.writeActive /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/m_axi_gmem1.writeActive }

putsLog "Compute Units: Port Data Transfer"
putsLog "Compute Unit, Port, Write Time (us), Outstanding Write (%), Read Time (us), Outstanding Read (%)"

for { set i 0 } { $i < [llength $computeUnits] } { incr i } {
  set cuName [lindex $computeUnits $i]
  set cuFullPath [lindex $cuFullPaths $i]
  set portName [lindex $ports $i]
  set readBusy [lindex $readBusySignals $i]
  set writeBusy [lindex $writeBusySignals $i]

  # Runtime
  set signal $cuFullPath/ap_idle
  set moduleTime [lindex [getTimeUsec $signal 0 1] 2]

  # Write times
  set writeTime [lindex [getTimeUsec $writeBusy 1] 2]
  set writePercent [expr {$moduleTime == 0} ? 0 : min((100.0 * $writeTime) / $moduleTime, 100.0)]

  # Read times
  set readTime [lindex [getTimeUsec $readBusy 1] 2]
  set readPercent [expr {$moduleTime == 0} ? 0 : min((100.0 * $readTime) / $moduleTime, 100.0)]

  set logStr [format "%s,%s,%.3f,%.3f,%.3f,%.3f" $cuName $portName $writeTime $writePercent $readTime $readPercent]
  putsLog $logStr
}
putsLog ""

#
# Trace Events
#
putsLog2 "Kernel trace events"
putsLog2 "Device, Binary, Kernel, Compute Unit, Function, Data Type, Index, Start Time (ms), End Time (ms), Value"
# Set list of CU, modules, protoinst for kernel vadd at index 0 in " kernels " list declared later 
set cuForKernel0 { vadd_1  } 
set cuPathForKernel0 { /pfm_top_wrapper/pfm_top_i/pfm_dynamic_inst/vadd_1/inst/  } 
set moduleForKernel0 { entry_proc_U0 Block_entry_split_proc_U0 load_input_U0 load_input_U0/grp_load_input_Pipeline_mem_rd_fu_86 load_input_1_U0 compute_add_U0 compute_add_U0/grp_compute_add_Pipeline_execute_fu_52 store_result_U0 store_result_U0/grp_store_result_Pipeline_mem_wr_fu_77  } 
set protoInstForKernel0 { entry_proc_U0 Block_entry_split_proc_U0 load_input_U0 grp_load_input_Pipeline_mem_rd_fu_86 load_input_1_U0 compute_add_U0 grp_compute_add_Pipeline_execute_fu_52 store_result_U0 grp_store_result_Pipeline_mem_wr_fu_77  } 

set kernels { vadd }
for { set i 0 } { $i < [llength $kernels] } { incr i } {
  set kernelName [lindex $kernels $i]
  set computeUnits [set cuForKernel$i] 
  set cuPaths [set cuPathForKernel$i] 
  for { set j 0 } { $j < [llength $computeUnits] } { incr j } {
    set cuName [lindex $computeUnits $j]
    set cuPath [lindex $cuPaths $j]
    set signal $cuPath/ap_idle
    getTraceEvents $kernelName $cuName top $signal 0 1
  }
}

for { set i 0 } { $i < [llength $kernels] } { incr i } {
  set kernelName [lindex $kernels $i]
  set computeUnits [set cuForKernel$i] 
  set cuPaths [set cuPathForKernel$i] 
  set modules [set moduleForKernel$i] 
  set protoInstNames [set protoInstForKernel$i] 
  for { set j 0 } { $j < [llength $computeUnits] } { incr j } {
    set cuName [lindex $computeUnits $j]
    set cuPath [lindex $cuPaths $j]
    for { set k 0 } { $k < [llength $modules] } { incr k } {
      set modulePathName [lindex $modules $k]
      set protoInstName  [lindex $protoInstNames $k]
      set signal $cuPath/$modulePathName/$protoInstName.dataflowTxWaveData.OutStanding
      getTraceEvents2 $kernelName $cuName $modulePathName outstanding 0 $signal
      for { set l 0 } { $l < 64 } { incr l } {
        set signal $cuPath/$modulePathName/$protoInstName.iterationsWave_$l
        getTraceEvents2 $kernelName $cuName $modulePathName row $l $signal
      }
    }
  }
}

close $ofp
close $ofp2
