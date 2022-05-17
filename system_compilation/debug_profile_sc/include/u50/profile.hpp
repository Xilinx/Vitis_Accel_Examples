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

#ifdef ENABLE_HW_PROFILING

PROFILE_KERNEL("all");
// example: only specific PEs
// PROFILE_KERNEL("incr_20[0] incr_20[1]");

PROFILE_PORT("all");
// example: only specific ports
// PROFILE_PORT("incr_20[0]/Y incr_20[1]/E");

// PROFILE_OFFLOAD("FIFO" | "DDR[0-3]" | "HBM[0-31]" );
PROFILE_OFFLOAD("HBM[4]");

#endif
