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
#ifndef PACKET_FILTER_SC_HPP
#define PACKET_FILTER_SC_HPP

#include <iostream>
#include <string>
#include <stdint.h>
#include <ap_int.h>
#include "ap_axi_sdata.h"

#include "vpp_acc.hpp"
#include "vpp_stream.hpp"

#include "net_utils.hpp"

enum ipProtocol {PRT_NONE = 0, TCP, UDP, ARP, ICMP, PRT_ALL};
enum ipTgtAction {IPT_LOG = 0, IPT_ACCEPT, IPT_REJECT, IPT_DROP};
enum matchFlag {M_NONE = 0,
                M_SRCIP = 1,
                M_DSTIP = 1 << 1,
                M_SRCPORT = 1 << 2,
                M_DSTPORT = 1 << 3,
                M_PROTOCOL = 1 << 4};

enum IPTCommand {IPT_RULE_UPDATE = 1, IPT_RULE_INSERT, IPT_RULE_DELETE, IPT_RULE_READBACK, IPT_PAUSE, IPT_RESUME,
                 IPT_INIT_DATA, IPT_GET_STATS, IPT_SET_DEFAULTS};
template <class T>
using ApGenericStream = vpp::stream<T, 32>;
using ApDataStream = vpp::stream<X3Data, 32>;

template <int DWIDTH>
using ApUintStream = vpp::stream<ap_uint<DWIDTH>, 2>;
template <int DWIDTH>
using AxiWordStream = vpp::stream<AXIWord_dt<DWIDTH>, 2>;

struct __attribute__((packed)) IPv4InfoPack_dt{
    ap_uint<32> src_ip_address;
    ap_uint<32> dst_ip_address;
    ap_uint<16> src_ip_port;
    ap_uint<16> dst_ip_port;
    ap_uint<4> protocol;
    ap_uint<16> frame_size;
    ap_uint<1> is_valid_frame;
};

template <class T>
using ApDsStream = vpp::stream<T, 2>;


#ifdef SW_EMU_TEST
template <int dm = 0>
std::string getIPString(uint32_t ip) {
    std::string ips{""};
    auto pfx = ".";
    for (uint8_t i = 0; i < 4; ++i) {
        if (i == 3) pfx = "";
        uint32_t tmp = ((ip >> (i * 8)) & 0xFF);
        ips.append(std::to_string(tmp) + pfx);
    }
    return ips;
}

template <int RULE_BITS = 192>
void printRule(ap_uint<RULE_BITS> currRule) {
	printf("--------------------\n");
	printf("default rule: %u, interface: %u, protocol: %u, target: %u\n",
			(uint8_t)currRule.range(0, 0), (uint8_t)currRule.range(3, 1),
			(uint8_t)currRule.range(7,4), (uint8_t)currRule.range(30, 27));
	printf("srcIPMask: %u, dstIPMask: %u\n", (uint8_t)currRule.range(13, 8), (uint8_t)currRule.range(19, 14));
	std::cout << "srcIP: " << getIPString((uint32_t)currRule.range(64 + 32 - 1, 64)) << ", dstIP: "
			<< getIPString((uint32_t)currRule.range(96 + 32 - 1, 96)) << std::endl;
	printf("srcPort: %u, dstPort: %u\n", (uint16_t)currRule.range(143, 128), (uint16_t)currRule.range(175, 160));
}
#endif

#endif // PACKET_FILTER_HPP
