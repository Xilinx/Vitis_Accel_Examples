#!/usr/bin/env python
from sys import argv
import json
import glob
import os
import re
import subprocess

script, desc_file = argv
desc = open(desc_file, 'r')
data = json.load(desc)
desc.close()

includes = []
if 'libs' in data:
    for item in data['libs']:
        dictny = {}
        dictny["name"] = item
        dictny["location"] = "GIT_REPO_DIR/common/includes/" + item + "/"
        includes.append(dictny)
    data["includes"] = includes
    del data['libs']

target = open('description.json', 'w+')
json.dump(data, target, indent=4, sort_keys=False)
