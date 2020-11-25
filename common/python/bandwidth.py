"""
 Copyright (C) 2020 Xilinx, Inc

 Ctypes based based bandwidth testcase used with every platform as part of
 xbutil validate

 Licensed under the Apache License, Version 2.0 (the "License"). You may
 not use this file except in compliance with the License. A copy of the
 License is located at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 License for the specific language governing permissions and limitations
 under the License.
"""

import sys
import time
import math

# Following found in PYTHONPATH setup by XRT
from xrt_binding import *
from ert_binding import *

# utils_binding.py
from utils_binding import *

# Define libc helpers
libc_name = ctypes.util.find_library("c")
libc = ctypes.CDLL(libc_name)
libc.memcmp.argtypes = (ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t)
libc.memcmp.restype = (ctypes.c_int)
libc.memcpy.argtypes = (ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t)
libc.memcpy.restype = (ctypes.c_void_p)

current_micro_time = lambda: int(round(time.time() * 1000000))

globalbuffersize = 1024*1024*16    #16 MB

def getInputOutputBuffer(devhdl, krnlhdl, argno, isInput):
    grpid = xrtKernelArgGroupId(krnlhdl, argno)
    if grpid < 0:
        raise RuntimeError("failed to find BO group ID: %d" % grpid)
    bo = xrtBOAlloc(devhdl, globalbuffersize, 0, grpid)
    if bo == 0:
        raise RuntimeError("failed to alloc buffer")

    bobuf = xrtBOMap(bo)
    if bobuf == 0:
        raise RuntimeError("failed to map buffer")
    lst = ctypes.cast(bobuf, ctypes.POINTER(ctypes.c_char))

    for i in range(globalbuffersize):
        lst[i] = i%256 if isInput else 0

    xrtBOSync(bo, xclBOSyncDirection.XCL_BO_SYNC_BO_TO_DEVICE, globalbuffersize, 0)
    return bo, bobuf

def runKernel(opt, num_kernel):
    krnl_name = "bandwidth"
    khandle = []
    for i in range(num_kernel):
        cu_id = str(i + 1);
        krnl_name_full = krnl_name + ":{" + "bandwidth_" + cu_id + "}"
        print('Creating a kernel {0} for CU({1})\n'.format(krnl_name_full,i + 1))
        khandle.append(xrtPLKernelOpen(opt.handle, opt.xuuid, krnl_name_full))

    kfunc = xrtKernelGetFunc(xrtBufferHandle, xrtBufferHandle, ctypes.c_int, ctypes.c_int)

    output_bo = []
    output_buf = []
    input_bo = []
    input_buf = []
    
    for i in range(num_kernel):
        output_bo_temp, output_buf_temp = getInputOutputBuffer(opt.handle, khandle[i], 0, False)
        output_bo.append(output_bo_temp)
        output_buf.append(output_buf_temp)
        input_bo_temp, input_buf_temp = getInputOutputBuffer(opt.handle, khandle[i], 1, True)
        input_bo.append(input_bo_temp)
        input_buf.append(input_buf_temp)

    typesize = 512
    globalbuffersizeinbeats = globalbuffersize/(typesize>>3)
    tests= int(math.log(globalbuffersizeinbeats, 2.0))+1
    beats = 16

    #lists
    dnsduration = []
    dsduration  = []
    dbytes      = []
    dmbytes     = []
    bpersec     = []
    mbpersec    = []

    #run tests with burst length 1 beat to globalbuffersize
    #double burst length each test
    test=0
    throughput = []
    rhandle = []
    failed = False
    while beats <= 1024 and not failed:
        print("LOOP PIPELINE %d beats" %beats)

        usduration = 0
        fiveseconds = 5*1000000
        reps = 64
        while usduration < fiveseconds:
            del rhandle[:]
            start = current_micro_time()
            for i in range(num_kernel):
                rhandle.append(kfunc(khandle[i], output_bo[i], input_bo[i], beats, reps))
            for i in range(num_kernel):
                xrtRunWait(rhandle[i])
            end = current_micro_time()

            for i in range(num_kernel):
                xrtRunClose(rhandle[i])

            usduration = end-start

            limit = beats*(typesize>>3)
            for i in range(num_kernel):
                xrtBOSync(output_bo[i], xclBOSyncDirection.XCL_BO_SYNC_BO_FROM_DEVICE, limit, 0)
                if libc.memcmp(input_buf[i], output_buf[i], limit):
                   failed = True
                   break

            # print("Reps = %d, Beats = %d, Duration = %lf us" %(reps, beats, usduration)) # for debug

            if usduration < fiveseconds:
                reps = reps*2

        dnsduration.append(usduration)
        dsduration.append(dnsduration[test]/1000000.0)
        dbytes.append(reps*beats*(typesize>>3))
        dmbytes.append(dbytes[test]/(1024 * 1024))
        bpersec.append((num_kernel*dbytes[test])/dsduration[test])
        mbpersec.append(2.0*bpersec[test]/(1024 * 1024))
        throughput.append(mbpersec[test])
        print("Test %d, Throughput: %d MB/s" %(test, throughput[test]))
        beats = beats*4
        test+=1

    #cleanup
    for i in range(num_kernel):
        xrtBOFree(input_bo[i])
        xrtBOFree(output_bo[i])
        xrtKernelClose(khandle[i])

    if failed:
        raise RuntimeError("ERROR: Failed to copy entries")
    
    print("TTTT: %d" %throughput[0])
    print("Maximum throughput: %d MB/s" %max(throughput))

def main(args):
    opt = Options()
    Options.getOptions(opt, args)

    num_kernel = parsePlatform(opt)

    try:
        initXRT(opt)
        assert (opt.first_mem >= 0), "Incorrect memory configuration"

        runKernel(opt,num_kernel)
        print("PASSED TEST")

    except OSError as o:
        print(o)
        print("FAILED TEST")
        sys.exit(o.errno)
    except AssertionError as a:
        print(a)
        print("FAILED TEST")
        sys.exit(1)
    except Exception as e:
        print(e)
        print("FAILED TEST")
        sys.exit(1)
    finally:
        xrtDeviceClose(opt.handle)

if __name__ == "__main__":
    main(sys.argv)
