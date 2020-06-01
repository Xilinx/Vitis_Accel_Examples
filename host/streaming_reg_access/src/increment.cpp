#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

extern "C" {
void increment(hls::stream<qdma_axis<32, 0, 0, 0>> &input,
               hls::stream<qdma_axis<32, 0, 0, 0>> &output, int foo,
               int &beyond_foo) {
#pragma HLS interface axis port = input
#pragma HLS interface axis port = output
#pragma HLS interface s_axilite port = foo bundle = control
#pragma HLS interface ap_hs port = foo
#pragma HLS interface s_axilite port = beyond_foo bundle = control
#pragma HLS interface ap_ctrl_none port = return

  int count = 0;
  while (1) {
    qdma_axis<32, 0, 0, 0> v = input.read();
    if (v.data > foo)
      count++;
    output.write(v);
    if (v.last) {
      break;
    }
  }
  beyond_foo = count;
}
}
