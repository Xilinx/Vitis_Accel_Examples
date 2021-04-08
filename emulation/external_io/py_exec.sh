#!/bin/bash
#stream_scripts
echo "master launched"
(./master.sh >  master.log ) &
echo "slave launched" 
(./slave.sh > slave.log) &
