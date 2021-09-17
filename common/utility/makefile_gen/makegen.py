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
    target.write("TARGET := hw\n")
    target.write("HOST_ARCH := x86\n")
    target.write("SYSROOT := \n")
    target.write("\n")
    target.write("include ./utils.mk\n")
    target.write("\n")
    target.write("XSA := \n")
    target.write("ifneq ($(DEVICE), )\n")
    target.write("XSA := $(call device2xsa, $(DEVICE))\n")
    target.write("endif\n")
    target.write("TEMP_DIR := ./_x.$(TARGET).$(XSA)\n")
    target.write("BUILD_DIR := ./build_dir.$(TARGET).$(XSA)\n")
    target.write("\n")

    target.write("# SoC variables\n")
    target.write("RUN_APP_SCRIPT = ./run_app.sh\n")
    target.write("PACKAGE_OUT = ./package.$(TARGET)\n")
    target.write("\n")
    target.write("LAUNCH_EMULATOR = $(PACKAGE_OUT)/launch_$(TARGET).sh\n")
    target.write("RESULT_STRING = TEST PASSED\n")
    target.write("\n")

    target.write("VPP := v++")
    target.write("\nVPP_PFLAGS := ")
    if "launch" in data:
        target.write("\n")
        target.write("CMD_ARGS =")
        cmd_args = data["launch"][0]["cmd_args"].split(" ")
        for cmdargs in cmd_args[0:]:
            target.write(" ")
            cmdargs = cmdargs.replace('BUILD', '$(BUILD_DIR)')
            cmdargs = cmdargs.replace('REPO_DIR','$(XF_PROJ_ROOT)')
            cmdargs = cmdargs.replace('PROJECT', '.')
            target.write(cmdargs)
    target.write("\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("SDCARD := ")
        target.write("sd_card\n")
    target.write("\n")
    target.write("include $(XF_PROJ_ROOT)/common/includes/opencl/opencl.mk\n")
    if "config_make" in data:
        target.write("include ")
        target.write(data["config_make"])
        target.write("\n\n")    
    target.write("CXXFLAGS += $(opencl_CXXFLAGS) -Wall -O0 -g -std=c++1y\n")
    target.write("LDFLAGS += $(opencl_LDFLAGS)\n")
    target.write("\n")

    blacklist = [board for board in data.get("platform_blacklist", [])]
    forbid_others = False
    if blacklist:
        for board in blacklist:
            if board != "others":
                target.write("ifeq ($(findstring ")
                target.write(board)
                target.write(", $(DEVICE)), ")
                target.write(board)
                target.write(")\n")
                target.write("$(error [ERROR]: This example is not supported for $(DEVICE).)\n")
                target.write("endif\n")
            else:
                forbid_others = True
        target.write("\n")
    whitelist = [board for board in data.get("platform_whitelist", [])]
    if whitelist:
        for board in whitelist:
            target.write("ifneq ($(findstring ")
            target.write(board)
            target.write(", $(DEVICE)), ")
            target.write(board)
            target.write(")\n")
        if forbid_others:
            target.write("$(error [ERROR]: This example is not supported for $(DEVICE).)\n")
        else:
            target.write("$(warning [WARNING]: This example has not been tested for $(DEVICE). It may or may not work.)\n")
        for board in whitelist:
            target.write("endif\n")
        target.write("\n")
    return

def add_host_flags(target, data):
    target.write("############################## Setting up Host Variables ##############################\n")
    target.write("#Include Required Host Source Files\n")
    

    if "host" in data:
        if "compiler" in data["host"]:
            if "includepaths" in data["host"]["compiler"]:
                for path in data["host"]["compiler"]["includepaths"]:
                    path = path.replace('BUILD', '$(BUILD_DIR)')
                    path = path.replace('PROJECT', '.')
                    path = path.replace('REPO_DIR','$(XF_PROJ_ROOT)')
                    target.write("CXXFLAGS += -I" + path + "\n")

    target.write("HOST_SRCS += ")
    source_flag = 0
    if "sources" in data["host"]["compiler"]:
        for src in data["host"]["compiler"]["sources"]:
            src = src.replace('PROJECT', '.')
            src = src.replace('REPO_DIR','$(XF_PROJ_ROOT)')
            target.write(src + " ")
            source_flag+=1
    if not source_flag:
        target.write("src/host.cpp\n")
    target.write("\n")
    target.write("# Host compiler global settings\n")  	
    target.write("CXXFLAGS += ")
    target.write("-fmessage-length=0")
        
    if "compiler" in data["host"]:
        if "options" in data["host"]["compiler"]:
            option = data["host"]["compiler"]["options"].split(" ")
            for opt in option[0:]:
                target.write(" ")	
                target.write(opt)
    target.write("\n")	
    target.write("LDFLAGS += ")
    target.write("-lrt -lstdc++ ")
    if "linker" in data["host"]:
        if "libraries" in data["host"]["linker"]:
            target.write("\nLDFLAGS +=")
            for library in data["host"]["linker"]["libraries"]:
                target.write(" -l")	
                target.write(library)
    if "linker" in data["host"]:
        if "options" in data["host"]["linker"]:
            target.write("\nLDFLAGS +=")
            option = data["host"]["linker"]["options"].split(" ")
            for opt in option[0:]:
                target.write(" ")	
                target.write(opt)
    target.write("\n\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):		
        target.write("ifneq ($(HOST_ARCH), x86)\n")
        target.write("\tLDFLAGS += --sysroot=$(SYSROOT)\n")
        target.write("endif\n\n")

    return

def add_kernel_flags(target, data):
    target.write("############################## Setting up Kernel Variables ##############################\n")
    target.write("# Kernel compiler global settings\n")

    if "v++" in data:
        target.write("VPP_FLAGS += \n")
    target.write("VPP_FLAGS += ")
    target.write("-t $(TARGET) --platform $(DEVICE) --save-temps \n")   
    target.write("ifneq ($(TARGET), hw)\n")
    target.write("\tVPP_FLAGS += -g\n")
    target.write("endif\n")
    if "containers" in data:
        for con in data["containers"]:
            for acc in con["accelerators"]:
                if "max_memory_ports" in acc:
                    target.write("VPP_FLAGS += ")
                    target.write(" --max_memory_ports ")
                    target.write(acc["name"])
                    target.write("\n")

    if "containers" in data:
        for con in data["containers"]:
            for acc in con["accelerators"]:
                if "clflags" in acc:
                    target.write("VPP_FLAGS_"+acc["name"]+" += ")
                    flags = acc["clflags"].split(" ")
                    for flg in flags[0:]:
                        target.write(" ")
                        flg = flg.replace('PROJECT', '.')
                        target.write(flg)
                    target.write("\n")
    
    if "compiler" in data["host"]:
        if "symbols" in data["host"]["compiler"]:
            target.write("\nCXXFLAGS +=")
            for sym in data["host"]["compiler"]["symbols"]:
                target.write(" ")
                target.write("-D")
                target.write(sym)
        target.write("\n")

    if "containers" in data:
        for con in data["containers"]:
            if  "ldclflags" in con:
                target.write("\n")
                target.write("# Kernel linker flags\n")
                target.write("VPP_LDFLAGS_"+con["name"]+" +=") 
                ldclflags = con["ldclflags"].split(" ")
                for flg in ldclflags[0:]:
                    target.write(" ")
                    flg = flg.replace('PROJECT', '.')
                    target.write(flg)
            target.write("\n")

    target.write("\n")
    target.write("EXECUTABLE = ./")
    if "host_exe" in data["host"]:
        target.write(data["host"]["host_exe"])    
    else: 
        target.write("host")
    target.write("\n")

    target.write("EMCONFIG_DIR = $(TEMP_DIR)\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("EMU_DIR = $(SDCARD)/data/emulation\n")
    target.write("\n")

    if "v++" in data:
        if "compiler" in data["v++"]:
            if "includepaths" in data["v++"]["compiler"]:
                for path in data["v++"]["compiler"]["includepaths"]:
                    path = path.replace('BUILD', '$(BUILD_DIR)')
                    path = path.replace('PROJECT', '.')
                    path = path.replace('REPO_DIR','$(XF_PROJ_ROOT)')
                    target.write("VPP_FLAGS += -I" + path + "\n")
                target.write("\n")

    if "v++" in data:
        if "compiler" in data["v++"]:
            if "clflags" in data["v++"]["compiler"]:
                clflags = data["v++"]["compiler"]["clflags"]
                target.write("VPP_FLAGS +=")
                for path in clflags:
                    path = path.replace('BUILD', '$(BUILD_DIR)')
                    path = path.replace('PROJECT', '.')
                    path = path.replace('REPO_DIR','$(XF_PROJ_ROOT)')
                    target.write(" " + path)
                target.write("\n\n")

    if "v++" in data:
        if "compiler" in data["v++"]:
            if "symbols" in data["v++"]["compiler"]:
                target.write("VPP_FLAGS += ")
                for symb in data["v++"]["compiler"]["symbols"]:
                    target.write("-D" + symb + " ")
                target.write("\n\n")

    return

def add_containers(target, data):
    target.write("############################## Declaring Binary Containers ##############################\n")

    if "containers" in data:
        for con in data["containers"]:
            target.write("BINARY_CONTAINERS += $(BUILD_DIR)/")
            target.write(con["name"])
            target.write(".xclbin\n")
            if "accelerators" in con:
                for acc in con["accelerators"]:
                    target.write("BINARY_CONTAINER_")
                    target.write(con["name"])
                    if "kernel_type" in acc and acc["kernel_type"] == "SystemC":
                            target.write("_OBJS += ")
                    else:
                        target.write("_OBJS += $(TEMP_DIR)/") 
                    target.write(acc["name"])
                    target.write(".xo\n")       	
    target.write("\n")

def building_kernel(target, data):
    if "containers" in data:
        target.write("############################## Setting Rules for Binary Containers (Building Kernels) ##############################\n")
        for con in data["containers"]:
            if "accelerators" in con:
                for acc in con["accelerators"]:
                    target.write("$(TEMP_DIR)/")
                    target.write(acc["name"])
                    target.write(".xo: ")
                    location = acc["location"]
                    location = location.replace('PROJECT', '.')
                    target.write(location)
                    target.write("\n")
                    target.write("\tmkdir -p $(TEMP_DIR)\n")
                    target.write("\t$(VPP) ")
                    target.write("$(VPP_FLAGS) ")
                    if "clflags" in acc:
                        target.write("$(VPP_FLAGS_"+acc["name"]+") ")
                    target.write("-c -k ")
                    target.write(acc["name"])
                    target.write(" --temp_dir ")
                    target.write("$(TEMP_DIR) ")
                    target.write(" -I'$(<D)'")
                    target.write(" -o'$@' '$<'\n")
        for con in data["containers"]:
            target.write("$(BUILD_DIR)/")
            target.write(con["name"])
            target.write(".xclbin:")
            target.write(" $(BINARY_CONTAINER_")
            target.write(con["name"])
            target.write("_OBJS)\n")
            target.write("\tmkdir -p $(BUILD_DIR)\n")
            if not ("platform_type" in data and data["platform_type"] == "pcie"):
                target.write("ifeq ($(HOST_ARCH), x86)\n")
            target.write("\t$(VPP) $(VPP_FLAGS) ")
            target.write("-l $(VPP_LDFLAGS) --temp_dir ")
            target.write("$(TEMP_DIR)")

            if "ldclflags" in con:
                target.write(" $(VPP_LDFLAGS_"+con["name"]+")")
            target.write(" -o'$(BUILD_DIR)/" + con["name"] + ".link.xclbin' $(+)\n")

            target.write("\t$(VPP) -p $(BUILD_DIR)/" + con["name"] + ".link.xclbin -t $(TARGET) --platform $(DEVICE) ")
            target.write("--package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/" + con["name"] + ".xclbin\n")
            if not ("platform_type" in data and data["platform_type"] == "pcie"):
                target.write("else\n")
                target.write("\t$(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) --temp_dir $(TEMP_DIR)")
                if "ldclflags" in con:
                    target.write(" $(VPP_LDFLAGS_"+con["name"]+")")
                target.write(" -o'$(BUILD_DIR)/" + con["name"] + ".xclbin' $(+)\n")
                target.write("endif\n")
    target.write("\n")
    return

def building_kernel_rtl(target, data):
    target.write("# Building kernel\n")
    if "containers" in data:
        for con in data["containers"]:
            target.write("$(BUILD_DIR)/")
            target.write(con["name"])
            target.write(".xclbin:")
            target.write(" $(BINARY_CONTAINER_")
            target.write(con["name"])
            target.write("_OBJS)\n")
            target.write("\tmkdir -p $(BUILD_DIR)\n")
            target.write("ifeq ($(HOST_ARCH), x86)\n")
            target.write("\t$(VPP) $(VPP_FLAGS) ")
            target.write("-l $(VPP_LDFLAGS) --temp_dir ")
            target.write("$(TEMP_DIR)")

            if "ldclflags" in con:
                target.write(" $(VPP_LDFLAGS_"+con["name"]+")")
            target.write(" -o'$(BUILD_DIR)/" + con["name"] + ".link.xclbin' $(+)\n")

            target.write("\t$(VPP) -p $(BUILD_DIR)/" + con["name"] + ".link.xclbin -t $(TARGET) --platform $(DEVICE) ")
            target.write("--package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/" + con["name"] + ".xclbin\n")
            target.write("else\n")
            target.write("\t$(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) --temp_dir $(TEMP_DIR)")
            if "ldclflags" in con:
                target.write(" $(VPP_LDFLAGS_"+con["name"]+")")
            target.write(" -o'$(BUILD_DIR)/" + con["name"] + ".xclbin' $(+)\n")
            target.write("endif\n")
    return

def building_kernel_systemc(target, data):
    if "containers" in data:
        target.write("############################## Setting Rules for Binary Containers (Building Kernels) ##############################\n")
        for con in data["containers"]:
            if "accelerators" in con:
                for acc in con["accelerators"]:
                    target.write(acc["name"])
                    target.write(".xo: ")
                    location = acc["location"]
                    location = location.replace('PROJECT', '.')
                    target.write(location)
                    target.write("\n")
                    target.write("\tmkdir -p $(TEMP_DIR)\n")
                    target.write("\tcreate_sc_xo ")
                    target.write(location)
                    target.write("\n")
        for con in data["containers"]:
            target.write("$(BUILD_DIR)/")
            target.write(con["name"])
            target.write(".xclbin:")
            target.write(" $(BINARY_CONTAINER_")
            target.write(con["name"])
            target.write("_OBJS)\n")
            target.write("\tmkdir -p $(BUILD_DIR)\n")
            target.write("ifeq ($(HOST_ARCH), x86)\n")
            target.write("\t$(VPP) $(VPP_FLAGS) ")
            target.write("-l $(VPP_LDFLAGS) --temp_dir ")
            target.write("$(TEMP_DIR)")

            if "ldclflags" in con:
                target.write(" $(VPP_LDFLAGS_"+con["name"]+")")
            target.write(" -o'$(BUILD_DIR)/" + con["name"] + ".link.xclbin' $(+)\n")

            target.write("\t$(VPP) -p $(BUILD_DIR)/" + con["name"] + ".link.xclbin -t $(TARGET) --platform $(DEVICE) ")
            target.write("--package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/" + con["name"] + ".xclbin\n")
            target.write("else\n")
            target.write("\t$(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) --temp_dir $(TEMP_DIR)")
            if "ldclflags" in con:
                target.write(" $(VPP_LDFLAGS_"+con["name"]+")")
            target.write(" -o'$(BUILD_DIR)/" + con["name"] + ".xclbin' $(+)\n")
            target.write("endif\n")
    target.write("\n")
    return

def building_host(target, data):
    target.write("############################## Setting Rules for Host (Building Host Executable) ##############################\n")

    target.write("$(EXECUTABLE): $(HOST_SRCS) | check-xrt\n")
    target.write("\t	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)\n")
    target.write("\n")
    target.write("emconfig:$(EMCONFIG_DIR)/emconfig.json\n")
    target.write("$(EMCONFIG_DIR)/emconfig.json:\n")
    target.write("\temconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)")
    if "num_devices" in data:
        target.write(" --nd ")
        target.write(data["num_devices"])
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
                    target.write("opencl_summary=true\n")
                    target.write("opencl_device_counter=true\n")
            else:
                target.write("opencl_summary=true\n")
                target.write("opencl_device_counter=true\n")
        else:
            target.write("opencl_summary=true\n")
            target.write("opencl_device_counter=true\n")
    return

def mk_clean(target, data):
    target.write("############################## Cleaning Rules ##############################\n")

    target.write("# Cleaning stuff\n")
    target.write("clean:\n")
    target.write("\t-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} \n")
    target.write("\t-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv \n")
    target.write("\t-$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb\n")
    target.write("\n")

    target.write("cleanall: clean\n")
    target.write("\t-$(RMDIR) build_dir*")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write(" sd_card*")
    target.write("\n")
    target.write("\t-$(RMDIR) package.*\n")
    target.write("\t-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin\n")
    if "output_files" in data:         
        target.write("\t-$(RMDIR) ")
        for files in data["output_files"]:
            target.write("./")       
            target.write(files)
            target.write(" ")       
    target.write("\n")

    return

def mk_build_all(target, data):
    target.write("############################## Setting Targets ##############################\n")
    target.write("CP = cp -rf\n")
    target.write("\n")

    target.write(".PHONY: all clean cleanall docs emconfig\n")
    target.write("all: check-devices $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write(" sd_card")
    target.write("\n\n")
    
    target.write(".PHONY: host\n")
    target.write("host: $(EXECUTABLE)\n")
    target.write("\n")
    
    target.write(".PHONY: build\n")
    target.write("build: check-vitis $(BINARY_CONTAINERS)\n")
    target.write("\n")

    target.write(".PHONY: xclbin\n")
    target.write("xclbin: build\n")
    target.write("\n")
    
    rtl_counter = 0
    systemc_counter = 0
    if "containers" in data:
        for con in data["containers"]:
            if "accelerators" in con:
                for acc in con["accelerators"]:
                    if "kernel_type" in acc:
                    	if acc["kernel_type"] == "RTL":
     	                    rtl_counter = 1
                        elif acc["kernel_type"] == "SystemC":
                            systemc_counter = 1
                        
    if rtl_counter == 1:
        building_kernel_rtl(target, data)
    elif systemc_counter == 1:
        building_kernel_systemc(target, data)
    else:
    	building_kernel(target, data)
    building_host(target, data)
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
    target.write("############################## Setting Essential Checks and Running Rules ##############################\n")

    target.write("run: all\n")
    target.write("ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("ifeq ($(HOST_ARCH), x86)\n")
    target.write("\t$(CP) $(EMCONFIG_DIR)/emconfig.json .\n") 
    target.write("\tXCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE)")
    
    if "launch" in data:	
        if "cmd_args" in data["launch"][0]:
            target.write(" $(CMD_ARGS)")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("\nelse\n")
        target.write("\t$(LAUNCH_EMULATOR) -run-app $(RUN_APP_SCRIPT) | tee run_app.log; exit $${PIPESTATUS[0]}")
        if "containers" not in data:
            target.write(" -ps-only")
    target.write("\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("endif\n")
    target.write("else\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("ifeq ($(HOST_ARCH), x86)\n")
    target.write("\t$(EXECUTABLE)")
	
    if "launch" in data:
        if "cmd_args" in data["launch"][0]:
            target.write(" $(CMD_ARGS)")
    if "post_launch" in data:
        for post_launch in data["post_launch"]:
            if "launch_cmd" in post_launch:
                target.write("\n")
                args = post_launch["launch_cmd"]
                args = args.replace('BUILD', '$(BUILD_DIR)')
                args = args.replace('REPO_DIR','$(XF_PROJ_ROOT)')
                args = args.replace('HOST_EXE', '$(EXE_FILE)')
                target.write("\t" + args)
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("\nendif")
    target.write("\nendif\n")
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

    target.write("\n\n")
    
    target.write(".PHONY: test\n")
    target.write("test: $(EXECUTABLE)\n")
    target.write("ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("ifeq ($(HOST_ARCH), x86)\n")
    target.write("\tXCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE)")
    
    if "launch" in data:	
        if "cmd_args" in data["launch"][0]:
            target.write(" $(CMD_ARGS)")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("\nelse\n")
        target.write("\t$(LAUNCH_EMULATOR) -run-app $(RUN_APP_SCRIPT) | tee run_app.log; exit $${PIPESTATUS[0]}")
        if "containers" not in data:
            target.write(" -ps-only")
    target.write("\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("endif\n")
    target.write("else\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("ifeq ($(HOST_ARCH), x86)\n")
    target.write("\t$(EXECUTABLE)")
	
    if "launch" in data:
        if "cmd_args" in data["launch"][0]:
            target.write(" $(CMD_ARGS)")
    if "post_launch" in data:
        for post_launch in data["post_launch"]:
            if "launch_cmd" in post_launch:
                target.write("\n")
                args = post_launch["launch_cmd"]
                args = args.replace('BUILD', '$(BUILD_DIR)')
                args = args.replace('REPO_DIR','$(XF_PROJ_ROOT)')
                args = args.replace('HOST_EXE', '$(EXE_FILE)')
                target.write("\t" + args)
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("\nelse\n")
        target.write("\t$(ECHO) \"Please copy the content of sd_card folder and data to an SD Card and run on the board\"")
        target.write("\nendif")
    target.write("\nendif\n")
    if "targets" in data:
        target.write("ifneq ($(TARGET),$(findstring $(TARGET),")
        args = data["targets"]
        for arg in args:
            target.write(" ")
            target.write(arg)
        target.write("))\n")
        target.write("$(warning WARNING:Application supports only")
        for arg in args:
            target.write(" ")
            target.write(arg)
        target.write(" TARGET. Please use the target for running the application)\n")
        target.write("endif\n")
        target.write("\n")
    target.write("\n\n")

def mk_sdcard(target, data):
    target.write("############################## Preparing sdcard ##############################\n")
    target.write("sd_card: $(BINARY_CONTAINERS) $(EXECUTABLE) gen_run_app\n")
    extra_file_list = []
    if "launch" in data:	
        if "cmd_args" in data["launch"][0]:
            args = data["launch"][0]["cmd_args"].split(" ")    
            for arg in args:
                if "xclbin" not in arg:
                    arg = arg.replace('BUILD', '$(BUILD_DIR)')
                    arg = arg.replace('REPO_DIR','$(XF_PROJ_ROOT)')
                    arg = arg.replace('PROJECT', '.')
                    extra_file_list.append(arg)  
    target.write("ifneq ($(HOST_ARCH), x86)\n")
    if "containers" in data:
        for con in data["containers"]:
            target.write("\t$(VPP) $(VPP_PFLAGS) -p $(BUILD_DIR)/")
            target.write(con["name"])
            target.write(".xclbin -t $(TARGET) --platform $(DEVICE) ")
            target.write("--package.out_dir $(PACKAGE_OUT) --package.rootfs $(EDGE_COMMON_SW)/rootfs.ext4 --package.sd_file $(SD_IMAGE_FILE) --package.sd_file xrt.ini --package.sd_file $(RUN_APP_SCRIPT) --package.sd_file $(EXECUTABLE)")
            for extra_filename in extra_file_list:
                if ('-' not in extra_filename):
                    target.write(" --package.sd_file ")
                    target.write(extra_filename)
            target.write(" -o ")
            target.write(con["name"])
            target.write(".xclbin\n")
    target.write("endif\n")
    target.write("\n")

def run_nimbix(target, data):
    target.write("run_nimbix: all\n")
    if "launch" in data:
        if "cmd_args" in data["launch"][0]:
    	    target.write("\t$(COMMON_REPO)/common/utility/nimbix/run_nimbix.py $(EXECUTABLE) $(CMD_ARGS) $(XSA)\n\n")
    else:
        target.write("\t$(COMMON_REPO)/common/utility/nimbix/run_nimbix.py $(EXECUTABLE) $(XSA)\n\n")	
    
def aws_build(target):
    target.write("aws_build: check-aws_repo $(BINARY_CONTAINERS)\n")
    target.write("\t$(COMMON_REPO)/common/utility/aws/run_aws.py $(BINARY_CONTAINERS)\n\n")

def mk_help(target):
    target.write("\n############################## Help Section ##############################\n")   

    target.write(".PHONY: help\n")
    target.write("\n")
    target.write("help::\n")
    target.write("\t$(ECHO) \"Makefile Usage:\"\n")
    target.write("\t$(ECHO) \"  make all TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform>");
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
		target.write(" HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>")
    target.write("\"\n")
    target.write("\t$(ECHO) \"      Command to generate the design for specified Target and Shell.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make clean \"\n");
    target.write("\t$(ECHO) \"      Command to remove the generated non-hardware files.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make cleanall\"\n")
    target.write("\t$(ECHO) \"      Command to remove all the generated files.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make test DEVICE=<FPGA platform>\"\n")    
    target.write("\t$(ECHO) \"      Command to run the application. This is same as 'run' target but does not have any makefile dependency.\"\n")  
    target.write("\t$(ECHO) \"\"\n")

    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("\t$(ECHO) \"  make sd_card TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>\"\n");
        target.write("\t$(ECHO) \"      Command to prepare sd_card files.\"\n")
        target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make run TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform>");
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
		target.write(" HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>")
    target.write("\"\n")
    target.write("\t$(ECHO) \"      Command to run application in emulation.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make build TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform>");
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
		target.write(" HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>")
    target.write("\"\n")
    target.write("\t$(ECHO) \"      Command to build xclbin application.\"\n")
    target.write("\t$(ECHO) \"\"\n")
    target.write("\t$(ECHO) \"  make host");
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
		target.write(" HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>")
    target.write("\"\n")
    target.write("\t$(ECHO) \"      Command to build host application.\"\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
    	target.write("\t$(ECHO) \"  By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells\"\n")
    target.write("\t$(ECHO) \"\"\n")
    #target.write("\t$(ECHO) \"  make run_nimbix DEVICE=<FPGA platform>\"\n");
    #target.write("\t$(ECHO) \"      Command to run application on Nimbix Cloud.\"\n")
    #target.write("\t$(ECHO) \"\"\n")
    #target.write("\t$(ECHO) \"  make aws_build DEVICE=<FPGA platform>\"\n");
    #target.write("\t$(ECHO) \"      Command to build AWS xclbin application on AWS Cloud.\"\n")
    #target.write("\t$(ECHO) \"\"\n")
    target.write("\n")

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
    target.write("B_TEMP = `$(XF_PROJ_ROOT)/common/utility/parse_platform_list.py $(DEVICE)`\n")
    target.write("\n")
    target.write("#Generates debug summary report\n")
    target.write("ifeq ($(DEBUG), yes)\n")
    target.write("VPP_LDFLAGS += --dk list_ports\n")
    target.write("endif\n")
    target.write("\n")

def device2xsa_gen(target):
    target.write("#   device2xsa - create a filesystem friendly name from device name\n")
    target.write("#   $(1) - full name of device\n")
    target.write("device2xsa = $(strip $(patsubst %.xpfm, % , $(shell basename $(DEVICE))))\n")
    target.write("\n")

def util_checks(target):
    target.write("#Setting Platform Path\n")
    target.write("ifeq ($(findstring xpfm, $(DEVICE)), xpfm)\n")
    target.write("\tB_NAME = $(shell dirname $(DEVICE))\n")
    target.write("else\n")
    target.write("\tB_NAME = $(B_TEMP)/$(DEVICE)\n")
    target.write("endif\n")
    target.write("\n")

    target.write("#Checks for XILINX_VITIS\n")
    target.write("check-vitis:\n")
    target.write("ifndef XILINX_VITIS\n")
    target.write("\t$(error XILINX_VITIS variable is not set, please set correctly and rerun)\n")
    target.write("endif\n")
    target.write("\n")

    target.write("#Checks for XILINX_XRT\n")
    target.write("check-xrt:\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("ifeq ($(HOST_ARCH), x86)\n")
    target.write("ifndef XILINX_XRT\n")
    target.write("\t$(error XILINX_XRT variable is not set, please set correctly and rerun)\n")
    target.write("endif\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("else\n")
        target.write("ifndef XILINX_VITIS\n")
        target.write("\t$(error XILINX_VITIS variable is not set, please set correctly and rerun)\n")
        target.write("endif\n")
        target.write("endif\n")
    target.write("\n")

    target.write("#Checks for Correct architecture\n")
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("ifneq ($(HOST_ARCH), $(filter $(HOST_ARCH),aarch64 aarch32 x86))\n")
        target.write("$(error HOST_ARCH variable not set, please set correctly and rerun)\n") 
    else:
        target.write("ifneq ($(HOST_ARCH), $(filter $(HOST_ARCH),x86))\n")
        target.write("$(error HOST_ARCH variable not set correctly, supported HOST_ARCH is x86 only)\n") 
    target.write("endif\n")
    target.write("\n")

    
    if "host" in data:
        if "linker" in data["host"]:
            if "libraries" in data["host"]["linker"]:
                if "xrt_coreutil" in data["host"]["linker"]["libraries"] and "uuid" in data["host"]["linker"]["libraries"]:                    
                    target.write("USR_LDFLAGS :=\n")
                    target.write("LDFLAGS += $(USR_LDFLAGS)\n")
                    target.write("\n")

    target.write("#Setting CXX\n")
    target.write("CXX := g++\n")
    target.write("\n")
    
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("#Checks for EDGE_COMMON_SW\n")
        target.write("ifneq ($(HOST_ARCH), x86)\n")
        target.write("ifndef EDGE_COMMON_SW\n")
        target.write("$(error EDGE_COMMON_SW variable is not set, please set correctly and rerun)\n")
        target.write("endif\n")
        target.write("ifeq ($(HOST_ARCH), aarch64)\n")
        target.write("SYSROOT := $(EDGE_COMMON_SW)/sysroots/cortexa72-cortexa53-xilinx-linux\n")
        target.write("SD_IMAGE_FILE := $(EDGE_COMMON_SW)/Image\n")
        target.write("CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++\n")
        target.write("else ifeq ($(HOST_ARCH), aarch32)\n")
        target.write("SYSROOT := $(EDGE_COMMON_SW)/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/\n")
        target.write("SD_IMAGE_FILE := $(EDGE_COMMON_SW)/uImage\n")
        target.write("CXX := $(XILINX_VITIS)/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-g++\n")
        target.write("endif\n")
        target.write("endif\n\n")

    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        target.write("gen_run_app:\n")
        target.write("ifneq ($(HOST_ARCH), x86)\n")
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
        target.write("endif\n")
    
    target.write("check-devices:\n")
    target.write("ifndef DEVICE\n")
    target.write("\t$(error DEVICE not set. Please set the DEVICE properly and rerun. Run \"make help\" for more details.)\n")
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
    mk_help(target)
    create_params(target,data)
    add_host_flags(target, data)
    add_kernel_flags(target, data)
    add_containers(target, data)
    mk_build_all(target, data)
    mk_run(target, data)
    if not ("platform_type" in data and data["platform_type"] == "pcie"):
        mk_sdcard(target, data)
    mk_clean(target,data)
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
