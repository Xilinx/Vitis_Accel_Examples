#!/bin/bash -f
export XTLM_LOG_STATE=WAVEFORM_AND_LOG
echo " running simulate.sh $*"
./simulate.sh $* >output.txt 2>&1
