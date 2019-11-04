#!/usr/bin/env python

#
# utility that lists down
# all the examples with respective unnecessary keywords mentioned
#

import glob
import json
import re
import sys
import os

import os.path

for dirpath, dirnames, filenames in os.walk("../.././"):   
	for filename in [f for f in filenames if (f.endswith("description.json") and f not in "../../common/.")]:

		f = open(os.path.join(dirpath, filename), "r+")
		flag = 0
		t = 0
		string_check = ""

		for txt in f:
			if ("keywords" in txt and flag == 0):
				flag = 1
				continue

			if (flag):
				if('}' in txt or ']' in txt):
					break

				else:
					c_list = txt.split("\"")
					check_flag = 0
					alt_dirpath = dirpath + "/src"
					for c_dirpath, c_dirnames, c_filenames in os.walk(os.path.join(alt_dirpath)):
						for check_filename in [c_f for c_f in c_filenames]:
							c_f = open(os.path.join(c_dirpath, check_filename), "r+")
						
							for check_txt in c_f:
								if (c_list[1] in check_txt):
									check_flag = 1
									break
						
							c_f.close()

						if (check_flag is 0):
							string_check = string_check + txt
							t = 1

		if (t):
			print(os.path.join(dirpath, filename))
			print(string_check)
		
		f.close()
	
