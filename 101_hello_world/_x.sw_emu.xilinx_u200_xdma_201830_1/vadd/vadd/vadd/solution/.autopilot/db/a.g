#!/bin/sh
lli=${LLVMINTERP-lli}
exec $lli \
    /proj/xhdhdstaff3/rsarkari/scout/ScoutExamples/101_hello_world/_x.sw_emu.xilinx_u200_xdma_201830_1/vadd/vadd/vadd/solution/.autopilot/db/a.g.bc ${1+"$@"}
