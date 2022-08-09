#!/usr/bin/env python
from sys import argv
import json
import glob
import os
import re
import subprocess

#ini flags
config_file = 0

def mk_copyright(target):
    target.write("""#
# Copyright 2019-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# makefile-generator v1.0.3
#
""")
    return

def create_params(target,data): 
    target.write("# Points to top directory of Git repository\n")

    target.write("MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))\n")
    refpath = init_cur_dir.replace(cur_dir+"/","")
    target.write("COMMON_REPO ?= ")
    MK_PATH = 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%' + refpath+ '/*}'
    target.write("$(shell bash -c '%s')" %MK_PATH)
    target.write("\n")
    target.write("PWD = $(shell readlink -f .)\n")
    target.write("XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))\n")
    target.write("\n")

    blocklist = [board for board in data.get("platform_blocklist", [])]
    forbid_others = False
    target.write("\n########################## Checking if PLATFORM in allowlist #######################\n")
    if blocklist:
        target.write("PLATFORM_BLOCKLIST += ")        
        for board in blocklist:
          if board != "others":
              target.write(board)
              target.write(" ")
          else:
              forbid_others = True
        target.write("\n")
    allowlist = [board for board in data.get("platform_allowlist", [])]
    if allowlist:
        target.write("PLATFORM_ALLOWLIST += ")
        for board in allowlist:
            target.write(board)
            target.write(" ")
        target.write("\n\n")
    return

def profile_report(target):
    target.write("[Debug]\n")
    if "host" in data:
        if "linker" in data["host"]:
            if "libraries" in data["host"]["linker"]:
                if "xrt_coreutil" in data["host"]["linker"]["libraries"] and "uuid" in data["host"]["linker"]["libraries"]:                    
                    target.write("native_xrt_trace=true\n")
                else:
                    target.write("opencl_trace=true\n")
                    target.write("device_trace=fine\n")
                    target.write("device_counters=true\n")
            else:
                target.write("opencl_trace=true\n")
                target.write("device_trace=fine\n")
                target.write("device_counters=true\n")
        else:
            target.write("opencl_trace=true\n")
            target.write("device_trace=fine\n")
            target.write("device_counters=true\n")
    return

def deprecated_check(target, data):
    target.write("############################## Deprecated Checks and Running Rules ##############################\n")

    target.write("check:\n")
    target.write("\t$(ECHO) \"WARNING: \\\"make check\\\" is a deprecated command. Please use \\\"make run\\\" instead\"\n")
    target.write("\tmake run\n")
    target.write("\n")

    target.write("exe:\n")
    target.write("\t$(ECHO) \"WARNING: \\\"make exe\\\" is a deprecated command. Please use \\\"make host\\\" instead\"\n")
    target.write("\tmake host\n")
    target.write("\n")

def mk_run(target, data):
    if "targets" in data:
        target.write("ifneq ($(TARGET),$(findstring $(TARGET),")
        args = data["targets"]
        for arg in args:
            target.write(" ")
            target.write(arg)
        target.write("))\n")
        target.write("$(error Application supports only")
        for arg in args:
            target.write(" ")
            target.write(arg)
        target.write(" TARGET. Please use the target for running the application)\n")
        target.write("endif\n")
    target.write("PLATFORM ?= xilinx_u250_gen3x16_xdma_3_1_202020_1\n")
    target.write("DEV_ARCH := $(shell platforminfo -p $(PLATFORM) | grep 'FPGA Family' | sed 's/.*://' | sed '/ai_engine/d' | sed 's/^[[:space:]]*//')\n")
    target.write("CPU_TYPE := $(shell platforminfo -p $(PLATFORM) | grep 'CPU Type' | sed 's/.*://' | sed '/ai_engine/d' | sed 's/^[[:space:]]*//')\n")
    target.write("\n")
    
    target.write("ifeq ($(CPU_TYPE), cortex-a9)\n")
    target.write("HOST_ARCH := aarch32\n")
    target.write("else ifneq (,$(findstring cortex-a, $(CPU_TYPE)))\n")
    target.write("HOST_ARCH := aarch64\n")
    target.write("else\n")
    target.write("HOST_ARCH := x86\n")
    target.write("endif\n")
    target.write("\n")

    makefile_lst = ["makegen_us_alveo.py" , "makegen_versal_alveo.py", "makegen_versal_ps.py", "makegen_zynqmp.py", "makegen_zynq7000.py"]

    blocklist = [board for board in data.get("platform_blocklist", [])]
    if ("platform_type" in data and data["platform_type"] == "pcie"):
      makefile_lst.remove("makegen_versal_ps.py")
      makefile_lst.remove("makegen_zynqmp.py")
      makefile_lst.remove("makegen_zynq7000.py")
    if "zc" in blocklist:
      if "makegen_zynqmp.py" in makefile_lst : makefile_lst.remove("makegen_zynqmp.py")
      if "makegen_zynq7000.py" in makefile_lst : makefile_lst.remove("makegen_zynq7000.py")
    if "vck" in blocklist:
      if "makegen_versal_ps.py" in makefile_lst : makefile_lst.remove("makegen_versal_ps.py")
      makefile_lst.remove("makegen_versal_alveo.py")
    if "dma" in blocklist:
      if "makegen_versal_alveo.py" in makefile_lst : makefile_lst.remove("makegen_versal_alveo.py")
      makefile_lst.remove("makegen_us_alveo.py")

    for i in makefile_lst:
      os.system("cd " + os.getcwd())
      os.system(cur_dir + "/common/utility/makefile_gen/" + i + " " + desc_file)
    
    if ("platform_type" in data and data["platform_type"] == "pcie") and ("vck" in blocklist):
      target.write("include makefile_us_alveo.mk\n")
    elif ("platform_type" in data and data["platform_type"] == "pcie"):
      target.write("ifeq ($(DEV_ARCH), versal)\n")
      target.write("include makefile_versal_alveo.mk\n")
      target.write("else\n")
      target.write("include makefile_us_alveo.mk\n")
      target.write("endif\n")
    elif "dma" in blocklist:
      target.write("ifeq ($(DEV_ARCH), zynquplus)\n")
      target.write("include makefile_zynqmp.mk\n")
      target.write("else ifeq ($(DEV_ARCH), zynq)\n")
      target.write("include makefile_zynq7000.mk\n")
      target.write("else ifeq ($(DEV_ARCH), versal)\n")
      target.write("include makefile_versal_ps.mk\n")
      target.write("endif\n")
    else:
      target.write("ifeq ($(DEV_ARCH), zynquplus)\n")
      target.write("ifeq ($(HOST_ARCH), aarch64)\n")
      target.write("include makefile_zynqmp.mk\n")
      target.write("else\n")
      target.write("include makefile_us_alveo.mk\n")
      target.write("endif\n")
      target.write("else ifeq ($(DEV_ARCH), zynq)\n")
      target.write("include makefile_zynq7000.mk\n")
      target.write("else ifeq ($(DEV_ARCH), versal)\n")
      target.write("ifeq ($(HOST_ARCH), x86)\n")
      target.write("include makefile_versal_alveo.mk\n")
      target.write("else\n")
      target.write("include makefile_versal_ps.mk\n")
      target.write("endif\n")
      target.write("else\n")
      target.write("include makefile_us_alveo.mk\n")
      target.write("endif\n")
    
