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

#ifndef _XF_PLCTR_DMA_CTRL_HPP_
#define _XF_PLCTR_DMA_CTRL_HPP_

#include "ctrl_pkt_utils.hpp"
namespace xf {
namespace plctrl {
/**Set DMA buffer description length
 *
 */
static void setDmaBd(hls::stream<ap_axiu<32, 0, 0, 0> >& ctrlOut,
                     ap_uint<32> ctrlPktID,
                     ap_uint<32> dma_bd_number,
                     ap_uint<32> dma_bd_value) {
#pragma HLS inline
    ap_uint<32> dma_bd_ctrl_addr;
    switch (dma_bd_number) {
        case 0: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD0CTRL;
            break;
        };
        case 1: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD1CTRL;
            break;
        };
        case 2: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD2CTRL;
            break;
        };
        case 3: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD3CTRL;
            break;
        };
        case 4: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD4CTRL;
            break;
        };
        case 5: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD5CTRL;
            break;
        };
        case 6: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD6CTRL;
            break;
        };
        case 7: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD7CTRL;
            break;
        };
        case 8: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD8CTRL;
            break;
        };
        case 9: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD9CTRL;
            break;
        };
        case 10: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD10CTRL;
            break;
        };
        case 11: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD11CTRL;
            break;
        };
        case 12: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD12CTRL;
            break;
        };
        case 13: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD13CTRL;
            break;
        };
        case 14: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD14CTRL;
            break;
        };
        case 15: {
            dma_bd_ctrl_addr = XAIEGBL_MEM_DMABD15CTRL;
            break;
        };
        default:
#ifndef __HLS_SYNTHESIS__
            printf("setDmaBdLength(): ERROR: Unknown BD number: %d\n", (unsigned int)dma_bd_number);
#endif
            break;
    }
    // write new value to register
    controlPacketWrite(ctrlOut, ctrlPktID, dma_bd_ctrl_addr, 1, dma_bd_value);
}
/**
 * TBD
 */
static void enqueueDmaBd(hls::stream<ap_axiu<32, 0, 0, 0> >& ctrlOut,
                         ap_uint<32> ctrlPktID,
                         ap_uint<32> dma_bd_number,
                         ap_uint<32> dma_channel,
                         bool dma_s2mm) {
#pragma HLS inline

    ap_uint<32> reg_start_enqueue;
    if (dma_channel == 0) {
        reg_start_enqueue = dma_s2mm ? XAIEGBL_MEM_DMAS2MM0STAQUE : XAIEGBL_MEM_DMAMM2S0STAQUE;
    } else {
        reg_start_enqueue = dma_s2mm ? XAIEGBL_MEM_DMAS2MM1STAQUE : XAIEGBL_MEM_DMAMM2S1STAQUE;
    }
    controlPacketWrite(ctrlOut, ctrlPktID, reg_start_enqueue, 1, dma_bd_number);
};

} // end of namespace plctrl
} // end of namespace xf
#endif
