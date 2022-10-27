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

#include "exerciser_config.hpp"

extern "C" {
void exerciser_config_rx(unsigned int n_tables,
                         uint16_t udp_port,
                         uint16_t* tables,
                         hls::stream<ap_axiu<16, 0, 0, 0> >& table_data,
                         hls::stream<ap_axiu<16, 0, 0, 0> >& udp_dest) {
    exerciser_config(n_tables, udp_port, tables, table_data, udp_dest);
}
}
