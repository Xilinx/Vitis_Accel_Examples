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

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <adf.h>
#include "aie_kernel.h"

using namespace adf;

class simpleGraph : public graph {
   private:
    kernel adder;

   public:
    port<input> in0, in1;
    port<output> out;

    simpleGraph() {
        adder = kernel::create(aie_adder);

        connect<stream>(in0, adder.in[0]);
        connect<stream>(in1, adder.in[1]);
        connect<stream>(adder.out[0], out);

        source(adder) = "aie_adder.cc";

        runtime<ratio>(adder) = 0.1;
    };
};

#endif /**********__GRAPH_H__**********/
