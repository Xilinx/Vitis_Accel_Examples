/*
 * Copyright 2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "graph.h"
#include "aie/kernels/include.h"
#include "kernels.h"
#include <adf.h>

using namespace adf;

//setup test graph and PLIO/GMIO
class mygraph_test : public adf::graph {
   public:
    // 1.input and output port
    output_plio dout0;
    input_plio din0;
    input_plio ctrlIn;

    // 2.setup graph
    simpleGraph mygraph;

    // 3.setup graph test, port adf::buffer to adf::plio
    mygraph_test() {
        // input & output plio
        din0 = adf::input_plio::create("Datain0", adf::plio_32_bits, "data/input0.txt");
        dout0 = adf::output_plio::create("Dataout0", adf::plio_32_bits, "data/output0.txt");
        ctrlIn = adf::input_plio::create("Ctrlin0", adf::plio_32_bits, "data/ctrlIn.txt");

        // connections
        adf::connect<>(din0.out[0], mygraph.in0);
        adf::connect<>(ctrlIn.out[0], mygraph.ctrl_in);
        adf::connect<>(mygraph.out0, dout0.in[0]);
    }

};

mygraph_test mygraph;

#if defined(__AIESIM__) || defined(__ADF_FRONTEND__)
int main(int argc, char** argv) {
    mygraph.init();

    std::cout << "main(): Starting first graph run sequence..." << std::endl;
    mygraph.run(NUM_ITERATIONS1);
    std::cout << "main(): Waiting for first graph run sequence to finish..." << std::endl;
    mygraph.wait();
    std::cout << "main(): Done with first graph run sequence." << std::endl;

    std::cout << "main(): Starting second graph run sequence..." << std::endl;
    mygraph.run(NUM_ITERATIONS2);
    std::cout << "main(): Waiting for second graph run sequence to finish..." << std::endl;
    mygraph.end();
    std::cout << "main(): Done with second graph run sequence." << std::endl;

    return 0;
}
#endif
