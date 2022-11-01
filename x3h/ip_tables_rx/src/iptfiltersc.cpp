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

#ifndef IPT_FILTER_SC_CPP
#define IPT_FILTER_SC_CPP

#include <iostream>
#include <string>
#include <stdint.h>
#include <ap_int.h>
#include "ap_axi_sdata.h"

#include "x3_specs.hpp"
#include "net_utils.hpp"
#include "packet_filter_sc.hpp"

#ifndef RULEWORD_BITS
#define RULEWORD_BITS 192
#endif

#ifndef IPRULE_TABLE_SIZE
#define IPRULE_TABLE_SIZE 64
#endif

namespace IPTablesSC {
    // Inter-kernel control streams
    static ApUintStream<8> commandStream("commandStream");
    static AxiWordStream<RULEWORD_BITS> ruleUpdateStream("ruleUpdateStream");
    static AxiWordStream<RULEWORD_BITS> ruleReadbackStream("ruleReadbackStream");
    // ip-port-protocol streams
    static ApUintStream<1> ethValidStream("ethValidStream");
    static ApDsStream<IPv4InfoPack_dt> ipInfoStream("ipInfoStream");
    static ApUintStream<4> actionStream("actionStream");
    
    static void controlUnit(ap_uint<192> *ruleIn,
                            ap_uint<192> *ruleOut,
                            uint32_t cmd,
                            ApUintStream<8> &commandStream,
                            AxiWordStream<RULEWORD_BITS> &ruleUpdateStream,
                            AxiWordStream<RULEWORD_BITS> &ruleReadbackStream) {
        // produce test data streams for ipTables Filter kernel
        // Input and output data format
        // <ruleCnt><ruleData>
        
        const unsigned rwLim = ((RULEWORD_BITS - 1)/ 32) + 1;
        
        // send command
        ap_uint<8> outCmdVal = cmd;
        commandStream << outCmdVal;
    #ifdef SW_EMU_TEST
        std::cout << "RU: Got command: " << cmd << std::endl;
    #endif
        if (cmd == uint32_t(IPT_RULE_UPDATE) || cmd == uint32_t(IPT_RULE_INSERT) || cmd == uint32_t(IPT_RULE_DELETE)) {
            // write updated iptable rules to ruleTableStream
            uint32_t ruleCnt = ruleIn[0].range(31, 0);   // Rule count, i.e, number of rules to update/insert/delete
            uint32_t s_idx = ruleIn[0].range(63,32);     // starting index
            AXIWord_dt<RULEWORD_BITS> ruleWord;
    #ifdef SW_EMU_TEST
            std::cout << "RU: Number of Rules: " << ruleCnt << std::endl;
    #endif
            // send rule count and starting index first
            ruleWord.last = (cmd == uint32_t(IPT_RULE_DELETE));
            ruleWord.data.range(31, 0) = s_idx;
            ruleWord.data.range(63, 32) = ruleCnt;
            ruleUpdateStream << ruleWord;
            // write table stream, send extra rule with value 0 for efficient matching state machine
        READ_RULES_M2S:
            for (uint32_t i = 0; (cmd != uint32_t(IPT_RULE_DELETE)) && (i < ruleCnt); ++i) {
    #pragma HLS PIPELINE II = 1
                ruleWord.last = (i == ruleCnt - 1);
                ruleWord.data = ruleIn[i+1];
                ruleWord.keep = -1;
                ruleUpdateStream << ruleWord;
                // std::cout << "RU: Sent Rule: " << i << std::endl;
            }
        } else if (cmd == uint32_t(IPT_RULE_READBACK)) {
            // read the rules back to DDR from read stream
            // first read from stream tells the rule count
            uint32_t idx = 0;
            AXIWord_dt<RULEWORD_BITS> ruleVal;
            ruleVal.last = 0;
            // read data into memory
            READ_RULES_S2M:
            while (1) {
                ruleVal = ruleReadbackStream.read();
                ruleOut[idx++] = ruleVal.data;
                if (ruleVal.last) break;
                // std::cout << "RU: Read Rule: " << i << std::endl;
            }
    #ifdef SW_EMU_TEST
            std::cout << "RU: Number of Rules to readback: " << (idx - 1)/6 << std::endl;
    #endif
        } else if (cmd == uint32_t(IPT_GET_STATS)) {
            // read the stats back to DDR from read stream
            // first read from stream tells the rule count
            uint32_t idx = 0;
            AXIWord_dt<RULEWORD_BITS> outVal;
            outVal.last = 0;
            // read data into memory
            READ_STATS_S2M:
            while (1) {
    #pragma HLS PIPELINE II = 1
                outVal = ruleReadbackStream.read();
                ruleOut[idx++] = outVal.data.range(31, 0);
                // std::cout << "RU: Read Stats: " << i << std::endl;
                if (outVal.last) break;
            }
    #ifdef SW_EMU_TEST
            std::cout << "RU: Number of Values to readback: " << idx - 1 << std::endl;
    #endif
        } else if (cmd == uint32_t(IPT_SET_DEFAULTS)) {
            // send the config for default rule or unparseable packets
            AXIWord_dt<RULEWORD_BITS> ruleWord;
            ruleWord.last = 1;
            ruleWord.data = ruleIn[0];
            ruleUpdateStream << ruleWord;
        }
    #ifdef SW_EMU_TEST
        std::cout << "RU: Finished !!" << std::endl;
    #endif
    }
    
