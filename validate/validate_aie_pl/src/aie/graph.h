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

#include <adf.h>
#include <adf/window/types.h>
#include <adf/stream/types.h>
#include <forward.hpp>

using namespace adf;

class forwardGraph : public adf::graph {
   private:
    kernel k1;

   public:
    port<input> in;
    port<output> out;

    forwardGraph() {
        k1 = kernel::create(forward);

        connect<window<2048 * 4> >(in, k1.in[0]);
        connect<window<2048 * 4> >(k1.out[0], out);

        source(k1) = "src/aie/forward.cpp";

        runtime<ratio>(k1) = 0.5;
    }
};
