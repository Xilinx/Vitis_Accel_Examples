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

`define xil_warning(TAG, MSG) \
  $warning("[%s] (%m) %0t : %s", TAG, $time, MSG);

`define xil_info(TAG, MSG, LEVEL) \
  if(LEVEL > 300) \
  $display("INFO: [%s] (%m) %0t : %s", TAG, $time, MSG);

`define xil_error(TAG, MSG) \
  $error("[%s] (%m) %0t : %s", TAG, $time, MSG);

`define xil_fatal(TAG, MSG) \
  $fatal(1,"[%s] (%m) %0t : %s", TAG, $time, MSG);

