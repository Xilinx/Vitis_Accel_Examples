#!/usr/bin/env python
from sys import argv
import json
import glob
import os
import re
import subprocess

script, desc_file = argv
desc = open(desc_file, 'r')
data = json.load(desc)
desc.close()

#top level list : dict_new
dict_new = {}
if 'example' in data:
	dict_new['name'] = data.pop('example')
if 'overview' in data:
	dict_new['description'] = data.pop('overview')
if 'board' in data:
	dict_new['device'] = data.pop('board')
if 'nboard' in data:
	dict_new['ndevice'] = data.pop('nboard')
if 'sdx_gui' in data:
	dict_new['gui'] = data.pop('sdx_gui')



host_list = []
host_dict = {}
if 'host_exe' in data:
	host_dict['host_exe'] = data.pop('host_exe')
if 'host_srcs' in data:
	srcs = data['host_srcs'].split(" ")
	host_dict['sources'] = srcs
	del data['host_srcs'] 
if 'host_hdrs' in data:
	hdrs = data['host_hdrs'].split(" ")
	host_dict['sources'].extend(hdrs)
	del data['host_hdrs']

linker_list = []
linker_dict = {}
library_paths = []
library_paths.append("REPO_DIR/common/includes/")
linker_dict['librarypaths'] = library_paths
if 'libs' in data:
	library = []
	for item in data['libs']:
		library.append(item)
	linker_dict['libraries'] = library
if 'linker' in data:
	linker_dict.update(data['linker'])
	del data['linker']
linker_list.append(linker_dict)
host_dict['linker'] = linker_list
	 


if 'libs' or 'compiler' in data:
	compiler_list = []
	compiler_dict = {}
	if 'libs' in data:	
		include_paths = []
		for item in data['libs']:
			include_paths.append('REPO_DIR/common/includes/'+ item)	
		compiler_dict['includepaths'] = include_paths
		del data['libs']
	if 'compiler' in data:
		compiler_dict.update(data['compiler'])
	compiler_list.append(compiler_dict)
	host_dict['compiler'] = compiler_list

launch_list = []

if 'cmd_args' in data:
	emu_cmd_dictny = {}
	emu_cmd_dictny['name'] = 'sw_emu'
	emu_cmd_dictny['cmd_args'] = data['cmd_args']
	launch_list.append(emu_cmd_dictny)
	emu_cmd_dictny1 = {}
	emu_cmd_dictny1['name'] = 'hw_emu'
	emu_cmd_dictny1['cmd_args'] = data.pop('cmd_args')
	launch_list.append(emu_cmd_dictny1)
dict_new['launch'] = launch_list  


host_list.append(host_dict)
dict_new['host'] = host_list
data.update(dict_new)

target = open('description.json', 'w+')
json.dump(data, target, indent=4, sort_keys=False)
