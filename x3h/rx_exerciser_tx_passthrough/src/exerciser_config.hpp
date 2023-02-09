/*
 * Copyright 2022 Xilinx, Inc.
 * Copyright 2022-2023 Advanced Micro Devices, Inc.
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
#ifndef __XFNETWORKING_EXERCISER_CONFIG__
#define __XFNETWORKING_EXERCISER_CONFIG__
#include <stdint.h>

#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

void write_udp(uint16_t udp_port, hls::stream<ap_axiu<16, 0, 0, 0> >& udp_dest) {
    ap_axiu<16, 0, 0, 0> val;
    val.data = udp_port;
    val.last = 1;
    val.keep = -1;
    udp_dest.write(val);
}

void write_table(unsigned int n_tables, uint16_t* tables, hls::stream<ap_axiu<16, 0, 0, 0> >& table_data) {
TABLE_WRITE_LOOP:
    for (uint32_t i = 0; i < n_tables; i++) {
#pragma HLS pipeline II = 1
        ap_axiu<16, 0, 0, 0> val;
        val.data = tables[i];
        val.keep = -1;
        val.last = (i == n_tables - 1);
        table_data.write(val);
    }
}
void exerciser_config(unsigned int n_tables,
                      uint16_t udp_port,
                      uint16_t* tables,
                      hls::stream<ap_axiu<16, 0, 0, 0> >& table_data,
                      hls::stream<ap_axiu<16, 0, 0, 0> >& udp_dest) {
#pragma HLS interface axis port = table_data
#pragma HLS interface axis port = udp_dest
    write_udp(udp_port, udp_dest);
    write_table(n_tables, tables, table_data);
}
#endif
