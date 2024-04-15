#!/usr/bin/python3

# Copyright (C) 2019-2021 Xilinx, Inc
#
# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

import os
import sys
import uuid
import re

# Following found in PYTHONPATH setup by XRT
import pyxrt

from utils_binding import *

def runKernel(opt):
    d = pyxrt.device(opt.index)
    xbin = pyxrt.xclbin(opt.bitstreamFile)
    uuid = d.load_xclbin(xbin)

    kernellist = xbin.get_kernels()

    rule = re.compile("vadd*")
    kernel = list(filter(lambda val: rule.match(val.get_name()), kernellist))[0]
    kHandle= pyxrt.kernel(d, uuid, kernel.get_name(), pyxrt.kernel.shared)

    zeros = bytearray(opt.DATA_SIZE)
    print("Allocate and initialize buffers")
    boHandle1 = pyxrt.bo(d, opt.DATA_SIZE, pyxrt.bo.normal, kHandle.group_id(0))
    boHandle1.write(zeros, 0)
    bo1 = boHandle1.map()

    boHandle2 = pyxrt.bo(d, opt.DATA_SIZE, pyxrt.bo.normal, kHandle.group_id(1))
    boHandle2.write(zeros, 0)
    bo2 = boHandle2.map()

    boHandle3 = pyxrt.bo(d, opt.DATA_SIZE, pyxrt.bo.normal, kHandle.group_id(2))
    boHandle3.write(zeros, 0)
    bo3 = boHandle3.map()

    for i in range(opt.DATA_SIZE):
        bo1[i] = 1 
        bo2[i] = 2

    bufReference = [3 for i in range(opt.DATA_SIZE)]

    boHandle1.sync(pyxrt.xclBOSyncDirection.XCL_BO_SYNC_BO_TO_DEVICE, opt.DATA_SIZE, 0)
    boHandle2.sync(pyxrt.xclBOSyncDirection.XCL_BO_SYNC_BO_TO_DEVICE, opt.DATA_SIZE, 0)

    print("Start the kernel")
    run = kHandle(boHandle1, boHandle2, boHandle3, opt.DATA_SIZE)
    print("Now wait for the kernel to finish")
    state = run.wait()

    print("Get the output data from the device and validate it")
    boHandle3.sync(pyxrt.xclBOSyncDirection.XCL_BO_SYNC_BO_FROM_DEVICE, opt.DATA_SIZE, 0)
    
    for i in range(opt.DATA_SIZE):
        if(bufReference[i] != bo3[i]):
            print("Computed value done not match reference")
            assert False

def main(args):
    opt = Options()
    Options.getOptions(opt, args)

    try:
        runKernel(opt)
        print("TEST PASSED")
        return 0

    except OSError as o:
        print(o)
        print("TEST FAILED")
        return -o.errno

    except AssertionError as a:
        print(a)
        print("TEST FAILED")
        return -1
    except Exception as e:
        print(e)
        print("TEST FAILED")
        return -1

if __name__ == "__main__":
    os.environ["Runtime.xrt_bo"] = "false"
    result = main(sys.argv)
    sys.exit(result)