    template <typename T>
    static void ethernetParser(T &ulpInStream,
                               ApDataStream &ipInDataStream,
                               ApUintStream<1> &ethValidStream,
                               T &inPacketStream) {
        // parse ethernet frame and pass rest of the packet data
        constexpr uint16_t c_ipv4_ether_type = 0x0800;
        uint16_t in_cnt = 0;
        ap_uint<1> eth_valid = 1;
        bool last = false;
        X3Data ip_out_val;
        ap_uint<X3_DWIDTH * 2> out_reg = 0;
    ETH_PARSER:
        while (!last) {
    #pragma HLS PIPELINE II = 1
            auto inVal = ulpInStream.read();
            inPacketStream << inVal;
            last = inVal.last;
            ap_uint<32> ltEndianWord = netOrder(inVal.data);
            ++in_cnt;
            if (in_cnt == 4) {
                // get ip version and IHL (reversed nibbles), verify correct packet
                out_reg.range(15, 0) = inVal.data.range(31, 16);
                uint16_t eth_type = ltEndianWord.range(31, 16);
                // valid if type is IPV4
                eth_valid = ((eth_type == c_ipv4_ether_type)? 1 : 0);
                ethValidStream << eth_valid;
            } else if (in_cnt > 4 && in_cnt < 12 && eth_valid) {
                out_reg.range(47, 16) = inVal.data;
                ip_out_val.user = 0;
                ip_out_val.data = out_reg.range(31, 0);
                ip_out_val.keep = -1;
                ip_out_val.last = (in_cnt == 11);   // only send data relevant to IPV4 header parsing
                ipInDataStream << ip_out_val;
                out_reg >>= 32;
            }
        }
    }
    
