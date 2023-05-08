#!/bin/bash

if [ "$1" = "BLOCKING" ];
then
	expected_data="expected_data_blocking.txt"
	recv_data="recv_data_blocking.txt"
else
	expected_data="expected_data_non_blocking.txt"
	recv_data="recv_data_non_blocking.txt"
fi

diff $expected_data $recv_data
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


