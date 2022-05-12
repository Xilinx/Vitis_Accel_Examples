/**
* Copyright (C) 2022 Xilinx, Inc
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


//GMIO
GMIO *in_source1 = new GMIO("in_source1", 64, 100);
GMIO *in_source2 = new GMIO("in_source2", 64, 100);
GMIO *out_sink = new GMIO("out_sink", 64, 100);


// instantiate adf dataflow graph to compute weighted moving average
Mm mm;

// connect dataflow graph to simulation platform
simulation::platform<2,1> platform(in_source1,in_source2,out_sink);


connect<> net0(platform.src[0], mm.inA);			// Full system source --> AIE Array
connect<> net1(platform.src[1], mm.inB);			// Full system source --> AIE Array
connect<> net3(mm.out, platform.sink[0]);		// AIE Array --> Full system sink

#ifdef __AIESIM__
// initialize and run the dataflow graph
int main(void) 
{
	  mm.init();
	  mm.run();
	  mm.end();
  return 0;
}

#endif
