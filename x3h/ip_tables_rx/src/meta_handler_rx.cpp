/*
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
#include "meta_handler_rx.hpp"

namespace rxHandler {
    static void mac_meta_handler_rx(hls::stream<AxiX3RxData<0> >& mac2ulpData,
                                    RxPktStream& ulpInData) {
        mac_handler_rx(mac2ulpData, ulpInData);
    }
    
    static void nic_meta_handler_rx(RxPktStream& ulpOutData,
                                    hls::stream<AxiX3RxData<0> >& ulp2nicData) {
        nic_handler_rx(ulpOutData, ulp2nicData);
    }
}
