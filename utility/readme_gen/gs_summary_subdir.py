#!/usr/bin/env python

import os, re
import fnmatch
import json
import sys

# To avoid .pyc files
sys.dont_write_bytecode = True

sys.path.append(".")
import gs_summary_util

gs_summary_util.genReadMe2(".")
