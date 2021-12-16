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
#include "aie/kernels/include.h"
#include "kernels.h"
#include <adf.h>

using namespace adf;

simpleGraph mygraph;

PLIO* din0 = new PLIO("Datain0", plio_32_bits, "data/input0.txt");
PLIO* dout0 = new PLIO("Dataout0", plio_32_bits, "data/output0.txt");
PLIO* ctrlIn = new PLIO("Ctrlin0", plio_32_bits, "data/ctrlIn.txt");

simulation::platform<2, 1> platform(din0, ctrlIn, dout0);

connect<> net0(platform.src[0], mygraph.in0);
connect<> net1(mygraph.out0, platform.sink[0]);
connect<> net2(platform.src[1], mygraph.ctrl_in);

#if defined(__AIESIM__) || defined(__ADF_FRONTEND__)
int main(int argc, char** argv) {
    mygraph.init();

    mygraph.update(mygraph.num_samples, NUM_SAMPLES1);
    std::cout << "main(): Starting first graph run sequence..." << std::endl;
    mygraph.run(NUM_ITERATIONS1);
    std::cout << "main(): Waiting for first graph run sequence to finish..." << std::endl;
    mygraph.wait();
    std::cout << "main(): Done with first graph run sequence." << std::endl;

    mygraph.update(mygraph.num_samples, NUM_SAMPLES2);
    std::cout << "main(): Starting second graph run sequence..." << std::endl;
    mygraph.run(NUM_ITERATIONS2);
    std::cout << "main(): Waiting for second graph run sequence to finish..." << std::endl;
    mygraph.end();
    std::cout << "main(): Done with second graph run sequence." << std::endl;

    return 0;
}
#endif
