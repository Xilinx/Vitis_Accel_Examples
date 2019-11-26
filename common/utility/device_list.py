#!/usr/bin/env python

#
# utility that creates a file that lists down the supported and unsupported devices
# for each example
#

import glob
import json
import re
import sys
import os

import os.path

string = "" 
for dirpath, dirnames, filenames in os.walk("../../."):   
	for filename in [f for f in filenames if (f.endswith("description.json") and f not in "../../common/.")]:
	
		f = open(os.path.join(dirpath, filename), "r+")
		listing = []
		flag = 0 
		name_flag = 0

		for txt in f:

			x = re.search(".*device\".*", txt)

			if (x):
				if(name_flag is 0):
                                        name_flag = 1
                                        string = string + "\n" + dirpath + "\n"

				if(',' not in txt):	
					flag = 1
				
				string = string + txt
				continue	

			if (flag):
				string = string + txt
				
				if(']' in txt):
					flag = 0
		f.close()

g = open ("Data.txt", "w")
g.write(string)
g.close()
	
