#!/bin/sh

# 
# v++(TM)
# runme.sh: a v++-generated Runs Script for UNIX
# Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
# 

if [ -z "$PATH" ]; then
  PATH=/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/SDK/2019.1/bin:/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Scout/2019.1/bin:/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Vivado/2019.1/bin
else
  PATH=/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/SDK/2019.1/bin:/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Scout/2019.1/bin:/proj/xbuilds/2019.1_Scout_0415_0722/installs/lin64/Vivado/2019.1/bin:$PATH
fi
export PATH

if [ -z "$LD_LIBRARY_PATH" ]; then
  LD_LIBRARY_PATH=
else
  LD_LIBRARY_PATH=:$LD_LIBRARY_PATH
fi
export LD_LIBRARY_PATH

HD_PWD='/proj/xhdhdstaff3/rsarkari/scout/ScoutExamples/101_hello_world/_x.sw_emu.xilinx_u200_xdma_201830_1/vadd/vadd'
cd "$HD_PWD"

HD_LOG=runme.log
/bin/touch $HD_LOG

ISEStep="./ISEWrap.sh"
EAStep()
{
     $ISEStep $HD_LOG "$@" >> $HD_LOG 2>&1
     if [ $? -ne 0 ]
     then
         exit
     fi
}

EAStep vivado_hls -f vadd.tcl -messageDb vivado_hls.pb
