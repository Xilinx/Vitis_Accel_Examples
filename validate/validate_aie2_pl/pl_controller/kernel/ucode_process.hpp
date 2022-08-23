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
#ifndef PL_CONTROLLER_UCODE_PROCESS_HPP
#define PL_CONTROLLER_UCODE_PROCESS_HPP

#include "utils.hpp"
/**
 * @brief ucode_reader read ucode from DDR to stream
 * @param size_strm ucode size
 * @param ucode ucode data
 * @param ucode_strm output ucode stream
 */
inline void ucode_reader(hls::stream<ap_uint<32> >& size_strm,
                         ap_uint<32>* ucode_buf,
                         hls::stream<ap_uint<33> >& ucode_strm) {
    ap_uint<33> data = 0;
    ap_uint<32> size = size_strm.read();
loop_reader:
    for (ap_uint<32> i = 0; i < size; i++) {
#pragma HLS pipeline ii = 1
        data(31, 0) = ucode_buf[i];
        ucode_strm << data;
    }
    data[32] = 1;
    ucode_strm << data;
}

/**
 * @brief ucode_decoder decode ucode, and dispatch them to aximm_trans_handler, see
 * https://confluence.xilinx.com/display/XSW/Ucode+Format
 * @tparam W AIX-MM bit-width
 * @tparam CH number of AIX-MM
 * @param rt_addr_buff runtime address
 * @param i_ucode_strm input ucode stream
 * @param o_type_strm output type, 0: end, 1: write, 2: dmaMaskPoll read, 3: dmaGetPendingBd read, 4: event trigger, 5:
 * event wait
 * @param o_addr_nm_strm output address and data number, addr: (31, 0), nm: (63, 32)
 * @param o_val_strm output value
 */
