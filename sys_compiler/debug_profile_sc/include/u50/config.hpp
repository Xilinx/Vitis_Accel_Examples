/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

/////// hardware config settings

// global memory connections to Accelerator ports
#define PORT_MAP_A (HBM[0] : HBM[2])
#define PORT_MAP_B (HBM[0] : HBM[2])
#define PORT_MAP_E (HBM[0] : HBM[2])

// max (MxM) matrix size for accelerator code
constexpr int MaxM = 8;
// array partition factor
constexpr int APF = MaxM * MaxM;
// depth of axi-stream buffer between PEs (can be diff from matrix size)
constexpr int STREAM_DEPTH = 64;

// Number of (replicated ACC) compute units
#define NCU 2
