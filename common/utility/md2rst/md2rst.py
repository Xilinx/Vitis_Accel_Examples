#!/usr/bin/env /tools/cpkg/.packages/x86_64/RHEL7.2/python/3.7.1/bin/python3.7  
import json
import os
import subprocess
from os import listdir
from sys import argv
from collections import OrderedDict
from itertools import islice

XSA = 'xilinx_u200_qdma'
VERSION = 'Vitis 2019.2'

DEVICES = {
     'zcu102_base': {
       'version': '201920_1',
       'name': 'Xilinx UltraScale+ MPSoC ZCU102'
    },
     'zcu104_base': {
       'version': '201920_1',
       'name': 'Xilinx UltraScale+ MPSoC ZCU104'
    },
     'zc706_base': {
       'version': '201920_1',
       'name': 'Xilinx Zynq-7000 SoC ZC706'
    },
     'zc702_base': {
       'version': '201920_1',
       'name': 'Xilinx Zynq-7000 SoC ZC702'
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
    'xilinx_u50_xdma': {
       'version': '201920_1',
       'name': 'Xilinx Alveo U50'
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
    target.write("<a href=\"./")
    dirName = os.getcwd()
    dirNameList = list(dirName.split("/"))
    dirNameIndex = dirNameList.index("Vitis_Accel_Examples")
    diff = len(dirNameList) - dirNameIndex - 1
    while diff > 0:
        target.write("../")
        diff -= 1
    for locs in range(dirNameIndex + 1,len(dirNameList)):
        target.write(dirNameList[locs])
        target.write("/")
    target.write("\">")
    title = data["name"]
    title = title.replace("(C)", "")
    title = title.replace("(CL)", "")
    title = title.replace("(RTL)", "")
    title = title.replace("(HLS C/C++ Kernel)", "")
    target.write(title)
    target.write("</a>")
    target.write("\n")
    target.write("=====================================================\n\n")
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
    listfiles = os.listdir('./')
    if 'details.md' in listfiles:
        with open('details.md', 'r') as fin:
            for i, x in enumerate(fin):
                if 2 <= i :
                    target.write(x)
    target.write("\n")
    return

def requirements(target,data):
    target.write("## SUPPORTED SHELLS\n")
    target.write("SHELL | Board             | Software Version\n")
    target.write("---------|-------------------|-----------------\n")

    boards = []
    if 'device' in data:
        board = data['device']
        boards = [word for word in DEVICES if word in board]
    else:
        for board in DEVICES:
                nboard_flag=0
                if 'ndevice' in data:	
                        for nboard in data['ndevice']:
                                if nboard in board:
                       	       	        nboard_flag=1
                if nboard_flag == 0:
                        boards.append(board)			

    boards.sort()
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
    output = str(output).split("\'")[1]
    output = output.split("\\n")
    flag = 0
    for lines in output:
        if flag is 1:
            target.write("\n")
        target.write(lines)
        flag = 1
    target.write("```\n")
    target.write("\n")
    return

def commandargs(target,data):
    target.write("##  COMMAND LINE ARGUMENTS\n")
    target.write("Once the environment has been configured, the application can be executed by\n")
    target.write("```\n")
    if "launch" in data:
        if not "cmd_args" in data["launch"][0]:
            target.write('./' + data["host"]["host_exe"])
        else:
            target.write('./' + data["host"]["host_exe"])
            args = data["launch"][0]["cmd_args"].split(" ")
            for arg in args[0:]:
                target.write(" ")
                arg = arg.replace('BUILD/', '<')
                arg = arg.replace('PROJECT', '.')
                arg = arg.replace('.xclbin', ' XCLBIN>') 
                target.write(arg)
    else:
        target.write('./' + data["host"]["host_exe"])
    target.write("\n```\n")
    target.write("\n")

# Get the argument from the description
script, desc_file = argv

# load the description file
print ("Vitis MD2RST File Genarator")
desc = open(desc_file,'r')

# load the json data from the file
data = json.load(desc)
desc.close()

assert("OpenCL" in data['runtime'])

if "match_readme" in data and data["match_readme"] == "false":
    print ("ERROR:: README Manually Edited:: README Generator Failed\n")
else:
    print ("Generating the README for %s" % data["name"])
    target = open("D_README.md","w")
    overview(target,data)
    requirements(target,data)
    hierarchy(target)
    commandargs(target,data)

target.close
