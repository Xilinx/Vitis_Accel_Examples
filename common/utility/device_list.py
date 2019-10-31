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
		condition = 0
		listing = []
		flag = 0 

		for txt in f:

			x = re.search(".*device\".*", txt)

			if (x and flag == 0):
				flag = 1
				string = string + "\n" + dirpath +  "\n" + txt
				continue

			if (flag):
				string = string + txt
				
				if(']' in txt):
					break
		f.close()

g = open ("Data.txt", "w")
g.write(string)
g.close()
	