template <int W = 32, int CH>
void ucode_decoder(ap_uint<64>* rt_addr_buff,
                   hls::stream<ap_uint<33> >& i_ucode_strm,
                   hls::stream<ap_uint<8> > o_type_strm[CH],
                   hls::stream<ap_uint<64> > o_addr_nm_strm[CH],
                   hls::stream<ap_uint<W> > o_val_strm[CH]) {
    const int nw = W / 32;
    ap_uint<CH> last_is_wr = 0;
    ap_uint<33> opcode = i_ucode_strm.read();
    while (!opcode[32]) {
        ap_uint<6> state = opcode(5, 0);
        bool order = opcode[6];
        ap_uint<4> chan = opcode(10, 7);
        bool rt = opcode[11];
        ap_uint<8> rtid = opcode(19, 12);

        ap_uint<64> bd_addr = rt_addr_buff[rtid];
        ap_uint<64> addr_nm;
        ap_uint<W> val;

        switch (state) {
            case SET_BD: {
                if (!last_is_wr[chan]) {
                    // if last opteration no write
                    o_type_strm[chan].write(TYPE_WRITE);
                } else if (order) {
                    // if need ordered write
                    o_addr_nm_strm[chan].write(0); // exit manual burst write
                    o_type_strm[chan].write(TYPE_WRITE);
                }
                ap_uint<32> addr = i_ucode_strm.read();
                ap_uint<32> nm = i_ucode_strm.read();
                addr_nm(31, 0) = addr >> ADDR_ALIGN_SHIFT;
                addr_nm(63, 32) = nm;
                o_addr_nm_strm[chan].write(addr_nm);
                for (int i = 0; i < nm; i++) {
                    ap_uint<32> data = i_ucode_strm.read();
                    if (rt && i == 1) {
                        ap_uint<32> bd_addr32 = (bd_addr >> NOC_MODULE_DMA_BD0_1_BASE_ADDRESS_LOW_LSB);
                        data = ((bd_addr32 << NOC_MODULE_DMA_BD0_1_BASE_ADDRESS_LOW_LSB) &
                                NOC_MODULE_DMA_BD0_1_BASE_ADDRESS_LOW_MASK);
                    } else if (rt && i == 2) {
                        ap_uint<32> bd_addr32 = bd_addr(63, 32);
                        ap_uint<32> val = ((bd_addr32 << NOC_MODULE_DMA_BD0_2_BASE_ADDRESS_HIGH_LSB) &
                                           NOC_MODULE_DMA_BD0_2_BASE_ADDRESS_HIGH_MASK);
                        data = val | data;
                    }
                    int im = i % nw;
                    // Splice nw 32bit data as a W bit value, and write into o_val_strm
                    val(32 * im + 31, 32 * im) = data;
                    if (im + 1 == nw) o_val_strm[chan].write(val);
                }
                last_is_wr[chan] = 1;
                break;
            }
            case WRITE_MM:
            case ENQ_BD:
            case LOCK_INIT: {
                if (!last_is_wr[chan]) {
                    // if last opteration no write
                    o_type_strm[chan].write(TYPE_WRITE);
                } else if (order) {
                    // if need ordered write
                    o_addr_nm_strm[chan].write(0xFFFFFFFF); // flag to ordered write
                    o_type_strm[chan].write(TYPE_WRITE);
                }
                ap_uint<32> addr = i_ucode_strm.read();
                val(31, 0) = i_ucode_strm.read();
                addr_nm(31, 0) = addr >> ADDR_ALIGN_SHIFT;
                addr_nm(63, 32) = LOCK_INIT_ENQ_BD_NM;
                o_addr_nm_strm[chan].write(addr_nm);
                o_val_strm[chan].write(val);
                last_is_wr[chan] = 1;
                break;
            }
            case DMA_WAIT:
            case ACQ_LOCK:
            case REL_LOCK: {
                if (last_is_wr[chan]) {
                    // if last opteration is write, exit manual burst write
                    o_addr_nm_strm[chan].write(0);
                }
                o_type_strm[chan].write(TYPE_READ_POLL);
                ap_uint<32> addr = i_ucode_strm.read();
                addr_nm(31, 0) = addr >> ADDR_ALIGN_SHIFT;
                addr_nm(63, 32) = ACQ_LOCK_NM;
                o_addr_nm_strm[chan].write(addr_nm);
                // Splice nw 32bit data as a W bit value, and write into o_val_strm
                if (nw == 1) {
                    val(31, 0) = i_ucode_strm.read(); // mask
                    o_val_strm[chan].write(val);
                    val(31, 0) = i_ucode_strm.read(); // value
                    o_val_strm[chan].write(val);
                } else {
                    val(31, 0) = i_ucode_strm.read();  // mask
                    val(63, 32) = i_ucode_strm.read(); // value
                    o_val_strm[chan].write(val);
                }
                last_is_wr[chan] = 0;
                break;
            }
            case DMA_WAIT_QUE: {
                if (last_is_wr[chan]) {
                    // if last opteration is write, exit manual burst write
                    o_addr_nm_strm[chan].write(0);
                }
                o_type_strm[chan].write(TYPE_READ_PEND);
                ap_uint<32> addr = i_ucode_strm.read();
                addr_nm(31, 0) = addr >> ADDR_ALIGN_SHIFT;
                addr_nm(63, 32) = DMA_WAIT_QUE_NM;
                o_addr_nm_strm[chan].write(addr_nm);
                // Splice nw 32bit data as a W bit value, and write into o_val_strm
                if (nw == 1) {
                    val(31, 0) = i_ucode_strm.read(); // taskqsize_mask
                    o_val_strm[chan].write(val);
                    val(31, 0) = i_ucode_strm.read(); // taskqsie_lsb
                    o_val_strm[chan].write(val);
                    val(31, 0) = i_ucode_strm.read(); // mask
                    o_val_strm[chan].write(val);
                } else if (nw == 2) {
                    val(31, 0) = i_ucode_strm.read();
                    val(63, 32) = i_ucode_strm.read();
                    o_val_strm[chan].write(val);
                    val(31, 0) = i_ucode_strm.read();
                    o_val_strm[chan].write(val);
                } else {
                    val(31, 0) = i_ucode_strm.read();
                    val(63, 32) = i_ucode_strm.read();
                    val(95, 64) = i_ucode_strm.read();
                    o_val_strm[chan].write(val);
                }
                last_is_wr[chan] = 0;
                break;
            }
            case EVENT_TRIGGER: {
                if (last_is_wr[chan]) {
                    // if last opteration is write, exit manual burst write
                    o_addr_nm_strm[chan].write(0);
                }
                o_type_strm[chan].write(TYPE_EVENT_TRIGGER);
                val(31, 0) = i_ucode_strm.read();
                o_val_strm[chan].write(val);
                last_is_wr[chan] = 0;
                break;
            }
            case EVENT_WAIT: {
                if (last_is_wr[chan]) {
                    // if last opteration is write, exit manual burst write
                    o_addr_nm_strm[chan].write(0);
                }
                o_type_strm[chan].write(TYPE_EVENT_WAIT);
                val(31, 0) = i_ucode_strm.read();
                o_val_strm[chan].write(val);
                last_is_wr[chan] = 0;
                break;
            }
            case WRITE_MMS: {
                if (!last_is_wr[chan]) {
                    // if last opteration no write
                    o_type_strm[chan].write(TYPE_WRITE);
                } else if (order) {
                    // if need ordered write
                    o_addr_nm_strm[chan].write(0xFFFFFFFF); // flag to ordered write
                    o_type_strm[chan].write(TYPE_WRITE);
                }
                ap_uint<32> addr = i_ucode_strm.read();
                ap_uint<32> nm = i_ucode_strm.read();
                addr_nm(31, 0) = addr >> ADDR_ALIGN_SHIFT;
                addr_nm(63, 32) = nm;
                o_addr_nm_strm[chan].write(addr_nm);
                for (int i = 0; i < nm; i++) {
                    ap_uint<32> data = i_ucode_strm.read();
                    int im = i % nw;
                    val(32 * im + 31, 32 * im) = data;
                    if (im + 1 == nw) o_val_strm[chan].write(val);
                }
                last_is_wr[chan] = 1;
                break;
            }
            case HALT:
            default: {
                // exit all handler
                for (int ch = 0; ch < CH; ch++) {
                    if (last_is_wr[ch]) {
                        o_addr_nm_strm[ch].write(0);
                    }
                    o_type_strm[ch].write(TYPE_END);
                }
                last_is_wr[chan] = 0;
                break;
            }
        }
        opcode = i_ucode_strm.read();
    }
}
#endif
