#!/usr/bin/env python
from sys import argv
import json
import argparse
import glob
import os
import re
import sys
import subprocess

def create_cfg_u2(data):    
    target = open("platform_"+filename+".cfg","w+")
    target.write("[connectivity]\n")
    target.write("sp=" + filename + "_1.input:bank0\n")
    target.write("sp=" + filename + "_1.output:bank0\n")
    target.write("nk=" + filename + ":1\n")
    return

def create_json_u2(data):
    target = open("platform.json","w+")
    target.write("{\n")
    target.write("    \"meminfo\": [\n")
    target.write("    {\n")
    target.write("        \"type\": \"DDR\",\n")
    target.write("        \"banks\": [\n")                    
    target.write("            {\n")
    target.write("                \"name\": \"bank0\"\n")
    target.write("            }\n")
    target.write("        ]\n")
    target.write("    }")
    target.write("\n    ],\n")
    target.write("    \"total_ddr_banks\": \"1\",\n")
    target.write("    \"version\": \"1.1\"\n")
    target.write("}\n")
    return

def create_cfg_vck(data):    
    target = open("platform_"+filename+".cfg","w+")
    target.write("[connectivity]\n")
    target.write("sp=" + filename + "_1.input:MC_NOC0\n")
    target.write("sp=" + filename + "_1.output:MC_NOC0\n")
    target.write("nk=" + filename + ":1\n")
    return

def create_json_vck(data):
    target = open("platform.json","w+")
    target.write("{\n")
    target.write("    \"meminfo\": [\n")
    target.write("    {\n")
    target.write("        \"type\": \"DDR\",\n")
    target.write("        \"banks\": [\n")                    
    target.write("            {\n")
    target.write("                \"name\": \"MC_NOC0\"\n")
    target.write("            }\n")
    target.write("        ]\n")
    target.write("    }")
    target.write("\n    ],\n")
    target.write("    \"total_ddr_banks\": \"1\",\n")
    target.write("    \"version\": \"1.1\"\n")
    target.write("}\n")
    return

def create_cfg(data):
    Memory_list = [0, 0, 0]
    DDR_count = 0
    HOST_count = 0
    if "hardwarePlatform" in data:
        if "extensions" in data["hardwarePlatform"]:
            if "raptor2" in data["hardwarePlatform"]["extensions"]:
                if "feature_support" in data["hardwarePlatform"]["extensions"]["raptor2"]:
                    feature_support = data["hardwarePlatform"]["extensions"]["raptor2"]["feature_support"]
                    if "DDR" in feature_support and (filename!='hostmemory'):
                        Memory_list[0] = int(feature_support["DDR"])
                    if "HOST" in feature_support and (filename=='hostmemory'):
                        Memory_list[1] = int(feature_support["HOST"])
                    if "HBM" in feature_support and (filename!='hostmemory'):
                        Memory_list[2] = int(feature_support["HBM"])
    
    Total_banks = Memory_list[0] + Memory_list[1]
    if Memory_list[2] != 0:
        Total_banks += 1

    if Total_banks:
        target = open("platform_"+filename+".cfg","w+")
        target.write("[connectivity]\n")
        p = 0
        q = 0
        for Memory_type in Memory_list:
            if Memory_type != 0:
                if p == 0:
                    Memory_name = "DDR"
                elif p == 1:
                    Memory_name = "HOST"
                elif p == 2:
                    Memory_name = "HBM"
                    Memory_type = 1                   
                for i in range(Memory_type):
                    q += 1
                    if p == 2:
                        target.write("sp=" + filename + "_" + str(q) + ".input:" + Memory_name + "[" + "0:31" + "]\n")
                        target.write("sp=" + filename + "_" + str(q) + ".output:" + Memory_name + "[" + "0:31" + "]\n")
                    else : 
                        target.write("sp=" + filename + "_" + str(q) + ".input:" + Memory_name + "[" + str(i) + "]\n")
                        target.write("sp=" + filename + "_" + str(q) + ".output:" + Memory_name + "[" + str(i)+ "]\n")
            p += 1
        target.write("nk=" + filename + ":" + str(Total_banks) + "\n")
    else :        
        print("Error: This example is not supported for targeted device")
        os._exit(1)

    return

def create_json(data):
    Memory_list = [0, 0, 0]
    DDR_count = 0
    HOST_count = 0
    if "hardwarePlatform" in data:
        if "extensions" in data["hardwarePlatform"]:
            if "raptor2" in data["hardwarePlatform"]["extensions"]:
                if "feature_support" in data["hardwarePlatform"]["extensions"]["raptor2"]:
                    feature_support = data["hardwarePlatform"]["extensions"]["raptor2"]["feature_support"]
                    if "DDR" in feature_support: 
                        Memory_list[0] = int(feature_support["DDR"])
                    if "HOST" in feature_support:
                        Memory_list[1] = int(feature_support["HOST"])
                    if "HBM" in feature_support:
                        Memory_list[2] = int(feature_support["HBM"])
    
    Total_ddr_banks = Memory_list[0]
    Total_host_banks =  Memory_list[1]
    if Memory_list[2] != 0:
        Total_ddr_banks += 1

    if Total_ddr_banks:
        target = open("platform.json","w+")
        target.write("{\n")
        target.write("    \"meminfo\": [\n")
        p = 0
        q = 0
        for Memory_type in Memory_list:
            if Memory_type != 0:
                if q != 0:
                    target.write(",\n")
                if p == 0:
                    Memory_name = "DDR"
                elif p == 1:
                    Memory_name = "HOST"
                elif p == 2:
                    Memory_name = "HBM"
                    Memory_type = 1
                target.write("    {\n")
                target.write("        \"type\": \"" + Memory_name + "\",\n")
                target.write("        \"banks\": [\n")                    
                for i in range(Memory_type):
                    target.write("            {\n")
                    if p == 2:                        
                        target.write("                \"name\": \"" + Memory_name + "[" + "0:31" + "]\"\n")
                    else : 
                        target.write("                \"name\": \"" + Memory_name + "[" + str(i) + "]\"\n")
                    target.write("            }")
                    if i != Memory_type - 1:
                        target.write(",")
                    target.write("\n")
                target.write("        ]\n")
                target.write("    }")
                q += 1
            p += 1
        target.write("\n    ],\n")
        target.write("    \"total_ddr_banks\": \"" +  str(Total_ddr_banks) + "\",\n")
        if Total_host_banks:
            target.write("    \"total_host_banks\": \"" +  str(Total_host_banks) + "\",\n")
        target.write("    \"version\": \"1.1\"\n")
        target.write("}\n")
    else :        
        print("Error: This example is not supported for targeted device")
        os._exit(1)
    return

plat_json_file = sys.argv[1]
filename = sys.argv[2]
desc = open(plat_json_file,'r')
data = json.load(desc)
desc.close()

plat_name = data["uniqueName"]

if "_u2_" in plat_name:
    if(filename!='verify'):
        create_cfg_u2(data)
    create_json_u2(data)
elif "vck" in plat_name:
    if(filename!='verify'):
        create_cfg_vck(data)
    create_json_vck(data)
else:    
    if(filename!='verify'):
        create_cfg(data)
    create_json(data)
