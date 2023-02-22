#!/bin/bash
#stream_scripts
echo "master launched"
(./scripts/master.sh $1 >  master.log ) &
echo "slave launched" 
(./scripts/slave.sh $1 > slave.log) &
