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

#include "graph.h"

PLIO* in1 = new PLIO("DataIn1", adf::plio_32_bits, "data/input1.txt");
PLIO* out1 = new PLIO("DataOut1", adf::plio_32_bits, "data/output.txt");

simulation::platform<1, 1> platform(in1, out1);

forwardGraph mygraph;

connect<> net0(platform.src[0], mygraph.in);
connect<> net1(mygraph.out, platform.sink[0]);

#if defined(__AIESIM__) || defined(__X86SIM__)
int main(int argc, char** argv) {
    mygraph.init();
    mygraph.run(1);
    mygraph.end();

    return 0;
}
#endif