def mk_help(target):
    target.write("\n############################## Help Section ##############################\n")   

    target.write("help:\n")
    target.write("\t$(ECHO) \"Makefile Usage:\"\n")
    target.write("\t$(ECHO) \"  make all TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>");
    target.write(" EDGE_COMMON_SW=<rootfs and kernel image path>")
    target.write("\"\n")
    target.write("\t$(ECHO) \"      Command to generate the design for specified Target and Shell.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make clean \"\n");
    target.write("\t$(ECHO) \"      Command to remove the generated non-hardware files.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make cleanall\"\n")
    target.write("\t$(ECHO) \"      Command to remove all the generated files.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make test PLATFORM=<FPGA platform>\"\n")    
    target.write("\t$(ECHO) \"      Command to run the application. This is same as 'run' target but does not have any makefile dependency.\"\n")  
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make sd_card TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform> EDGE_COMMON_SW=<rootfs and kernel image path>\"\n");
    target.write("\t$(ECHO) \"      Command to prepare sd_card files.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make run TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>");
    target.write(" EDGE_COMMON_SW=<rootfs and kernel image path>")
    target.write("\"\n")
    target.write("\t$(ECHO) \"      Command to run application in emulation.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make build TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>");
    target.write(" EDGE_COMMON_SW=<rootfs and kernel image path>")
    target.write("\"\n")
    target.write("\t$(ECHO) \"      Command to build xclbin application.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make host");
    target.write(" EDGE_COMMON_SW=<rootfs and kernel image path>")
    target.write("\"\n")
    target.write("\t$(ECHO) \"      Command to build host application.\"\n")
    target.write("\t$(ECHO) \"      EDGE_COMMON_SW is required for SoC shells. Please download and use the pre-built image from - \"\n")
    target.write("\t$(ECHO) \"      https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms.html\"\n")
    target.write("\t$(ECHO) \"\"\n")

