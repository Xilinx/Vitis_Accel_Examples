#!/usr/bin/env python
import os
import sys
import json

sys.dont_write_bytecode = True
krnl_names = sys.argv[1:]

try:
	s3_bucket = os.environ['S3_BUCKET']
	s3_dcp_key = os.environ['S3_DCP_KEY']
	s3_logs_key = os.environ['S3_LOGS_KEY']
except KeyError:
	s3_bucket = ""
	s3_dcp_key = ""
	s3_logs_key = ""

if s3_bucket == "" or s3_dcp_key == "" or s3_logs_keys == "":
	creds_filename = os.path.expanduser("~/.aws_creds.json")
	if os.path.isfile(creds_filename):
		creds_fp = open(creds_filename)
		creds = json.load(creds_fp)
		s3_bucket = creds['s3_bucket']
		s3_dcp_key = creds['s3_dcp_key']
		s3_logs_key = creds['s3_logs_key']
	else:
		print "ERROR: Must have ~/.aws_creds.json file"
		sys.exit(-1)

length = len(krnl_names)
string = os.environ["SDACCEL_DIR"]
for i in range(0, length):
    krnl_name = krnl_names[i].replace('.xclbin', '')
    os.system(string + "/tools/create_sdaccel_afi.sh -s3_bucket=" + s3_bucket + " -s3_dcp_key=" + s3_dcp_key + " -s3_logs_key=" + s3_logs_key + " -xclbin=" + krnl_name + ".xclbin -o=" + krnl_name)
