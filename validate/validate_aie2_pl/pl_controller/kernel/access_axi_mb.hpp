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
#ifndef PL_CTRL_ACCESS_AXI_MB_HPP
#define PL_CTRL_ACCESS_AXI_MB_HPP
#ifndef __SYNTHESIS__
#include <iostream>
#include <thread>
#include <unistd.h>
#endif
#include "ap_int.h"
#include "hls_stream.h"
#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "utils.hpp"
#include "maxi.hpp"

/**
 * @brief aximm_trans_handler read/write transaction to AI engine via single AXI-MM port
 * @tparam W AIX-MM bit-width
 * @tparam ADDR_SHIFT AIX-MM address shift bit
 * @tparam MWL max burst write latency (from write to write_response)
 * @param aie_ctrl aie AXI-MM interface port to read/write AIE
 * @param i_sync_strm input sync signal from scheduler
 * @param o_sync_strm input sync signal to scheduler
 * @param type_strm input type, 0: end, 1: write, 2: dmaMaskPoll read, 3: dmaGetPendingBd read, 4: event trigger, 5:
 * event wait
 * @param addr_nm_strm input address and data number, addr: (31, 0), nm: (63, 32)
 * @param val_strm input value
 */
template <int W, int ADDR_SHIFT, int MWL>
static void aximm_trans_handler(maxi<ap_uint<W> > aie_ctrl,
                                hls::stream<ap_uint<16> >& i_sync_strm,
                                hls::stream<ap_uint<16> >& o_sync_strm,
                                hls::stream<ap_uint<8> >& type_strm,
                                hls::stream<ap_uint<64> >& addr_nm_strm,
                                hls::stream<ap_uint<W> >& val_strm) {
    const int OFFSET = (1 << ADDR_SHIFT);
#ifndef __SYNTHESIS__
    std::thread::id tid = std::this_thread::get_id();
#endif
    ap_uint<8> type;
LOOP_HANDLER:
    do {
#pragma HLS loop_tripcount min = 100 max = 100
#ifndef __SYNTHESIS__
        while (!type_strm.read_nb(type))
            ;
#else
        type = type_strm.read();
#endif
        // std::cout << "type=" << type << std::endl;
        if (type) {
            if (type == TYPE_WRITE) {
                // write
                bool stop_write = false;
                ap_uint<32> cnt = 0;
                ap_uint<MWL> check = 0;
                ap_uint<MWL> order_check = 0;
                ap_uint<32> nm_w;
                ap_uint<32> addr_w;
            WRITE_LOOP:
                while (!(stop_write && check == 0)) {
#pragma HLS pipeline II = 1
#pragma HLS loop_tripcount min = 100 max = 100
                    bool check_l = check[MWL - 1];
                    check = (check << 1);
                    order_check = (order_check << 1);
                    if ((!stop_write) && (order_check == 0)) {
                        if (cnt == 0) {
                            ap_uint<64> t = 0;
                            if (addr_nm_strm.read_nb(t)) {
                                nm_w = t.range(63, 32);
                                addr_w = t.range(31, 0);
#ifndef __SYNTHESIS__
// std::cout << std::hex << "type=" << type << ", addr=" << addr << ", OFFSET=" << OFFSET
//          << ", nm=" << nm << std::endl;
#endif
                                if (nm_w != 0xFFFF) {
                                    if (nm_w > 1)
                                        // if nm_w > 1, data size must be an integer multiple of W
                                        cnt = nm_w(31, ADDR_SHIFT);
                                    else
                                        cnt = nm_w;
                                    stop_write = (cnt == 0);
                                    if (cnt) {
                                        aie_ctrl.write_request(addr_w(31, ADDR_SHIFT), cnt);
                                    }
                                } else {
                                    order_check = check;
                                }
                            }
                        } else {
                            ap_uint<W> d;
                            if (val_strm.read_nb(d)) {
                                check[0] = (cnt == 1); // if last of burst, put check to wait queue.
                                cnt--;
                                if (nm_w == 1) {
                                    // AXI valid byte flag
                                    ap_uint<W / 8> en = 0xF;
                                    if (ADDR_SHIFT > 0) {
                                        // calcu position for data and en
                                        d <<= addr_w(ADDR_SHIFT - 1, 0) * 32;
                                        en <<= addr_w(ADDR_SHIFT - 1, 0) * 4;
#ifndef __SYNTHESIS__
//                                        std::cout << addr_w << ", " << d(31 + addr_w(ADDR_SHIFT - 1, 0) * 32,
//                                                                         0 + addr_w(ADDR_SHIFT - 1, 0) * 32)
//                                                  << std::endl;
#endif
                                    }
#ifndef __SYNTHESIS__
                                    else {
                                        //                                        std::cout << addr_w << ", " << d(31,
                                        //                                        0) << std::endl;
                                    }
#endif
                                    aie_ctrl.write(d, en);
                                    // printf("write data: %d, en: %d\n", d, en);
                                } else {
#ifndef __SYNTHESIS__
//                                    std::cout << addr_w << ", " << d << std::endl;
#endif
                                    // SET_BD, all bytes valid
                                    ap_uint<W / 8> en = -1;
                                    aie_ctrl.write(d, en);
                                    // printf("write data: %d, en: %d\n", d, en);
                                }
                            }
                        }
                    }
                    if (check_l) {
#ifdef __SYNTHESIS__
                        aie_ctrl.write_response();
#endif
                    }
                }
            } else if (type == TYPE_READ_POLL) {
                // read
                ap_uint<64> addr_nm = addr_nm_strm.read();
                ap_uint<32> addr = addr_nm(31, ADDR_SHIFT);
                ap_uint<32> offset = addr_nm % OFFSET;
                ap_uint<32> mask;
                ap_uint<32> value;
                if (ADDR_SHIFT == 0) {
                    mask = val_strm.read();
                    value = val_strm.read();
                } else {
                    ap_uint<W> tmp = val_strm.read();
                    mask = tmp(31, 0);
                    value = tmp(63, 32);
                }
#ifndef __SYNTHESIS__
//                std::cout << addr << ", " << mask << ", " << value << std::endl;
// std::cout << "type=" << type << ", addr=" << addr << ", offset=" << offset << ", OFFSET=" << OFFSET
//          << ", nm=" << addr_nm(63, 32) << std::endl;
#endif
                bool endFlag = false;
            LOOP_READ1:
                while (!endFlag) {
#pragma HLS pipeline II = 1
#pragma HLS loop_tripcount min = 100 max = 100
                    aie_ctrl.read_request(addr, 1);
                    ap_uint<W> data = aie_ctrl.read();
                    ap_uint<32> val = data(offset * 32 + 31, offset * 32);
                    /* This will check the stalled and start queue size bits to be zero */
                    endFlag = ((val & mask) == value);
#ifndef __SYNTHESIS__
                    endFlag = true;
#endif
                    if (endFlag) o_sync_strm.write(1);
                }
            } else if (type == TYPE_READ_PEND) {
                // read
                ap_uint<64> addr_nm = addr_nm_strm.read();
                ap_uint<32> addr = addr_nm(31, ADDR_SHIFT);
                ap_uint<32> offset = addr_nm % OFFSET;
                ap_uint<32> part_mask;
                ap_uint<32> part_lsb;
                ap_uint<32> mask;
                if (ADDR_SHIFT == 0) {
                    part_mask = val_strm.read();
                    part_lsb = val_strm.read();
                    mask = val_strm.read();
                } else if (ADDR_SHIFT == 1) {
                    ap_uint<W> tmp = val_strm.read();
                    part_mask = tmp(31, 0);
                    part_lsb = tmp(63, 32);
                    tmp = val_strm.read();
                    mask = tmp(31, 0);
                } else {
                    ap_uint<W> tmp = val_strm.read();
                    part_mask = tmp(31, 0);
                    part_lsb = tmp(63, 32);
                    mask = tmp(95, 64);
                }
#ifndef __SYNTHESIS__
//                std::cout << addr << ", " << part_mask << ", " << part_lsb << ", " << mask << std::endl;
// std::cout << "type=" << type << ", addr=" << addr << ", offset=" << offset << ", OFFSET=" << OFFSET
//          << ", nm=" << addr_nm(63, 32) << std::endl;
#endif
                bool endFlag = false;
            LOOP_READ2:
                while (!endFlag) {
#pragma HLS pipeline II = 1
#pragma HLS loop_tripcount min = 100 max = 100
                    aie_ctrl.read_request(addr, 1);
                    ap_uint<W> data = aie_ctrl.read();
                    ap_uint<32> val = data(offset * 32 + 31, offset * 32);
                    ap_uint<32> t_sz = ((val & part_mask) >> part_lsb);
                    if (val & mask) t_sz++;
                    /* This will check the stalled and start queue size bits to be zero */
                    endFlag = (t_sz <= 3);
#ifndef __SYNTHESIS__
                    endFlag = true;
#endif
                    if (endFlag) o_sync_strm.write(1);
                }
            } else if (type == TYPE_EVENT_TRIGGER) {
                ap_uint<32> val = val_strm.read();
                o_sync_strm.write(val(15, 0));  // Event ID
                o_sync_strm.write(val(31, 16)); // trigger AXI-AIE channel flag
            } else if (type == TYPE_EVENT_WAIT) {
                // send sync flag to scheduler
                ap_uint<16> wait_event_id = val_strm.read();
                ap_uint<16> sync_event_id;
            LOOP_SYNC:
                // wait sync flag
                while (!i_sync_strm.read_nb(sync_event_id)) {
#ifndef __SYNTHESIS__
                    sleep(1);
                    std::cout << "wait sync flag: tid=" << tid << std::endl;
#endif
                }
            }
        }
        //}
    } while (type);
    o_sync_strm.write(0);
}

