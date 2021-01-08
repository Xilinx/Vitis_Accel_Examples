/**
* Copyright (C) 2020 Xilinx, Inc
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
#define MAX_COLS 512
#define K 11

#define TEST_WIDTH 256
#define TEST_HEIGHT 256

// TRIPCOUNT identifier
const unsigned int c_size = TEST_WIDTH * TEST_HEIGHT;

const unsigned int vcoeff[K] = {36, 111, 266, 498, 724, 821, 724, 498, 266, 111, 36};
