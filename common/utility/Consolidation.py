#!/usr/bin/env python

#
# utility that creates a folder that contains the common folder and
# all the files of the example 
#

from sys import argv
import re
import sys
import os
import os.path

path = os.getcwd()

actual_folder = path
folder_created = path + '_backup'
route = argv[0].split('common')

if(not os.path.isdir(folder_created)):
	cmd = 'cp -rf ' + actual_folder + ' ' + folder_created
	os.system(cmd)
	os.chmod(folder_created, 0o777)

	f = open(folder_created + '/Makefile', "r+")

	string = ""
	listing = ['opencl']

	for txt in f:

		if "COMMON_REPO ?=" in txt:
			txt = "COMMON_REPO = ./\n"

		string = string + txt

	f.close()

	f = open(folder_created + '/description.json', "r+")

	flag = 0
	for txt in f:
		if "\"includepaths\"" in txt:
			flag = 1
			continue

		if (flag == 1):
			if (']' in txt or '}' in txt):
				break
			else:
				listing.append(txt[txt.find("includes/") + 9 : txt.rfind('\"')])
		 
	f.close()

	commonfolders = route[0] + "common/includes/"

	for foldername in os.listdir(commonfolders):
		if foldername in listing:
			cmd1 = 'mkdir -p ' + folder_created + '/common/includes/' + foldername
			cmd2 = 'cp -rf ' + commonfolders + '/' + foldername + '/* ' + folder_created + '/common/includes/' + foldername
			os.system(cmd1)
			os.system(cmd2)

	cmd1 = 'mkdir -p ' + folder_created + '/common/utility/'
	cmd2 = 'cp -rf ' + route[0] + '/common/utility/run_emulation.pl ' + folder_created + '/common/utility/'
	os.system(cmd1)
	os.system(cmd2)

	g = open(folder_created + '/Makefile', "w")
	g.write(string)
	g.close()

	print ("The new folder's location is %s" % folder_created)
