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

#ifndef GENERAL_DEF_HPP
#define GENERAL_DEF_HPP

// default destination UDP port
#define DEFAULT_DEST_IP_PORT 0x1234

// width of AXI stream data port
#define KRNL_DATA_WIDTH 32
// width of AXI stream keep port
#define KRNL_KEEP_WIDTH (KRNL_DATA_WIDTH / 8)
// width of TUSER for Rx port (according to X3 spec) excluding reserve bits (parity + Meta)
#define RX_TUSER 9
// width of TUSER for Tx port (according to X3 spec) excluding reserve bits (parity + Meta)
#define TX_TUSER 9
// Len of word in bytes
#define WORD_LEN_IN_BYTES (512 / KRNL_DATA_WIDTH)
// no. of 32-bit words in Data port
#define WORD_COUNT_32 (KRNL_DATA_WIDTH / 32)

// match conditions
#define MATCH_ETHER_TYPE 0x0800
#define MATCH_IP_VERSION 4
#define MATCH_IP_HEADER_LEN 5
#define MATCH_IP_PROTOCOL 17

// definitions for action ID
#define ACT_ID_NOP 0
#define ACT_ID_DROP 4
#define ACT_ID_MODIFY_FROM_TABLE 5
#define ACT_ID_INSERT_FROM_TABLE 6

// bit length of each field in Ethernet frame
#define LEN_PACKET_META 0
#define LEN_META_CH_LEN 14
//
#define LEN_MAC_ADDRESS 48
#define LEN_ETHER_TYPE 16
//
#define LEN_IP_VERSION 4
#define LEN_IP_HEADER_LEN 4
#define LEN_IP_TOS 8
#define LEN_IP_TOTAL_LEN 16
#define LEN_IP_IDENTIFICATION 16
#define LEN_IP_FRAGMENT 16
#define LEN_IP_TTL 8
#define LEN_IP_PROTOCOL 8
#define LEN_IP_CHECKSUM 16
#define LEN_IP_SOURCE_IP 32
#define LEN_IP_DEST_IP 32
//
#define LEN_UDP_DEST_PORT 16
#define LEN_UDP_LEN 16
//
#define LEN_CMD_ACT_ID 8
#define LEN_CMD_TABLE_KEY 8
//
#define LEN_CHECKSUM 32

// Metadata
#define OFFSET_PACKET_META 0
#define OFFSET_META_CH_LEN 24
// Ether header
#define OFFSET_ETHER (OFFSET_PACKET_META + LEN_PACKET_META)
#define OFFSET_ETHER_SOURCE_MAC (OFFSET_ETHER + LEN_MAC_ADDRESS)
#define OFFSET_ETHER_TYPE (OFFSET_ETHER + 96)
// IP header
#define OFFSET_IP (OFFSET_ETHER_TYPE + LEN_ETHER_TYPE)
// -- IP version on MSB side of the byte
#define OFFSET_IP_VERSION (OFFSET_IP + 0)
#define OFFSET_IP_HEADER_LEN (OFFSET_IP + 4)
#define OFFSET_IP_TOS (OFFSET_IP_HEADER_LEN + 4)
#define OFFSET_IP_TOTAL_LEN (OFFSET_IP + 16)
#define OFFSET_IP_IDENTIFICATION (OFFSET_IP_TOTAL_LEN + 16)
#define OFFSET_IP_FRAGMENT (OFFSET_IP_IDENTIFICATION + 16)
#define OFFSET_IP_TTL (OFFSET_IP_FRAGMENT + 16)
#define OFFSET_IP_PROTOCOL (OFFSET_IP + 72)
#define OFFSET_IP_CHECKSUM (OFFSET_IP_PROTOCOL + LEN_IP_PROTOCOL)
#define OFFSET_IP_SOURCE_IP (OFFSET_IP_CHECKSUM + LEN_IP_CHECKSUM)
#define OFFSET_IP_DEST_IP (OFFSET_IP_SOURCE_IP + LEN_IP_SOURCE_IP)
// UDP header
#define OFFSET_UDP (OFFSET_IP + MATCH_IP_HEADER_LEN * 32)
#define OFFSET_UDP_DEST_PORT (OFFSET_UDP + 16)
#define OFFSET_UDP_LEN (OFFSET_UDP + 32)
#define OFFSET_UDP_CHECKSUM (OFFSET_UDP + 48)
// Command
#define OFFSET_CMD (OFFSET_UDP + 64)
#define OFFSET_CMD_ACT_ID (OFFSET_CMD + 0)
#define OFFSET_CMD_TABLE_KEY (OFFSET_CMD + 8)
// Rest of payload
#define OFFSET_PAYLOAD (OFFSET_CMD + 16)

// TOTAL HEADER LEN
#define LEN_TOTAL_HEADER 352
#endif // GENERAL_DEF_HPP