    static void ipv4Parser(ApDataStream &ipInDataStream,
                           ApUintStream<1> &ethValidStream,
                           ApDsStream<IPv4InfoPack_dt> &ipInfoStream) {
    // parse IPV4 header and TCP/UDP first word to get src/dst ports
        constexpr uint8_t c_tcp_num = 6;
        constexpr uint8_t c_udp_num = 17;
        IPv4InfoPack_dt ip_info;
        uint16_t in_cnt = 0;
        bool last = false;
        auto ip_valid = ethValidStream.read();
        bool write_info = false;
        if (ip_valid == 0) {
            // Not an IP packet
            ip_info.frame_size = 0;
            ip_info.protocol = static_cast<ap_uint<4> >(PRT_NONE);
            ip_info.is_valid_frame = ip_valid;
            ipInfoStream << ip_info;
        } else {
        IPV4_PARSER:
            while (!last) {
    #pragma HLS PIPELINE II = 1
                auto inVal = ipInDataStream.read();
                last = inVal.last;
                ap_uint<32> ltEndianWord = netOrder(inVal.data);
                ++in_cnt;
                if (in_cnt == 1) {
                    // get ip version and IHL (reversed nibbles), verify correct packet
                    uint8_t ip_header_len = inVal.data.range(3, 0) & 0xF;
                    uint8_t ip_version = inVal.data.range(7, 4) & 0xF;
                    ap_uint<16> pkt_size  = ltEndianWord.range(15, 0);
                    // set invalid packet if invalid version or header length
                    if (ip_version != 4 || ip_header_len < 5) ip_valid = 0;
                    ip_info.frame_size = pkt_size;
                } else if (in_cnt == 3) {
                    // get protocol, verify supported protocol
                    ap_uint<4> ip_protocol = 0;
                    uint8_t ip_prtcl = inVal.data.range(15, 8);
                    if (ip_prtcl == c_tcp_num) ip_protocol = static_cast<ap_uint<4> >(TCP);
                    if (ip_prtcl == c_udp_num) ip_protocol = static_cast<ap_uint<4> >(UDP);
                    if (ip_prtcl != c_tcp_num && ip_prtcl != c_udp_num) ip_valid = 0;
                    // write protocol and ip_valid
                    ip_info.protocol = ip_protocol;
                    ip_info.is_valid_frame = ip_valid;
                    write_info = (ip_valid == 0);
                } else if (in_cnt == 4) {
                    // src IP address, in big endian, used like that only
                    ip_info.src_ip_address = inVal.data;
                } else if (in_cnt == 5) {
                    // dst IP address, in big endian, used like that only
                    ip_info.dst_ip_address = inVal.data;
                } else if (in_cnt == 6) {
                    // src & dst IP ports in TCP/UDP header, convert to little endian
                    ip_info.src_ip_port = ltEndianWord.range(31, 16);
                    ip_info.dst_ip_port = ltEndianWord.range(15, 0);
                    write_info = (bool)ip_valid;
                }
                if (write_info) {
                    ipInfoStream << ip_info;
                    write_info = false;
                }
            }// ipv4_parser loop end
        }
    }
    
