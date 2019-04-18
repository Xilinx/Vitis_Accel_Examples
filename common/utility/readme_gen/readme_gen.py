#!/usr/bin/env python
from sys import argv
from collections import OrderedDict
import json
import os
import subprocess

DSA = 'xilinx_vcu1525_dynamic'
VERSION = 'SDx 2018.3'
DEVICES = {
    'xilinx_u200_xdma_201820_1': {
       'version': '5.0',
       'name': 'Xilinx Alveo U200',
       'nae':  'nx6'
    },
    'xilinx_u250_xdma_201820_1': {
       'version': '5.0',
       'name': 'Xilinx Alveo U250',
       'nae':  'nx7'
    },
    'xilinx_vcu1525_dynamic': {
       'version': '5.0',
       'name': 'Xilinx Virtex UltraScale+ VCU1525',
       'nae':  'nx5'
    }
}

def header(target,data):
    target.write(data["example"])
    target.write("\n")
    target.write("======================\n\n")
    target.write("This file contains the following sections:\n\n")
    target.write("1. SUPPORTED PLATFORMS\n")
    target.write("2. DESIGN FILES\n")
    target.write("3. COMMAND LINE ARGUMENTS\n\n\n")
    return

def overview(target,data):
    target.write(data["example"])
    target.write("\n")
    target.write("======================\n\n")
    target.write(('\n').join(data["overview"]))
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

    target.write("\nFor Setup instructions, please refer: ")
    target.write("[setup.md][]")
    target.write("\n\n")
    target.write("[setup.md]: setup.md")
    target.write("\n\n")
    return

def requirements(target,data):
    target.write("## 1. SUPPORTED PLATFORMS\n")
    target.write("Board | Device Name | Software Version\n")
    target.write("------|-------------|-----------------\n")

    boards = []
    if 'board' in data:
        board = data['board']
        boards = [word for word in DEVICES if word in board]
    else:
        nboard = []
        if 'nboard' in data:
            nboard = data['nboard']
        boards = [word for word in DEVICES if word not in nboard]

    for board in boards:
        target.write(DEVICES[board]['name'])
        target.write("|")
        target.write(board)
        target.write("|")
        for version in VERSION:
            target.write(version)
        target.write("\n")
    target.write("\n\n") 
    return

def hierarchy(target):
    target.write("## 2. DESIGN FILES\n")
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
    target.write("## 3. COMMAND LINE ARGUMENTS\n")
    target.write("Once the environment has been configured, the application can be executed by\n")
    target.write("```\n")
    if not "cmd_args" in data:
        target.write('./' + data["host_exe"])
    else:
        target.write('./' + data["host_exe"])
        args = data["cmd_args"].split(" ")
        for arg in args[0:]:
            target.write(" ")
            arg = arg.replace('BUILD/', '<')
            arg = arg.replace('PROJECT', '.')
            arg = arg.replace('.xclbin', ' XCLBIN>') 
            target.write(arg)
    target.write("\n```\n")
    target.write("\n")



def nimbix(target):
    target.write("The developer instance hosting the Scout tools on Nimbix is not directly connected to an FPGA accelerator card.\n")
    target.write("FPGA Accelerator cards are available as part of the Scout Runtime application. There are several ways of executing an application on the available cards:\n\n")
    target.write("***Submit the application from the developer to runtime instances (recommended flow)***\n")
    target.write("* Create a credentials file for the runtime machine based on your Nimbix username and API key. For more information on how to obtain the API key, refer to ")
    target.write("[Nimbix Application Submission README][]. The credentials file ( ~/.nimbix_creds.json ) should look like\n")
    target.write("```\n")
    target.write("{\n")
    target.write("\t\"username\": \"<username>\",\n")
    target.write("\t\"api-key\": \"<apikey>\"\n")
    target.write("}\n")
    target.write("```\n\n")
    target.write("where the values for username and apikey have been set to the values from your Nimbix account.\n\n")
    target.write("*NOTE:* The home directory of a Scout developer instance is not persistent across sessions. Only files stored in the /data directory are kept between sessions.")
    target.write("It is recommended that a copy of the nimbix_creds.json file be stored in the /data directory and copied to the appropriate location in the home directory ")
    target.write("at the start of each development session.\n")
    target.write("* Launch the application\n")
    target.write("```\n")
    target.write("make check\n")
    target.write("```\n")
    target.write("***Launch the application from a remote system outside of the Nimbix environment***\n")
    target.write("* Follow the instructions in [Nimbix Application Submission README][]\n\n")
    target.write("* Use the following command to launch the application from the users terminal (on a system outside of the Nimbix environment)\n")
    target.write("```\n")
    dirName = os.getcwd()
    dirNameList = list(dirName.split("/"))
    dirNameIndex = dirNameList.index("apps")
    diff = len(dirNameList) - dirNameIndex - 1
    while diff > 0:
	    target.write("../")
	    diff -= 1 
    target.write("utility/nimbix/nimbix-run.py -- ")
    if not "cmd_args" in data:
        target.write('./' + data["host_exe"])
    else:
        target.write('./' + data["host_exe"])
        args = data["cmd_args"].split(" ")
        for arg in args[0:]:
            target.write(" ")
            arg = arg.replace('BUILD', './xclbin')
            arg = arg.replace('PROJECT', '.')
            arg = arg.replace('.xclbin', '.<emulation target>.<device name>.xclbin')
            target.write(arg)
    target.write("\n")
    target.write("```\n\n")
    target.write("***Copy the application files from the Developer to Runtime instances on Nimbix***\n")
    target.write("* Copy the application *.exe file and xclbin directory to the /data directory\n")
    target.write("* Launch the application using the Nimbix web interface as described in [Nimbix Getting Started Guide][]\n")
    target.write("* Make sure that the application launch options in the Nimbix web interface reflect the applications command line syntax\n")
    target.write("```\n")
    if not "cmd_args" in data:
        target.write('./' + data["host_exe"])
    else:
        target.write('./' + data["host_exe"])
        args = data["cmd_args"].split(" ")
        for arg in args[0:]:
            target.write(" ")
            arg = arg.replace('BUILD', './xclbin')
            arg = arg.replace('PROJECT', '.')
            arg = arg.replace('.xclbin', '.<emulation target>.<device name>.xclbin')
            target.write(arg)
    target.write("\n")
    target.write("```\n")
    return

# Get the argument from the description
script, desc_file = argv

# load the description file
print "Scout README File Genarator"
desc = open(desc_file,'r')

# load the json data from the file
data = json.load(desc)
desc.close()

assert("OpenCL" in data['runtime'])

print "Generating the Setup details file for %s" %data["example"]
target = open("setup.md", "w+")
header(target,data)
requirements(target,data)
hierarchy(target)
commandargs(target,data)


if "match_readme" in data and data["match_readme"] == "false":
    print "ERROR:: README Manually Edited:: README Generator Failed\n"
else:
    print "Generating the README for %s" % data["example"]
    target = open("README.md","w")
    overview(target,data)

target.close