def report_gen(target, data):
    target.write("#+-------------------------------------------------------------------------------\n")
    target.write("# The following parameters are assigned with default values. These parameters can\n")
    target.write("# be overridden through the make command line\n")
    target.write("#+-------------------------------------------------------------------------------\n")
    target.write("\n")

    if "testinfo" in data and "profile" in data["testinfo"] and data["testinfo"]["profile"] == "no":
        pass
    else:
        target.write("PROFILE := no\n")
        target.write("\n")
        target.write("#Generates profile summary report\n")
        target.write("ifeq ($(PROFILE), yes)\n")
        target.write("VPP_LDFLAGS += --profile.data all:all:all\n")
        target.write("endif\n")
        target.write("\n")
    
    target.write("DEBUG := no\n")
    target.write("\n")
    target.write("#Generates debug summary report\n")
    target.write("ifeq ($(DEBUG), yes)\n")
    target.write("VPP_LDFLAGS += --dk list_ports\n")
    target.write("endif\n")
    target.write("\n")
    target.write("ifneq ($(TARGET), hw)\n")
    target.write("VPP_FLAGS += -g\n")
    target.write("endif\n")
    target.write("\n")
    
    target.write("############################## Setting up Project Variables ##############################\n")      
    target.write("# Points to top directory of Git repository\n")

    target.write("MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))\n")
    refpath = init_cur_dir.replace(cur_dir+"/","")
    target.write("COMMON_REPO ?= ")
    MK_PATH = 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%' + refpath+ '/*}'
    target.write("$(shell bash -c '%s')" %MK_PATH)
    target.write("\n")
    target.write("PWD = $(shell readlink -f .)\n")
    target.write("XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))\n")
    target.write("\n")

def device2xsa_gen(target):
    target.write("#   device2xsa - create a filesystem friendly name from device name\n")
    target.write("#   $(1) - full name of device\n")
    target.write("device2xsa = $(strip $(patsubst %.xpfm, % , $(shell basename $(PLATFORM))))\n")
    target.write("\n")
    target.write("XSA := \n")
    target.write("ifneq ($(PLATFORM), )\n")
    target.write("XSA := $(call device2xsa, $(PLATFORM))\n")
    target.write("endif\n")
    target.write("\n")

def util_checks(target):
    target.write("#Setting PLATFORM \n")
    target.write("ifeq ($(PLATFORM),)\n")
    target.write("ifneq ($(DEVICE),)\n")
    target.write("$(warning WARNING: DEVICE is deprecated in make command. Please use PLATFORM instead)\n")
    target.write("PLATFORM := $(DEVICE)\n")
    target.write("endif\n")
    target.write("endif\n")
    target.write("\n")

    target.write("#Checks for XILINX_VITIS\n")
    target.write("check-vitis:\n")
    target.write("ifndef XILINX_VITIS\n")
    target.write("\t$(error XILINX_VITIS variable is not set, please set correctly using \"source <Vitis_install_path>/Vitis/<Version>/settings64.sh\" and rerun)\n")
    target.write("endif\n")
    target.write("\n")

    target.write("#Checks for XILINX_XRT\n")
    target.write("check-xrt:\n")
    target.write("ifndef XILINX_XRT\n")
    target.write("\t$(error XILINX_XRT variable is not set, please set correctly using \"source /opt/xilinx/xrt/setup.sh\" and rerun)\n")
    target.write("endif\n")
    target.write("\n")

    target.write("check-device:\n")
    target.write("\t@set -eu; \\\n")
    target.write("\tinallowlist=False; \\\n")
    target.write("\tinblocklist=False; \\\n")
    target.write("\tif [ \"$(PLATFORM_ALLOWLIST)\" = \"\" ]; \\\n")
    target.write("\t    then inallowlist=True; \\\n")
    target.write("\tfi; \\\n")
    target.write("\tfor dev in $(PLATFORM_ALLOWLIST); \\\n")
    target.write("\t    do if [[ $$(echo $(PLATFORM) | grep $$dev) != \"\" ]]; \\\n")
    target.write("\t    then inallowlist=True; fi; \\\n")
    target.write("\tdone ;\\\n")
    target.write("\tfor dev in $(PLATFORM_BLOCKLIST); \\\n")
    target.write("\t    do if [[ $$(echo $(PLATFORM) | grep $$dev) != \"\" ]]; \\\n")
    target.write("\t    then inblocklist=True; fi; \\\n")
    target.write("\tdone ;\\\n")
    target.write("\tif [[ $$inblocklist == True ]]; \\\n")
    target.write("\t    then echo \"[ERROR]: This example is not supported for $(PLATFORM).\"; exit 1;\\\n")
    target.write("\tfi; \\\n")
    target.write("\tif [[ $$inallowlist == False ]]; \\\n")
    target.write("\t    then echo \"[Warning]: The platform $(PLATFORM) not in allowlist.\"; \\\n")
    target.write("\tfi;\n")
    target.write("\n")

    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("gen_run_app:\n")
        target.write("\trm -rf run_app.sh\n")
        target.write("\t$(ECHO) 'export LD_LIBRARY_PATH=/mnt:/tmp:$$LD_LIBRARY_PATH' >> run_app.sh\n")
        target.write("\t$(ECHO) 'export PATH=$$PATH:/sbin' >> run_app.sh\n")
        target.write("\t$(ECHO) 'export XILINX_XRT=/usr' >> run_app.sh\n")
        target.write("ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))\n")
        target.write("\t$(ECHO) 'export XILINX_VITIS=$$PWD' >> run_app.sh\n")
        target.write("\t$(ECHO) 'export XCL_EMULATION_MODE=$(TARGET)' >> run_app.sh\n")
        target.write("endif\n")
        target.write("\t$(ECHO) '$(EXECUTABLE)")
        if "launch" in data:	
            if "cmd_args" in data["launch"][0]:
                args = data["launch"][0]["cmd_args"].split(" ")    
                for arg in args:
                    arg_name = arg.split("/") 
                    target.write(" ")
                    target.write(arg_name[-1])
        target.write("' >> run_app.sh\n")
        target.write("\t$(ECHO) 'return_code=$$?' >> run_app.sh\n")
        target.write("\t$(ECHO) 'if [ $$return_code -ne 0 ]; then' >> run_app.sh\n")
        target.write("\t$(ECHO) 'echo \"ERROR: host run failed, RC=$$return_code\"' >> run_app.sh\n")
        target.write("\t$(ECHO) 'fi' >> run_app.sh\n")
        target.write("\t$(ECHO) 'echo \"INFO: host run completed.\"' >> run_app.sh\n")
    
    target.write("check-platform:\n")
    target.write("ifndef PLATFORM\n")
    target.write("\t$(error PLATFORM not set. Please set the PLATFORM properly and rerun. Run \"make help\" for more details.)\n")
    target.write("endif\n")
    target.write("\n")

def clean_util(target):
    target.write("# Cleaning stuff\n")
    target.write("RM = rm -f\n")
    target.write("RMDIR = rm -rf\n")
    target.write("\n")
    target.write("ECHO:= @echo\n")
    target.write("\n")
    
def readme_gen(target):
    target.write("docs: README.rst\n")
    target.write("\n")
    target.write("README.rst: description.json\n")
    target.write("\t$(XF_PROJ_ROOT)/common/utility/readme_gen/readme_gen.py description.json")
    target.write("\n")   

    
def create_mk(target, data):
    mk_copyright(target)
    create_params(target,data)
    mk_run(target, data)
    mk_help(target)
    return 

def create_utils(target, data): 
    report_gen(target, data) 
    util_checks(target)
    device2xsa_gen(target)
    deprecated_check(target, data)
    clean_util(target)
    readme_gen(target)
    return

script, desc_file = argv
desc = open(desc_file, 'r')
data = json.load(desc)
desc.close()

file_name = "LICENSE.txt" # file to be searched
cur_dir = os.getcwd()      # Dir from where search starts can be replaced with any path
init_cur_dir = cur_dir

while True:
    file_list = os.listdir(cur_dir)
    parent_dir = os.path.dirname(cur_dir)
    if file_name in file_list:
        break
    else:
        if cur_dir == parent_dir:         # if dir is root dir
            print ("LICENSE.txt file not found")
            break
        else:
            cur_dir = parent_dir

if "match_ini" in data and data["match_ini"] == "false":
    print("Info:: xrt.ini File Manually Edited:: Auto-file Generator Skipped")
    err = False
else:
    print("Generating xrt.ini file for %s" %data["name"])
    target = open("xrt.ini","w+")
    profile_report(target)

if "match_makefile" in data and data["match_makefile"] == "false":
    print("Info:: Makefile Manually Edited:: AutoMakefile Generator Skipped")
else:
    print("Generating Auto-Makefile for %s" %data["name"])
    target = open("Makefile", "w")
    create_mk(target, data)
    print("Generating utils.mk file for %s" %data["name"])
    target = open("utils.mk", "w+")    
    create_utils(target, data)

if target:
    target.close
