#!/bin/bash

if [ "$1" = "BLOCKING" ];
then
	expected="expected_blocking.txt"
	recv_data="recv_data_blocking.txt"
else
	expected="expected_non_blocking.txt"
	recv_data="recv_data_non_blocking.txt"
fi

tail -n+5 slave.log > $recv_data
sed -i "/Outstanding/d" $recv_data
sed -i "/Terminating/d" $recv_data

head -n-1 $recv_data >> recv_data_temp.txt
mv recv_data_temp.txt $recv_data
diff $expected $recv_data
if [ $? == 0 ]
then 
 {
  echo "$1 : TEST PASSED" 
 }
else
 {
   echo "ERROR: $1 : TEST FAILED"
 }
fi
