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
import sys
import getopt
import json
import struct
import uuid

class Options(object):
    def __init__(self):
        self.DATA_SIZE = 1024
        self.sharedLibrary = None
        self.bitstreamFile = None
        self.halLogFile = None
        self.alignment = 4096
        self.option_index = 0
        self.index = 0
        self.cu_index = 0
        self.verbose = False
        self.handle = None
        self.xcl_handle = None
        self.first_mem = -1
        self.cu_base_addr = -1
        self.xuuid = uuid.uuid4()
        self.kernels = []
        self.platformfile = None

    def getOptions(self, argv):
        try:
            opts, args = getopt.getopt(argv[1:], "k:p:l:a:c:d:vhe", ["bitstream=", "platform_json=", "hal_logfile=", "alignment=",
                                                                   "cu_index=", "device=", "verbose", "help", "ert"])
        except getopt.GetoptError:
            print(self.printHelp())
            sys.exit(2)

        for o, arg in opts:
            if o in ("--bitstream", "-k"):
                self.bitstreamFile = arg
            elif o in ("--platform_json", "-p"):
                self.platformfile = arg
            elif o in ("--hal_logfile", "-l"):
                self.halLogFile = arg
            elif o in ("--alignment", "-a"):
                print("-a/--alignment switch is not supported")
            elif o in ("--cu_index", "-c"):
                self.cu_index = int(arg)
            elif o in ("--device", "-d"):
                self.index = int(arg)
            elif o in ("--help", "-h"):
                print(self.printHelp())
            elif o == "-v":
                self.verbose = True
            elif o in ("-e", "--ert"):
                print("-e/--ert switch is not supported")
            else:
                assert False, "unhandled option"

        if self.bitstreamFile is None:
            raise RuntimeError("No bitstream specified")

        if self.halLogFile:
            print("Log files are not supported on command line, Please use xrt.ini to specify logging configuration")
        print("Host buffer alignment " + str(self.alignment) + " bytes")
        print("Compiled kernel = " + self.bitstreamFile)

    def printHelp(self):
        print("usage: %s [options]")
        print("  -k <bitstream>")
        print("  -p <platform_json>")
        print("  -d <device_index>")
        print("  -c <cu_index>")
        print("  -v")
        print("  -h")
        print("")
        print("* Bitstream is required")

def parsePlatform(opt):
    desc = open(opt.platformfile, 'r')
    data = json.load(desc)
    desc.close()

    count = 0
    if "hardwarePlatform" in data:
        if "busInterfaces" in data["hardwarePlatform"]:
            for busInterfaces in data["hardwarePlatform"]["busInterfaces"]:
                if "spTag" in busInterfaces:
                    if busInterfaces["spTag"] == "DDR" or busInterfaces["spTag"] == "HOST":
                        if "addressSegments" in busInterfaces:
                            for addressSegments in busInterfaces["addressSegments"]:
                                count += 1
                    elif busInterfaces["spTag"] == "HBM":
                        if "addressSegments" in busInterfaces:
                            count += 1
    return count
