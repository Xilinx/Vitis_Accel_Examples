#!/bin/bash
echo "launching traffic generator process"
if [ "$1" == "xsim" ]; then
  (./scripts/run_xsim.sh > tg_run.log ) &
else
  echo "unsupported Simulator for TG process"
fi

