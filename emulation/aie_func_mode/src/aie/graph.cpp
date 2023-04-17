#include "graph.h"

PLIO *in0 = new PLIO("DataIn1", adf::plio_32_bits,"data/input.txt");
PLIO *out0 = new PLIO("DataOut1",adf::plio_32_bits, "data/output.txt");

PLIO *pl_to_ai_source = new PLIO("polar_clip_in",adf::plio_32_bits, "data/input1.txt"); 
PLIO *ai_to_pl_sink = new PLIO("polar_clip_out", adf::plio_32_bits,"data/output1.txt");  


simulation::platform<2,2> platform(in0,pl_to_ai_source, out0,ai_to_pl_sink);

clipped clipgraph;

connect<> net0(platform.src[0], clipgraph.in);
connect<> net1(clipgraph.clip_in,platform.sink[1]);
connect<> net2(platform.src[1],clipgraph.clip_out);
connect<> net3(clipgraph.out, platform.sink[0]);


int main(int argc, char ** argv) {
	clipgraph.init();
    clipgraph.run();
    clipgraph.end();
    return 0;
}
