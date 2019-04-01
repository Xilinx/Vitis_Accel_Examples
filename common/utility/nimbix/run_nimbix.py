#!/usr/bin/env python
import sys
import os
import nimbix_run

sys.dont_write_bytecode = True

# Arguments
executable = sys.argv[1:-1]
# Device
device = sys.argv[-1]
if "kcu1500" in device:
	node = "nx4b"
	name = "xilinx-sdx_2018_2"
elif "vcu1525" in device:
	node = "nx5b"
	name = "xilinx-sdx_2018_2"
elif "u200_xdma_201820" in device:
	node = "nx5u"
	name = "xilinx-sdx_2018_2_xdf"
elif "u250_xdma_201820" in device:
	node = "nx6u"
	name = "xilinx-sdx_2018_2_xdf"
elif "u200_xdma_201830" in device:
	node = "nx5u_xdma_201830_1"
	name = "xilinx-sdx_dev_2018_3"
elif "u250_xdma_201830" in device:
	node = "nx6u_xdma_201830_1"
	name = "xilinx-sdx_dev_2018_3"
else:
	exit("Error: DEVICE not supported. Please select the DEVICE properly and rerun.")
# Join Command Line Arguments
string = ' '.join(executable)
# Get the Present Script directory
pwd = sys.path[0]
# Function Call for Nimbix Cloud Execution
os.system(pwd + "/nimbix_run.py " + string + " --type " + node + " --nae " + name)
