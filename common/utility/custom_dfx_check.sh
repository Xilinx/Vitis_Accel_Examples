#!/bin/bash
PLATFORM=$1
XF_PROJ_ROOT=$2
rm -rf ./platform_info.json
platforminfo -j -d $PLATFORM > platform_info.json
$XF_PROJ_ROOT/common/utility/custom_platform_check.py ./platform_info.json