/**
 * @brief scheduler sync with aximm_trans_hander
 * @tparam CH number of AIX-MM
 * @param i_sync_strm input sync signal from aximm_trans_handler
 * @param o_sync_strm input sync signal to aximm_trans_handler
 * @param out_buf output information (todo for debugging)
 */
template <int CH>
void scheduler(hls::stream<ap_uint<16> > i_sync_strm[CH],
               hls::stream<ap_uint<16> > o_sync_strm[CH],
               ap_uint<32>* out_buf) {
    ap_uint<CH> end_flag = -1;
    ap_uint<16> sync_event;
    while (end_flag) {
        for (int ch = 0; ch < CH; ch++) {
#ifndef __SYNTHESIS__
//            std::cout << "ch=" << ch << ", end_flag=" << end_flag << std::endl;
#endif
            if (end_flag[ch]) {
                if (i_sync_strm[ch].read_nb(sync_event)) {
#ifndef __SYNTHESIS__
                    std::cout << "sync_event[" << ch << "]=" << sync_event << std::endl;
#endif
                    if (sync_event == 0) // channal end
                        end_flag[ch] = 0;
                    else if (sync_event == 1) // read done flag
                        continue;
                    else {
#ifndef __SYNTHESIS__
                        while (i_sync_strm[ch].empty())
                            ;
#endif
                        ap_uint<16> sync_ch = i_sync_strm[ch].read();
                        for (int i = 0; i < CH; i++) {
                            if (sync_ch[i]) {
                                o_sync_strm[i].write(sync_event);
                            }
                        }
                    }
                }
            }
        }
    }
    out_buf[0] = 1;
}

#ifndef __SYNTHESIS__
// aximm_trans_handler wrap for csim
template <int W, int ADDR_SHIFT, int MWL>
static void aximm_trans_handler_th(maxi<ap_uint<W> >* aie_ctrl,
                                   hls::stream<ap_uint<16> >* i_sync_strm,
                                   hls::stream<ap_uint<16> >* o_sync_strm,
                                   hls::stream<ap_uint<8> >* type_strm,
                                   hls::stream<ap_uint<64> >* addr_nm_strm,
                                   hls::stream<ap_uint<W> >* val_strm) {
    aximm_trans_handler<W, ADDR_SHIFT, MWL>(*aie_ctrl, *i_sync_strm, *o_sync_strm, *type_strm, *addr_nm_strm,
                                            *val_strm);
}
#endif

#endif
