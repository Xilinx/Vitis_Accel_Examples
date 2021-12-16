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

#ifndef _XF_PLCTRL_CTRL_PKT_UTILS_HPP_
#define _XF_PLCTRL_CTRL_PKT_UTILS_HPP_

#include "hls_stream.h"
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "drivers/aiengine/xaiengine/xaiegbl_params.h"
#ifndef __HLS_SYNTHESIS__
#include <stdio.h>
#endif

namespace xf {
namespace plctrl {
/** Generate packet header.
 *
 */
static ap_uint<32> generateHeader(unsigned int pktType, unsigned int pktID) {
#pragma HLS inline
    ap_uint<32> header = 0;
    // Packet ID
    header(4, 0) = pktID;
    // 7'b0000000
    header(11, 5) = 0;
    // Packet Type
    header(14, 12) = pktType;
    // 1'b0
    header[15] = 0;
    // source row
    header(20, 16) = -1;
    // source column
    header(27, 21) = -1;
    // 3'b00
    header(30, 28) = 0;
    // Odd parity of bits[30:0]
    header[31] = header(30, 0).xor_reduce() ? (ap_uint<1>)0 : (ap_uint<1>)1;
    return header;
}

/**Generate control header
 *
 */
static ap_uint<32> generateControlHeader(ap_uint<32> localByteAddress,
                                         unsigned int numOfBeats,
                                         unsigned int operation,
                                         unsigned int returnStrmID = 0) {
#pragma HLS inline
    ap_uint<32> header = 0;
    // Local Byte Address
    header(17, 0) = localByteAddress;
    // Number of data beats(1-4)
    header(19, 18) = numOfBeats;
    // operation
    // 00: write without return packet
    // 01: read with return packet
    // 10: write with return packet
    // 11: reserved
    header(21, 20) = operation;
    // reserved 2'b00
    header(23, 22) = 0;
    // stream ID for return packet
    header(28, 24) = returnStrmID;
    // reserved 2'b00
    header(30, 29) = 0;
    // Odd parity on bits[30:0]
    header[31] = header(30, 0).xor_reduce() ? (ap_uint<1>)0 : (ap_uint<1>)1;
    return header;
}

/** write control packet
 *
 */
static void controlPacketWrite(hls::stream<ap_axiu<32, 0, 0, 0> >& ctrlOut,
                               ap_uint<32> ctrlPktID,
                               ap_uint<32> addr,
                               ap_uint<32> pldNum,
                               ap_uint<32> pld0,
                               ap_uint<32> pld1 = 0,
                               ap_uint<32> pld2 = 0,
                               ap_uint<32> pld3 = 0) {
#pragma HLS inline
    ap_axiu<32, 0, 0, 0> tmp;
    tmp.keep = -1;
    tmp.last = 0;
    // Write packet header
    ap_uint<32> pktHeader = generateHeader(/*PKT_TYPE*/ 0, ctrlPktID);
    tmp.data = pktHeader;
    ctrlOut.write(tmp);
    ap_uint<32> beats = pldNum - 1;
    // write control packet header
    ap_uint<32> ctrlPktHeader = generateControlHeader(addr, beats, 0x0 /*write without response*/);
    tmp.data = ctrlPktHeader;
    ctrlOut.write(tmp);
    // write payload
    for (unsigned int i = 0; i < pldNum; ++i) {
        if (i == 0)
            tmp.data = pld0;
        else if (i == 1)
            tmp.data = pld1;
        else if (i == 2)
            tmp.data = pld2;
        else
            tmp.data = pld3;

        if (i == pldNum - 1)
            tmp.last = 1;
        else
            tmp.last = 0;
        ctrlOut.write(tmp);
    }
}
/**write control packet
 *
 */
static void controlPacketBlockWrite(hls::stream<ap_axiu<32, 0, 0, 0> >& ctrlOut,
                                    ap_uint<32> ctrlPktID,
                                    ap_uint<32> addr,
                                    ap_uint<32> pldNum,
                                    ap_uint<32>* pldPtr) {
    for (unsigned int i = 0; i < pldNum; i += 4) {
        // payload cut into 4 words and sent to stream
        ap_uint<32> num_words;
        if (((i + 4) > pldNum))
            num_words = pldNum - i;
        else
            num_words = 4;
        controlPacketWrite(ctrlOut, ctrlPktID, addr + 4 * i, num_words, pldPtr[i], pldPtr[i + 1], pldPtr[i + 2],
                           pldPtr[i + 3]);
    }
}
} // end of namespace plctrl
} // end of namespace xf
#endif
