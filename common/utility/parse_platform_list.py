#!/usr/bin/python
import os
import sys

def main ():
    dev = sys.argv[1]
    if "PLATFORM_REPO_PATHS" in os.environ:
        plist = os.environ['PLATFORM_REPO_PATHS'].split(":")
        for shell in plist:
            if os.path.isdir(shell + "/" + dev):
                return shell

print (main())
