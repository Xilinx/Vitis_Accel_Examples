#!/bin/bash

helpFunction()
{
   echo ""
   echo "Usage: $0 -i IN_DIR -o OUT_FILE"
   echo -i "\t-i Input directory contains aie_rumtime_control.cpp file"
   echo -o "\t-o Output file to store the generated binary ucode"
   echo -c "\t-c setup AXI-MM channel number"
   exit 1 # Exit script after printing help
}

while getopts "i:o:c:h" opt
do
   case "$opt" in
      i ) in_dir="$OPTARG" ;;
      o ) out_file="$OPTARG" ;;
      c ) ch_num="$OPTARG" ;;
      h ) helpFunction ;;
      ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameters are empty
if [ -z "$in_dir" ] || [ -z "$out_file" ]
then
   echo "Some or all of the parameters are empty";
   helpFunction
fi

if [ -z "$ch_num" ]
then
    ch_num=2
fi

# Begin script in case all parameters are correct
#echo "$in_dir"
#echo "$out_file"
dir=$(dirname $(readlink -f "${BASH_SOURCE[0]}"))
make run -f $dir/build.mk IN_DIR=$in_dir OUT_FILE=$out_file CH_NUM=$ch_num

