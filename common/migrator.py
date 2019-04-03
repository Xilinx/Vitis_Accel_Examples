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
    counter = {'hello_world': 100, 'host': 200, 'cpp_kernels': 300,
            'rtl_kernels':400, 'ocl_kernels':500, 'applications':600}
    os.system("mkdir -p " + str(counter['hello_world']) + "_hello_world " +
                            str(counter['host']) + "_host " +
                            str(counter['cpp_kernels']) + "_cpp_kernels " +
                            str(counter['rtl_kernels']) + "_rtl_kernels " +
                            str(counter['ocl_kernels']) + "_ocl_kernels " +
                            str(counter['applications']) + "_applications ")
    hwd_ctr = counter['hello_world'] + 1
    hst_ctr = counter['host'] + 1
    cpp_ctr = counter['cpp_kernels'] + 1
    rtl_ctr = counter['rtl_kernels'] + 1
    ocl_ctr = counter['ocl_kernels'] + 1
    app_ctr = counter['applications'] + 1
    pwd = os.path.dirname(sys.path[0])
    cpp_list  = ['kernel_to_gmem', 'kernel_opt', 'dataflow', 'clk_freq']
    copy_list = ['hello_world', 'host', 'rtl_kernel', 'debug']
    application_list = ['kmeans', 'watermarking', 'median_filter']
    folder_list = ['acceleration', 'vision', 'getting_started']
    libutil_list = ['libs', 'utility']
    script, folder = argv
    sdaccel_folder = os.path.abspath(folder)
    for level1 in os.listdir(sdaccel_folder):
        if level1 in folder_list:
            for level2 in os.listdir(sdaccel_folder + '/' + level1):
                if level2 in copy_list:
                    for level3 in os.listdir(sdaccel_folder + '/' + level1 + '/' + level2):
                        copy_dir = os.path.abspath(sdaccel_folder + '/' + level1
                                + '/' + level2 + '/' + level3)
                        if re.search('ocl', level3):
                            os.system("cp -rf " + copy_dir + " " + pwd + "/" +
                                    str(counter['ocl_kernels']) + "_ocl_kernels/" +
                                    str(ocl_ctr) + "_" + level3)
                            ocl_ctr = ocl_ctr + 1
                        elif os.path.isdir(copy_dir):
                            if re.search('hello_world', level2):
                                os.system("cp -rf " + copy_dir + " " + pwd + "/" +
                                    str(counter['hello_world']) + "_hello_world/" +
                                    str(hwd_ctr) + "_" + level3)
                                hwd_ctr = hwd_ctr + 1
                            if re.search('host', level2):
                                os.system("cp -rf " + copy_dir + " " + pwd + "/" +
                                    str(counter['host']) + "_host/" +
                                    str(hst_ctr) + "_" + level3)
                                hst_ctr = hst_ctr + 1
                            if re.search('rtl_kernel', level2):
                                os.system("cp -rf " + copy_dir + " " + pwd + "/" +
                                    str(counter['rtl_kernels']) + "_rtl_kernels/" +
                                    str(rtl_ctr) + "_" + level3)
                                rtl_ctr = rtl_ctr + 1
                if level2 in cpp_list:
                    for level3 in os.listdir(sdaccel_folder + '/' + level1 + '/' + level2):
                        copy_dir = os.path.abspath(sdaccel_folder + '/' + level1
                                + '/' + level2 + '/' + level3)
                        if re.search('ocl', level3):
                            os.system("cp -rf " + copy_dir + " " + pwd + "/" +
                                    str(counter['ocl_kernels']) + "_ocl_kernels/" +
                                    str(ocl_ctr) + "_" + level3)
                            ocl_ctr = ocl_ctr + 1
                        elif os.path.isdir(copy_dir):
                            os.system("cp -rf " + copy_dir + " " + pwd + "/" +
                                    str(counter['cpp_kernels']) + "_cpp_kernels/" +
                                    str(cpp_ctr) + "_" + level3)
                            cpp_ctr = cpp_ctr + 1
                if level2 in application_list:
                    copy_dir = os.path.abspath(sdaccel_folder + '/' + level1 +
                            '/' + level2)
                    if os.path.isdir(copy_dir):
                        os.system("cp -rf " + copy_dir + " " + pwd + "/" +
                                    str(counter['applications']) + "_applications/" +
                                    str(app_ctr) + "_" + level2)
                        app_ctr = app_ctr + 1
    
    os.system(pwd + "/common/utility/makefile_gen/update_makegen_all.sh")
    os.system(pwd + "/common/utility/readme_gen/update_all_readme.sh")
    os.system(pwd + "/common/utility/check_readme.sh")
    os.system(pwd + "/common/utility/check_makefile.sh")
            
#    for level1 in os.listdir(sdaccel_folder):
#        if level1 in libutil_list:
#            copy_dir = os.path.abspath(sdaccel_folder + '/' + level1)
#            os.system("cp -rf " + copy_dir + " " + pwd + "/common/") 

                  


                   
