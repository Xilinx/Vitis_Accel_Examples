#!/bin/bash
diff expected_data.txt recv_data.txt
if [ $? == 0 ]
then 
 {
  echo "TEST PASSED" 
 }
else
 {
   echo "TEST FAILED"
 }
fi