    static void ruleManager(ApUintStream<8> &commandStream,
                            ApDsStream<IPv4InfoPack_dt> &ipInfoStream,
                            AxiWordStream<RULEWORD_BITS> &ruleUpdateStream,
                            AxiWordStream<RULEWORD_BITS> &ruleReadbackStream,
                            ApUintStream<4> &actionStream) {
        // Manage rules, matching and logging
        constexpr uint32_t c_bitMask[33] = {
           0x0, 0x1,       0x3,       0x7,       0xF,       0x1F,       0x3F,               0x7F,       0xFF,
                0x1FF,     0x3FF,     0x7FF,     0xFFF,     0x1FFF,     0x3FFF,     0x7FFF,     0xFFFF,
                0x1FFFF,   0x3FFFF,   0x7FFFF,   0xFFFFF,   0x1FFFFF,   0x3FFFFF,   0x7FFFFF,   0xFFFFFF,
                0x1FFFFFF, 0x3FFFFFF, 0x7FFFFFF, 0xFFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF};
    #pragma HLS ARRAY_PARTITION variable = c_bitMask complete
        // execution states
        static bool is_searching = false;
        static bool is_reading_rules = false;
        static bool is_updating_rules = false;
        static bool is_insert_rules = false;
        static bool is_delete_rules = false;
        static bool is_reading_stats = false;
        static bool is_set_defaults = false;
        static bool read_cmd = true;
        // intialize memories in module
        static bool is_init = true;
        // rule table
        static uint32_t table_size = 0;
        static ap_uint<RULEWORD_BITS> ruleTable[IPRULE_TABLE_SIZE + 1];
    #pragma HLS BIND_STORAGE variable = ruleTable type = ram_t2p impl = bram
        
        // default action configs
        static ipTgtAction invalid_pkt_action = IPT_DROP;
        static ipTgtAction default_pkt_action = IPT_DROP;
        // stats
        static ap_uint<64> rule_m_count[IPRULE_TABLE_SIZE * 2];    // <packet_count 64-bit><byte_count 64-bit>..matched
        static ap_uint<64> default_count[2];    // <packet_count 64-bit><byte_count 64-bit>..matched
        static ap_uint<64> invalid_count[2];    // <packet_count 64-bit><byte_count 64-bit>..matched
        static ap_uint<64> total_count[4];      // <<packet_count 64-bit><byte_count 64-bit>..accepted>..dropped
        // Variables for state machines
        // rule management
        static IPTCommand c_ipt_cmd = IPT_RESUME;
        ap_uint<8> cmd_val;
        // rule readback
        static bool rr_start = false;
        static uint32_t rr_cnt = 0;
        // stats readback
        static bool sr_start = false;
        static uint32_t sr_cnt = 0;
        static uint32_t total_rule_cnt = 0;
        static uint32_t total_sr_cnt = 8;
        static uint32_t sr_data[8];
    #pragma HLS ARRAY_PARTITION variable = sr_data complete
        
        // initialize memories in module
        if (is_init) {
            is_init = false;
        INIT_RULE_COUNTERS:
            for (int i = 0; i < (IPRULE_TABLE_SIZE * 2); ++i) rule_m_count[i] = 0;
            // init default counters
            default_count[0] = 0;
            default_count[1] = 0;
            invalid_count[0] = 0;
            invalid_count[1] = 0;
            total_count[0] = 0;
            total_count[1] = 0;
            total_count[2] = 0;
            total_count[3] = 0;
        }
        
    #ifdef SW_EMU_TEST
        uint8_t sw_cnt = 0;
    #endif
    #pragma HLS dataflow
        {
            // Function 1 -> Read command and read/update rules, read stats.
            // read command if available
            if (ipInfoStream.empty() && read_cmd && !commandStream.empty()) {
                cmd_val = commandStream.read();
                c_ipt_cmd = static_cast<IPTCommand>(int(cmd_val));
                std::cout << "Got command: " << int(cmd_val) << std::endl;
                // enable respective state as per command
                is_updating_rules = (c_ipt_cmd == IPT_RULE_UPDATE);
                is_insert_rules = (c_ipt_cmd == IPT_RULE_INSERT);
                is_delete_rules = (c_ipt_cmd == IPT_RULE_DELETE);
                is_reading_rules = (c_ipt_cmd == IPT_RULE_READBACK);
                is_reading_stats = (c_ipt_cmd == IPT_GET_STATS);
                is_set_defaults = (c_ipt_cmd == IPT_SET_DEFAULTS);
                // set start flags
                rr_start = is_reading_rules;
                sr_start = is_reading_stats;
                // disable reading next command
                read_cmd = false;
            } else if (is_updating_rules) {
                // update rule table, if not searching
                if (!is_searching) {
                    // first value is the starting index and rule count
                    auto ruleVal = ruleUpdateStream.read();
                    //uint32_t rule_cnt = ruleVal.data.range(63, 32); // not used at present
                    uint32_t ru_idx = ruleVal.data.range(31, 0);
                    if (table_size == 0) ru_idx = 0;    // force to update from start if no rules present
                RULE_UPD:
                    while (true) {
    #pragma HLS PIPELINE II = 1
                        ruleVal = ruleUpdateStream.read();
                        ruleTable[ru_idx++] = ruleVal.data;
    #ifdef SW_EMU_TEST
                        std::cout << "PF: Recieved Rule: " << ru_idx << std::endl;
                        printRule<RULEWORD_BITS>(ruleVal.data);
    #endif
                        if (ruleVal.last) break;
                    }
                    if (ru_idx > table_size) table_size = ru_idx;   //TODO: update all rules with less
                    read_cmd = true;
                    is_updating_rules = false;
                }
            } else if (is_insert_rules || is_delete_rules) {
                // insert or delete rules, if not searching
                if (!is_searching) {
                    // first value is the starting index and rule count
                    auto ruleVal = ruleUpdateStream.read();
                    uint32_t rule_cnt = ruleVal.data.range(63, 32);
                    uint32_t ru_idx = ruleVal.data.range(31, 0);
                    // eg.
                    // 0 1 2 3 4  5  6  7  8            tbl_sz = 9, ru_idx = 2, rule_cnt = 3
                    // 0 1 x x x  2  3  4  5  6  7  8
                    // 0 2 4 6 8 10 12 14 16
                    // 1 3 5 7 9 11 13 15 17
                    int shift_cnt = table_size - ru_idx;
                    int s_i = (is_insert_rules? (table_size - 1 + rule_cnt) : ru_idx);
                    int decr = (is_insert_rules? 1 : -1);
                    int ofst = rule_cnt * decr;     // +ve for insert, -ve for delete
                    // shift rules in case of insert/prepend/delete
                RULE_SHIFT:
                    for (int i = 0; i < shift_cnt; ++i) {
    #pragma HLS PIPELINE II = 1
                        // shift rule table
                        ruleTable[s_i] = ruleTable[s_i - ofst];
                        // shift stats table
                        int si_2t = s_i * 2;
                        int of_2t = ofst * 2;
                        rule_m_count[si_2t] = rule_m_count[si_2t - of_2t];
                        rule_m_count[si_2t + 1] = rule_m_count[si_2t - of_2t + 1];
                        // update index
                        s_i -= decr;
                    }
                    // copy new rules for insert/prepend
                RULE_COPY:
                    for (int i = 0; is_insert_rules && i < rule_cnt; ++i) {
    #pragma HLS PIPELINE II = 1
                        ruleVal = ruleUpdateStream.read();
                        ruleTable[ru_idx] = ruleVal.data;
                        // clear rule count for inserted new rule
                        int c_idx = ru_idx * 2;
                        rule_m_count[c_idx] = 0;
                        rule_m_count[c_idx + 1] = 0;
                        ++ru_idx;
                    }
                    // clear unused rules stats after deletion
                CLEAR_STATS:
                    for (int i = 0; is_delete_rules && i < rule_cnt; ++i) {
    #pragma HLS PIPELINE II = 1
                        int d_idx = 2 * (table_size - 1 - i);
                        rule_m_count[d_idx] = 0;
                        rule_m_count[d_idx + 1] = 0;
                    }
                    // update table size
                    table_size += ofst;
                    // reset states
                    read_cmd = true;
                    is_insert_rules = false;
                    is_delete_rules = false;
                }
            } else if (is_reading_rules) {
            // readback the rules, nested to infer either read or write at one time
                AXIWord_dt<RULEWORD_BITS> ruleOutVal;
                if (rr_start) {
                    ruleOutVal.data.range(31, 0) = table_size;
                    rr_start = false;
                } else {
                    ruleOutVal.data = ruleTable[rr_cnt++];
                }
                ruleOutVal.last = (rr_cnt == table_size);
                ruleReadbackStream << ruleOutVal;
                if (ruleOutVal.last) {
                    is_reading_rules = false;
                    read_cmd = true;
                    rr_cnt = 0;
                }
            } else if (is_set_defaults) {
            // set default action and unparseable packet action
                auto in_conf_val = ruleUpdateStream.read();
                uint8_t tgt_conf = 0x0F & in_conf_val.data.range(3, 0);
                uint8_t conf_act = 0x0F & in_conf_val.data.range(7, 4);
                // update action
                if (tgt_conf == 0) {
                    default_pkt_action = static_cast<ipTgtAction>(conf_act);
                } else if (tgt_conf == 1) {
                    invalid_pkt_action = static_cast<ipTgtAction>(conf_act);
                }
    #ifdef SW_EMU_TEST
                printf("Set default target: %u, for: %u\n", conf_act, tgt_conf);
    #endif
                is_set_defaults = false;
                read_cmd = true;
            } else if (is_reading_stats) {
            // send statistics
                AXIWord_dt<RULEWORD_BITS> statsOutVal;
                if (sr_start) {
                    total_rule_cnt = (2 * table_size);
                    total_sr_cnt = 8 + total_rule_cnt;
                    statsOutVal.data.range(31, 0) = total_sr_cnt;
                    sr_start = false;
                    // copy sr data
                    sr_data[0] = default_count[0];
                    sr_data[1] = default_count[1];
                    sr_data[2] = invalid_count[0];
                    sr_data[3] = invalid_count[1];
                    sr_data[4] = total_count[0];
                    sr_data[5] = total_count[1];
                    sr_data[6] = total_count[2];
                    sr_data[7] = total_count[3];
                } else {
                    if (sr_cnt < total_rule_cnt) {
                        statsOutVal.data.range(31, 0) = rule_m_count[sr_cnt];
                    } else {
                        statsOutVal.data.range(31, 0) = sr_data[sr_cnt - total_rule_cnt];
                    }
                    ++sr_cnt;
                }
                statsOutVal.last = (sr_cnt == total_sr_cnt);
                ruleReadbackStream << statsOutVal;
                if (statsOutVal.last) {
                    is_reading_stats = false;
                    read_cmd = true;
                    sr_cnt = 0;
                }
            }
        }
        {
            // Function 2 -> Search and Match rules with input packet metadata and output target action
            if (!ipInfoStream.empty()) {
                // get protocol, packet size and valid signal
                auto ip_info = ipInfoStream.read();
                ipTgtAction res_action = IPT_DROP;
                // search if valid packet's metadata
                if (ip_info.is_valid_frame) {
                    uint32_t s_idx = 0;
                    bool default_match = (table_size == 0);
                    is_searching = (!default_match);
                    // Searching starts in 5th cycle in this iteration of outermost loop
                RULE_MATCH:
                    while (is_searching) {
    #pragma HLS PIPELINE II = 1
                        // search for rule match
                        auto curr_rule = ruleTable[s_idx++];
                        // get rule match parameters
                        uint8_t src_mask = curr_rule.range(13, 8) & 0x3F;
                        uint8_t dst_mask = curr_rule.range(19, 14) & 0x3F;
                        ap_uint<16> rule_src_port = curr_rule.range(143, 128);
                        ap_uint<16> rule_dst_port = curr_rule.range(175, 160);
                        ap_uint<4> rule_protocol = curr_rule.range(7, 4);
                        uint8_t cr_target = int(curr_rule.range(30, 27)) & 0xF;
                        ap_uint<32> rule_src_ip = curr_rule.range(95, 64);
                        ap_uint<32> rule_dst_ip = curr_rule.range(127, 96);
                        // get & apply IP Masks to rule IP
                        ap_uint<32> rule_src_ip_m = c_bitMask[src_mask] & rule_src_ip;
                        ap_uint<32> rule_dst_ip_m = c_bitMask[dst_mask] & rule_dst_ip;
                        ap_uint<32> src_ip_m = ip_info.src_ip_address & c_bitMask[src_mask];
                        ap_uint<32> dst_ip_m = ip_info.dst_ip_address & c_bitMask[dst_mask];
                        // match ip-port-protocol
                        bool src_ip_match = (src_ip_m == rule_src_ip_m || rule_src_ip == 0xFFFFFFFF);
                        bool dst_ip_match = (dst_ip_m == rule_dst_ip_m || rule_dst_ip == 0xFFFFFFFF);
                        
                        bool src_port_match = (ip_info.src_ip_port == rule_src_port || rule_src_port == 0);
                        bool dst_port_match = (ip_info.dst_ip_port == rule_dst_port || rule_dst_port == 0);
                        
                        bool protocol_match = (ip_info.protocol == rule_protocol ||
                                              (uint8_t)rule_protocol == static_cast<uint8_t>(PRT_ALL) ||
                                              (uint8_t)rule_protocol == static_cast<uint8_t>(PRT_NONE));
                        
                        bool ipp_match = (src_ip_match & dst_ip_match) & (src_port_match & dst_port_match);
                        if (ipp_match  && protocol_match) {
                            // send action if match
                            res_action = static_cast<ipTgtAction>(cr_target);
                            is_searching = false;
    #ifdef SW_EMU_TEST
                            printf("Matched rule number: %d, action: %d\n", s_idx, cr_target);
    #endif
                        } else if (s_idx == table_size) {
                            // default action if no match found
                            is_searching = false;
                            default_match = true;
                        }
                    }
                    // update stats for the rule matched and target for default match
                    if (default_match) {
                        // default
                        res_action = default_pkt_action;
                        default_count[0]++;
                        default_count[1] += ip_info.frame_size;
                    } else {
                        // respective to matched rule
                        int rmc_idx = (s_idx - 1) * 2;
                        rule_m_count[rmc_idx]++;
                        rule_m_count[rmc_idx + 1] += ip_info.frame_size;
                    }
                } else {
                    // handle invalid case, since nothing else will come for invalid/unparseable packet
                    res_action = invalid_pkt_action;
                    // update stats for the rule matched
                    invalid_count[0]++;
                    invalid_count[1] += ip_info.frame_size;
                }
                ap_uint<4> tgt_action = static_cast<ap_uint<4> >(res_action);
                actionStream << tgt_action;
                // update total count
                auto ttl_ofst = 2 * int((res_action == IPT_DROP));
                total_count[ttl_ofst]++;
                total_count[ttl_ofst + 1] += ip_info.frame_size;
            }
        }
    }
}
#endif // IPT_FILTER_SC_CPP
