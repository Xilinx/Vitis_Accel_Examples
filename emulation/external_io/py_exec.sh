#!/bin/bash
#stream_scripts
sleep 200
echo "master launched"
(./master.sh >  master.log ) &
echo "slave launched" 
(./slave.sh > slave.log) &
