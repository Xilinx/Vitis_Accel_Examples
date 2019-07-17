#!/usr/bin/env python
from sys import argv
from collections import OrderedDict
import json
import os
import subprocess

DSA = 'xilinx_u200_qdma'
VERSION = 'SCOUT 2019.1'
DEVICES = {
    'xilinx_aws-vu9p-f1-04261818': {
       'version': '5.0',
       'name': 'Xilinx Only 5.0 Shell',
    },	
    'xilinx_u200_qdma': {
       'version': '201910_1',
       'name': 'Xilinx Alveo U200',
       'nae':  'nx5u_xdma_201830_1'
    },
    'xilinx_u280_xdma': {
       'version': '201910_1',
       'name': 'Xilinx Alveo U280'
    },
    'xilinx_u280-es1_xdma': {
       'version': '201910_1',
       'name': 'Xilinx Alveo U280',
       'nae':  'nx7u_xdma_201830_1'
    },
    'xilinx_u200_xdma': {
       'version': '201830_1',
       'name': 'Xilinx Alveo U200',
       'nae':  'nx5u_xdma_201830_1'
    },
    'xilinx_u250_qdma': {
       'version': '201910_1',
       'name': 'Xilinx Alveo U250'
    },
    'xilinx_u250_xdma': {
       'version': '201830_1',
       'name': 'Xilinx Alveo U250',
       'nae':  'nx6u_xdma_201830_1'
    }
}

def overview(target,data):
    target.write(data["name"])
    target.write("\n")
    target.write("======================\n\n")
    target.write(('\n').join(data["description"]))
    target.write("\n\n")
    if 'more_info' in data:
        target.write(('\n').join(data["more_info"]))
        target.write("\n\n")
    if 'perf_fields' in data:
        target.write("### PERFORMANCE\n")
	ctr = len(data["perf_fields"])
	for idx in range(0, ctr - 1):
            target.write(data["perf_fields"][idx])
            target.write("|")
	target.write(data["perf_fields"][ctr - 1])
        target.write("\n")
	for idx in range(0, ctr - 1):
            target.write("-----|")
	target.write("-----\n")
	count = len(data["performance"])	
        for result in range(0, count):
	    for i in range(0, ctr - 1):
		target.write(data["performance"][result][i])
		target.write("|")
            target.write(data["performance"][result][ctr - 1])	
            target.write("\n")
    if 'key_concepts' in data:
        target.write("***KEY CONCEPTS:*** ")
        elem_count = len(data["key_concepts"])
        for result in data["key_concepts"]:
            elem_count -= 1
            target.write(result)
            if elem_count != 0:
                target.write(", ")
        target.write("\n\n")
    if 'keywords' in data:
        target.write("***KEYWORDS:*** ")
        word_count = len(data["keywords"])
        for result in data["keywords"]:
            word_count -= 1
            target.write(result)
            if word_count != 0:
                target.write(", ")
        target.write("\n\n")
    return

def requirements(target,data):
    target.write("## SUPPORTED PLATFORMS\n")
    target.write("Platform | Board             | Software Version\n")
    target.write("---------|-------------------|-----------------\n")

    boards = []
    if 'device' in data:
        board = data['device']
        boards = [word for word in DEVICES if word in board]
    else:
        nboard = []
        if 'ndevice' in data:
            nboard = data['ndevice']
        boards = [word for word in DEVICES if word not in nboard]

    for board in boards:
        target.write(board)
        target.write("|")
        target.write(DEVICES[board]['name'])
        target.write("|")
        target.write(VERSION)
        target.write("\n")
    target.write("\n\n") 
    return

def hierarchy(target):
    target.write("##  DESIGN FILES\n")
    target.write("Application code is located in the src directory. ")
    target.write("Accelerator binary files will be compiled to the xclbin directory. ")
    target.write("The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files ")
    target.write("in this example is shown below\n\n")
    target.write("```\n")
    tree_cmd = ["git ls-files | grep -e data -e src"]
    proc = subprocess.Popen(tree_cmd,stdout=subprocess.PIPE, shell=True)
    output = proc.communicate()[0]
    target.write(output)
    target.write("```\n")
    target.write("\n")
    return

def commandargs(target,data):
    if "launch" in data:
        target.write("##  COMMAND LINE ARGUMENTS\n")
        target.write("Once the environment has been configured, the application can be executed by\n")
        target.write("```\n")
        if not "cmd_args" in data["launch"][0]:
            target.write('./' + data["host"][0]["host_exe"])
        else:
            target.write('./' + data["host"][0]["host_exe"])
            args = data["launch"][0]["cmd_args"].split(" ")
            for arg in args[0:]:
                target.write(" ")
                arg = arg.replace('BUILD/', '<')
                arg = arg.replace('PROJECT', '.')
                arg = arg.replace('.xclbin', ' XCLBIN>') 
                target.write(arg)
        target.write("\n```\n")
        target.write("\n")



# Get the argument from the description
script, desc_file = argv

# load the description file
print "SCOUT README File Genarator"
desc = open(desc_file,'r')

# load the json data from the file
data = json.load(desc)
desc.close()

assert("OpenCL" in data['runtime'])

if "match_readme" in data and data["match_readme"] == "false":
    print "ERROR:: README Manually Edited:: README Generator Failed\n"
else:
    print "Generating the README for %s" % data["name"]
    target = open("README.md","w")
    overview(target,data)
    requirements(target,data)
    hierarchy(target)
    commandargs(target,data)

target.close
