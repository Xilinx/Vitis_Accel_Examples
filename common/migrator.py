#!/usr/bin/env python

import sys
from sys import argv
import json
import glob
import os
import re
import subprocess
sys.dont_write_bytecode = True

if __name__=="__main__":
    pwd = os.path.dirname(sys.path[0])
    cpp_list  = ['kernel_to_gmem', 'kernel_opt', 'dataflow', 'clk_freq']
    copy_list = ['hello_world', 'cpu_to_fpga', 'host', 'rtl_kernel', 'debug']
    application_list = ['kmeans', 'watermarking', 'median_filter']
    folder_list = ['acceleration', 'vision', 'getting_started']
    libutil_list = ['libs', 'utility']
    script, folder = argv
    sdaccel_folder = os.path.abspath(folder)
    for level1 in os.listdir(sdaccel_folder):
        if level1 in folder_list:
            for level2 in os.listdir(sdaccel_folder + '/' + level1):
                if level2 in copy_list:
                    copy_dir = os.path.abspath(sdaccel_folder + '/' + level1 + '/' + level2)
                    os.system("cp -rf " + copy_dir + " " + pwd + "/") 
                if level2 in cpp_list:
                    for level3 in os.listdir(sdaccel_folder + '/' + level1 + '/' + level2):
                        copy_dir = os.path.abspath(sdaccel_folder + '/' + level1 + '/' + level2 + '/' + level3)
                        os.system("cp -rf " + copy_dir + " " + pwd + "/cpp_kernel/")
                if level2 in application_list:
                    copy_dir = os.path.abspath(sdaccel_folder + '/' + level1 + '/' + level2)
                    os.system("cp -rf " + copy_dir + " " + pwd + "/applications/")
            
    for level1 in os.listdir(sdaccel_folder):
        if level1 in libutil_list:
            copy_dir = os.path.abspath(sdaccel_folder + '/' + level1)
            os.system("cp -rf " + copy_dir + " " + pwd + "/common/") 

                  


                   
