#!/bin/bash
grep "TEST DONE : Test Completed Successfully"  tg_run.log
if [ $? == 0 ]
then 
 {
  echo "TEST PASSED"
 }
else
 {
  echo "ERROR: TEST FAILED"
 }
fi
