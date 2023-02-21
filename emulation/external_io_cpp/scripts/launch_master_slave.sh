#!/bin/bash
#stream_scripts
rm -rf master.log slave.log ../expected.txt
echo "master launched"
(./scripts/master.sh $1 >  master.log ) &
echo "slave launched" 
(./scripts/slave.sh $1 > slave.log) &
