#!/usr/bin/env python
from sys import argv
import json
import argparse
import glob
import os
import re
import sys
import subprocess

def check_cust_plt(data):
    if "hardwarePlatform" in data:
        if "usesPR" in data["hardwarePlatform"]:
            dfx_chk = data["hardwarePlatform"]["usesPR"]
    elif "hardwarePlatforms" in data:
        if "hardwarePlatform" in data["hardwarePlatforms"]:
            if "usesPR" in data["hardwarePlatforms"]["hardwarePlatform"]:
                dfx_chk = data["hardwarePlatforms"]["hardwarePlatform"]["usesPR"]
        elif "reconfigurablePartitions" in data["hardwarePlatforms"]:
            for reconfigurablePartitions in data["hardwarePlatforms"]["reconfigurablePartitions"]:
                if "hardwarePlatform" in reconfigurablePartitions:
                    if "usesPR" in reconfigurablePartitions["hardwarePlatform"]:
                        dfx_chk = reconfigurablePartitions["hardwarePlatform"]["usesPR"]
    
    return dfx_chk

plat_json_file = sys.argv[1]
desc = open(plat_json_file,'r')
data = json.load(desc)
desc.close()

print(check_cust_plt(data))
