#!/bin/csh

#
# Copyright 2022 Xilinx, Inc.
# Copyright 2022-2023 Advanced Micro Devices, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

if ( "$XILINX_VIVADO" == "" ) then
    echo "XILINX_VIVADO variable needs to be set"
    return 1
endif

set SCRIPT_PATH=`readlink -f $_`
set SCRIPT_DIR=`dirname "$SCRIPT_PATH"`
set ABSPATH=`echo $SCRIPT_PATH | awk '{print $2}'`
set ABSDIR=`echo $SCRIPT_DIR | awk '{print $2}'`

setenv PYTHONPATH ${XILINX_VIVADO}/data/emulation/python/xtlm_ipc:${XILINX_VIVADO}/data/emulation/hw_em/lib/python:${XILINX_VIVADO}/data/emulation/ip_utils/xtlm_ipc/xtlm_ipc_v1_0/python:${ABSDIR}/x3ntg_library/src:${ABSDIR}/deps:${ABSDIR}
echo "PYTHONPATH=${PYTHONPATH}"
