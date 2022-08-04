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
#include "pl_controller_hw.hpp"

/**
 * @brief parse_cfg parse configure to get ucode size
 *
 * @param cfg_buf input configure buffer for ddr
 * @param ucode_sz ouput ucode size
 * @param rt_addr_buff output runtime address
 */
void parse_cfg(ap_uint<32>* cfg_buf, hls::stream<ap_uint<32> >& ucode_sz_strm, ap_uint<64>* rt_addr_buff) {
    ucode_sz_strm.write(cfg_buf[0]);
    int rt_sz = cfg_buf[1];
    for (int i = 0; i < rt_sz; i++) {
#pragma HLS pipeline II = 2
        uint64_t low32 = cfg_buf[i * 2 + 2];
        uint64_t high32 = cfg_buf[i * 2 + 3];
        rt_addr_buff[i] = low32 + (high32 << 32);
    }
}

extern "C" void pl_controller_hw(ap_uint<32>* ucode_buf,
                                 ap_uint<32>* cfg_buf,
                                 ap_uint<32>* out_buf,
                                 maxi<ap_uint<W> > aie_ctrl,
                                 maxi<ap_uint<W> > aie_ctrl2) {
    const int MWL = 16;
#pragma HLS INTERFACE m_axi latency = 32 bundle = gmem_ucode port = ucode_buf offset = slave depth = 16384
#pragma HLS INTERFACE m_axi latency = 32 bundle = gmem_cfg port = cfg_buf offset = slave depth = 1024
#pragma HLS INTERFACE m_axi latency = 32 bundle = gmem_out port = out_buf offset = slave depth = 1024
#pragma HLS INTERFACE m_axi max_write_burst_length = 256 num_write_outstanding = 16 latency = MWL bundle = \
    gmem_aie_ctrl port = aie_ctrl.Ptr offset = slave depth = 268435456
#pragma HLS INTERFACE m_axi max_write_burst_length = 256 num_write_outstanding = 16 latency = MWL bundle = \
    gmem_aie_ctrl2 port = aie_ctrl2.Ptr offset = slave depth = 268435456

#pragma HLS INTERFACE s_axilite bundle = control port = ucode_buf
#pragma HLS INTERFACE s_axilite bundle = control port = cfg_buf
#pragma HLS INTERFACE s_axilite bundle = control port = out_buf
#pragma HLS INTERFACE s_axilite bundle = control port = aie_ctrl.Ptr
#pragma HLS INTERFACE s_axilite bundle = control port = aie_ctrl2.Ptr
#pragma HLS INTERFACE s_axilite bundle = control port = return

#pragma HLS dataflow
#ifndef __SYNTHESIS__
    std::cout << "pl_controller_hw kernel start...\n";
#endif
    hls::stream<ap_uint<32>, 2> ucode_sz_strm("ucode_sz_strm");
    ap_uint<64> rt_addr_buff[512];
#pragma HLS bind_storage variable = rt_addr_buff type = ram_1p impl = bram

#ifndef __SYNTHESIS__
    std::cout << "parse_cfg...\n";
#endif
    // parse configure to get ucode size, and runtime address
    parse_cfg(cfg_buf, ucode_sz_strm, rt_addr_buff);

    hls::stream<ap_uint<33>, 64> ucode_strm("ucode_strm");
#ifndef __SYNTHESIS__
    std::cout << "ucode_reader...\n";
#endif
    // read ucode_buf, read ucode from DDR, and write stream
    ucode_reader(ucode_sz_strm, ucode_buf, ucode_strm);

    hls::stream<ap_uint<8>, 1024> type_strm[CH_NUM];
    hls::stream<ap_uint<64>, 1024> addr_nm_strm[CH_NUM];
    hls::stream<ap_uint<W>, 4096> val_strm[CH_NUM];
#pragma HLS bind_storage variable = type_strm type = fifo impl = bram
#pragma HLS bind_storage variable = addr_nm_strm type = fifo impl = uram
#pragma HLS bind_storage variable = val_strm type = fifo impl = uram
#ifndef __SYNTHESIS__
    std::cout << "ucode_decoder...\n";
#endif
    // decode ucode and runtime address, dispatch them to aximm_trans_handler
    ucode_decoder<W, CH_NUM>(rt_addr_buff, ucode_strm, type_strm, addr_nm_strm, val_strm);

    hls::stream<ap_uint<16>, 32> sync_strm[CH_NUM];
    hls::stream<ap_uint<16>, 32> sync_strm2[CH_NUM];

#ifndef __SYNTHESIS__
    std::vector<std::thread> vec;
    std::string name;
    for (int ch = 0; ch < CH_NUM; ch++) {
        name = "type_strm-" + std::to_string(ch);
        type_strm[ch].set_name(name.c_str());
        name = "addr_nm_strm-" + std::to_string(ch);
        addr_nm_strm[ch].set_name(name.c_str());
        name = "val_strm-" + std::to_string(ch);
        val_strm[ch].set_name(name.c_str());
        name = "sync_strm-" + std::to_string(ch);
        sync_strm[ch].set_name(name.c_str());
        name = "sync_strm2-" + std::to_string(ch);
        sync_strm2[ch].set_name(name.c_str());
    }
#endif

    int ch = 0;
#ifndef __SYNTHESIS__
    std::cout << "aximm_trans_handler[" << ch << "]...\n";
    // push aximm_trans_handler_th to thread for csim
    vec.push_back(std::thread(aximm_trans_handler_th<W, ADDR_SHIFT, MWL>, &aie_ctrl, sync_strm + ch, sync_strm2 + ch,
                              type_strm + ch, addr_nm_strm + ch, val_strm + ch));
#else
    // read/write transaction to AI engine via single AXI-MM port.
    aximm_trans_handler<W, ADDR_SHIFT, MWL>(aie_ctrl, sync_strm[ch], sync_strm2[ch], type_strm[ch], addr_nm_strm[ch],
                                            val_strm[ch]);
#endif
    ch = 1;
#ifndef __SYNTHESIS__
    std::cout << "aximm_trans_handler[" << ch << "]...\n";
    // push aximm_trans_handler_th to thread for csim
    vec.push_back(std::thread(aximm_trans_handler_th<W, ADDR_SHIFT, MWL>, &aie_ctrl, sync_strm + ch, sync_strm2 + ch,
                              type_strm + ch, addr_nm_strm + ch, val_strm + ch));
#else
    // read/write transaction to AI engine via single AXI-MM port.
    aximm_trans_handler<W, ADDR_SHIFT, MWL>(aie_ctrl2, sync_strm[ch], sync_strm2[ch], type_strm[ch], addr_nm_strm[ch],
                                            val_strm[ch]);
#endif

#ifndef __SYNTHESIS__
    std::cout << "scheduler...\n";
    // push scheduler to thread for csim
    vec.push_back(std::thread(scheduler<CH_NUM>, sync_strm2, sync_strm, out_buf));
#else
    // handle event list and sync with aximm_trans_hander
    scheduler<CH_NUM>(sync_strm2, sync_strm, out_buf);
#endif

#ifndef __SYNTHESIS__
    // wait all threads done for csim
    for (int i = 0; i < vec.size(); i++) vec[i].join();
#endif

#ifndef __SYNTHESIS__
    std::cout << "pl_controller_hw kernel end\n";
#endif
}
