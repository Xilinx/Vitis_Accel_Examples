#!/bin/sh

until [ -f package.hw_emu/sim/behav_waveform/xsim/simulate.log ]
	do
		sleep 5
	done

until grep -E "\[ Vitis_Deadlock_Detector \] Inter Kernel deadlock has been detected" "package.hw_emu/sim/behav_waveform/xsim/simulate.log" ; do sleep 10; done

#QPID=$(grep "qemu_pid" make.log | grep -Po '(?<=qemu_pid=)[^ ]+')
SIMPID=$(grep "simulator_pid" make.log | grep -Po '(?<=simulator_pid )[^ ]+')
kill -9 $SIMPID
echo "TEST PASSED"
exit
