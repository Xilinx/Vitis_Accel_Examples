#!/bin/bash

#
# Copyright 2022 Xilinx, Inc.
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

ABSPATH=$(realpath $BASH_SOURCE)
ABSDIR=$(dirname "$ABSPATH")

# Install relavant packages to local directory
if [ ! -d "$ABSDIR/deps" ] 
then 
    mkdir -p deps
    python3 -m pip install urllib3 -t $ABSDIR/deps
    python3 -m pip install pyparsing -t $ABSDIR/deps
    python3 -m pip install attrs -t $ABSDIR/deps
    python3 -m pip install pyrsistent -t $ABSDIR/deps
    python3 -m pip install importlib-metadata -t $ABSDIR/deps
    python3 -m pip install six -t $ABSDIR/deps
    python3 -m pip install scapy -t $ABSDIR/deps
else
    echo "Dependencies (Six and Scappy) are already installed"
fi
