/**
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

#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <ap_int.h>

#include "vpp_acc_core.hpp"
#include "iptfilterRxsc.hpp"

#include "cmdlineparser.h"
#include "iptparser.hpp"


/*enum IPTCommand {IPT_RULE_UPDATE = 1, IPT_RULE_INSERT, IPT_RULE_DELETE, IPT_RULE_READBACK, IPT_PAUSE, IPT_RESUME,
                   IPT_INIT_DATA, IPT_GET_STATS};*/
constexpr int c_max_rule_cnt = 64;

bool getRulesFromFile(const std::string& rule_file_name, std::vector<uint32_t> &m_rules) {
    std::cout << "Opening file " << rule_file_name << "!!\n";
    std::ifstream svFile{rule_file_name};
    if(!svFile) {
        std::cerr << "Unable to open file " << rule_file_name << " !!\n";
        //exit(EXIT_FAILURE);
        return false;
    }
    svFile.seekg(0, std::ios::end);
    size_t size = svFile.tellg();
    std::string rulesStr(size, ' ');
    svFile.seekg(0);
    svFile.read(&rulesStr[0], size);
    // parse iptable rules
    IPTableRules ip_rules{rulesStr};
    m_rules.resize(0);
    // m_rules.push_back(ip_rules.getRuleCount());
    ip_rules.getAllRules(m_rules);
    return true;
}

void updateRules(std::vector<uint32_t> &new_rules, uint32_t s_index, bool is_insert = false) {
    // module to update/append/insert rules
    // first 2 values in new rules sent to kernel must be <rule_cnt><starting_index>
    uint32_t ruleCnt = new_rules.size() / 6;
    std::cout << "Sending " << ruleCnt << " rules to update starting from index-" << s_index << std::endl;
    // allocate buffers and copy rules
    ap_uint<192>* ruleTbl = (ap_uint<192>*)IPTablesRxSC::alloc_buf(sizeof(ap_uint<192>) * (ruleCnt + 1), vpp::input);
    // copy header and data
    ruleTbl[0].range(31, 0) = ruleCnt;
    ruleTbl[0].range(63, 32) = s_index;
    for (int i = 0; i < ruleCnt; i++) memcpy(&ruleTbl[i+1], &new_rules.data()[6*i], sizeof(uint32_t) * 6);
    uint32_t cmd = uint32_t(is_insert ? IPT_RULE_INSERT: IPT_RULE_UPDATE);
    // Send these rules to kernel
    auto fut = IPTablesRxSC::compute_async(ruleTbl, nullptr, cmd);
    fut.get();
    std::cout << "Updated rule Table" << std::endl;
    // free buffers
    IPTablesRxSC::free_buf(ruleTbl);
}

void insertRules(std::vector<uint32_t> &new_rules, uint32_t s_index) {
    updateRules(new_rules, s_index, true);
}

bool updateAllRules(const std::string& rule_file_name, std::vector<uint32_t> &m_rules) {
    // read & get rules from rule file
    bool res = getRulesFromFile(rule_file_name, m_rules);
    if (!res) return false;
    
    updateRules(m_rules, 0, false);
    return true;
}


void deleteRule(uint32_t s_index, uint32_t delete_cnt) {
    std::cout << "Deleting " << delete_cnt << " rules starting from index-" << s_index << std::endl;
    // allocate buffers and copy values
    uint32_t* msgHeader  = (uint32_t*)IPTablesRxSC::alloc_buf(sizeof(uint32_t) * 2, vpp::input);
    // populate header
    msgHeader[0] = delete_cnt;
    msgHeader[1] = s_index;
    // Send this header to kernel
    auto fut = IPTablesRxSC::compute_async(msgHeader, nullptr, uint32_t(IPT_RULE_DELETE));
    fut.get();
    std::cout << "Updated rule Table" << std::endl;
    // free buffers
    IPTablesRxSC::free_buf(msgHeader);
}


void readBackRules(std::vector<uint32_t> &r_rules) {
    // allocate buffer of size = rule_cnt * (6 * 32-bits), 192-bits per rule
    uint32_t alcSize = sizeof(ap_uint<192>) * (c_max_rule_cnt);
    ap_uint<192>* ruleTbl = (ap_uint<192>*)IPTablesRxSC::alloc_buf(alcSize, vpp::output);
    
    std::cout << "Sending readback command.." << std::endl;
    // Read back rules from kernel
    auto fut = IPTablesRxSC::compute_async(nullptr, ruleTbl, uint32_t(IPT_RULE_READBACK));
    fut.get();
    uint32_t ruleCount = ruleTbl[0].range(31, 0);
    r_rules.resize(6 * ruleCount);
    for (int i = 0; i < ruleCount; i++) memcpy(&r_rules.data()[6*i], &ruleTbl[i+1], sizeof(uint32_t) * 6);
    std::cout << "Read rules back from kernel !!" << std::endl;
    // free buffers
    IPTablesRxSC::free_buf(ruleTbl);
}

void fetchStats(std::vector<uint32_t> &stats) {
    uint32_t alcSize = sizeof(ap_uint<192>) * (8 + (2 * c_max_rule_cnt));
    ap_uint<192>* stats_list  = (ap_uint<192>*)IPTablesRxSC::alloc_buf(alcSize, vpp::output);
    std::cout << "Sending get_stats command.." << std::endl;
    // Read back stats from kernel
    auto fut = IPTablesRxSC::compute_async(nullptr, stats_list, uint32_t(IPT_GET_STATS));
    fut.get();
    uint32_t rs = stats_list[0].range(31, 0);
    stats.resize(6 * rs);
    for (int i = 0; i < rs; i++) memcpy(&stats.data()[6*i], &stats_list[i+1], sizeof(uint32_t) * 6);
    std::cout << "Read stats from kernel !!" << std::endl;
    // free buffers
    IPTablesRxSC::free_buf(stats_list);
}

void setDefaultAction(bool is_default, ipTgtAction action) {
    uint32_t* def_conf  = (uint32_t*)IPTablesRxSC::alloc_buf(sizeof(uint32_t), vpp::output);
    // prepare config word
    // LSB..MSB
    // <config:default/unparseable 4-bit><target 4-bit>
    uint8_t cf_word = uint8_t(action) << 4;
    if (!is_default) cf_word += 1;   // 0 for default, 1 for unparseable packet config
    *def_conf = cf_word;
    
    std::cout << "Sending set_defaults command.." << int(action) << ", " << int(cf_word) << std::endl;
    // Read back stats from kernel
    auto fut = IPTablesRxSC::compute_async(def_conf, nullptr, uint32_t(IPT_SET_DEFAULTS));
    fut.get();
    std::cout << "Set default actions !!" << std::endl;
    // free buffers
    IPTablesRxSC::free_buf(def_conf);
}

void verifyRuleReadBack(const std::vector<uint32_t>& expected, const std::vector<uint32_t>& read_rules) {
    // verify rules read back from the kernel
    std::cout << "Rules Sent to Kernel: " << expected.size() / 6
                << "\nRules Read from Kernel: " << read_rules.size() / 6 << std::endl;
    if (expected.size() != read_rules.size()) {
        std::cout << "Error: Rule size mismatch!!\nGot: " << read_rules.size() / 6
                    << " Expected: " << expected.size() / 6 << std::endl;
        std::cout << "TEST FAILED: Rules read back NOT same as rules sent to kernel !!" << std::endl;
    } else {
        bool success = true;
        for (uint32_t i = 1; i < expected.size(); ++i) {
            if (expected[i] != read_rules[i]) {
                success = false;
                std::cout << "Error: " << i << ". Got: " << read_rules[i] << " Expected: " << expected[i] << std::endl;
                break;
            }
        }
        if (success) std::cout << "TEST PASSED: Rules read back are same as rules sent to kernel !!" << std::endl;
        else std::cout << "TEST FAILED: Rules read back NOT same as rules sent to kernel !!" << std::endl;
    }
}

void showStats() {
    // fetch stats and display them for each rule
    std::vector<uint32_t> m_stats;
    fetchStats(m_stats);
    // result in pairs <packet count><byte_count>
    // for : rule 1---rule n
    //       default rule
    //       unparsable packets
    //       total
    std::cout << "---- Statistics ----\n";
    std::cout << "Set Rules\n";
    int w = 0;
    for (int i = 1; i < 1 + ((m_stats.size() - 48)/12); ++i) {
        std::cout << "Rule-" << i << ". Matched " << m_stats[w] << " packets, " << m_stats[w + 6] << " bytes\n";
        w += 12;
    }
    std::cout << "Default Rule: ";
    std::cout << " Matched " << m_stats[w] << " packets, " << m_stats[w + 6] << " bytes\n";
    w += 12;
    std::cout << "Unparsable Packet: ";
    std::cout << "Got " << m_stats[w] << " packets, " << m_stats[w + 6] << " bytes\n";
    w += 12;
    std::cout << "-- Total--\n";
    std::cout << "Accepted " << m_stats[w] << " packets, " << m_stats[w + 6] << " bytes\n";
    w += 12;
    std::cout << "Dropped " << m_stats[w] << " packets, " << m_stats[w + 6] << " bytes\n";
    w += 12;
    std::cout << "Total Processed -> " << m_stats[w - 24] + m_stats[w - 12] << " packets, "
                << m_stats[w - 18] + m_stats[w - 6] << " bytes\n";
    std::cout << "---- Done ----\n";
}

void testIncrementalUpdates(std::vector<uint32_t> &m_rules) {
    // test incremental rule update functionality
    /*
     * Kernel already has input rules table with minimum 4 rules. Execute the below actions:
     * 1. Delete 1 rule from the end of the table, readback to confirm
     * 2. Append back the last deleted rule, readback to confirm
     * 3. Delete 1 rule from the table at index-2, readback to confirm
     * 4. Insert the rule back at index-2, readback to confirm
     * 5. Verify that the final table is same as initial table
     */
    if (m_rules.size() < 24) {
        std::cout << "INVALID: Minimum 4 rules needed in kernel to test incremental updates !!" << std::endl;
        return;
    }
    
    uint32_t o_rule_cnt = m_rules.size() / 6;
    uint32_t c_rule_cnt = o_rule_cnt;
    uint32_t r_rule_cnt = 0;
    std::vector<uint32_t> c_rules;
    std::vector<uint32_t> r_rules;
    c_rules.resize(m_rules.size());
    r_rules.resize(0);
    // copy m_rule to c_rules
    for (int i = 0; i < m_rules.size(); ++i) c_rules[i] = m_rules[i];
    
    //------------ Step-1: Delete 1 rule from the end of the table
    deleteRule(o_rule_cnt - 1, 1);
    readBackRules(r_rules);
    r_rule_cnt = r_rules.size() / 6;
    // verify 1 less rule at the end
    bool is_match = true;
    for (int i = 0; i < r_rules.size() && is_match; ++i) is_match = (c_rules[i] == r_rules[i]);
    if (is_match && r_rule_cnt == c_rule_cnt - 1) std::cout << "SUCCESS: Deleted 1 rule from end." << std::endl;
    else {
        std::cout << "FAILURE: Could not delete 1 rule from end." << std::endl;
        return;
    }
    //------------ Step-2: Append back the last deleted rule
    std::vector<uint32_t> new_rules;
    new_rules.resize(6);
    int ofs_i = c_rules.size() - 6;
    for (int i = 0; i < 6; ++i) new_rules[i] = c_rules[ofs_i + i]; // copy new rules, previously deleted
    r_rules.resize(0);
    updateRules(new_rules, o_rule_cnt - 1);
    readBackRules(r_rules);
    r_rule_cnt = r_rules.size() / 6;
    // verify full match
    is_match = true;
    for (int i = 0; i < r_rules.size() && is_match; ++i) is_match = (c_rules[i] == r_rules[i]);
    if (is_match && r_rule_cnt == c_rule_cnt) std::cout << "SUCCESS: Appended 1 rule at the end." << std::endl;
    else {
        std::cout << "FAILURE: Could not append 1 rule at the end." << std::endl;
        return;
    }
    //------------ Step-3: Delete 1 rule from the table at index-2
    r_rules.resize(0);
    deleteRule(2, 1);
    readBackRules(r_rules);
    r_rule_cnt = r_rules.size() / 6;
    // verify full match before and after deleted rule
    is_match = true;
    if (r_rule_cnt != c_rule_cnt - 1) {
        std::cout << "FAILURE: Could not delete 1 rule at index-2." << std::endl;
        return;
    }
    for (int i = 0; i < 12 && is_match; ++i) is_match = (c_rules[i] == r_rules[i]);
    if (!is_match) {
        std::cout << "FAILURE: Could not delete 1 rule at index-2. Mismatch at rules before index-2" << std::endl;
        return;
    }
    for (int i = 12; i < r_rules.size() && is_match; ++i) is_match = (c_rules[6 + i] == r_rules[i]);
    if (!is_match) {
        std::cout << "FAILURE: Could not delete 1 rule at index-2. Mismatch at rules after index-2" << std::endl;
        return;
    }
    std::cout << "SUCCESS: Deleted 1 rule at index-2." << std::endl;
    
    //------------ Step-4: Insert the rule back at index-2
    new_rules.resize(6);
    ofs_i = 12;
    for (int i = 0; i < 6; ++i) new_rules[i] = c_rules[ofs_i + i]; // copy new rules, previously deleted
    r_rules.resize(0);
    insertRules(new_rules, 2);
    readBackRules(r_rules);
    r_rule_cnt = r_rules.size() / 6;
    
    //------------ Step-5: Verify that the final table is same as initial table
    for (int i = 0; i < r_rules.size() && is_match; ++i) is_match = (c_rules[i] == r_rules[i]);
    if (is_match && r_rule_cnt == c_rule_cnt) std::cout << "SUCCESS: Inserted 1 rule at index-2." << std::endl;
    else {
        std::cout << "FAILURE: Could not insert 1 rule at index-2." << std::endl;
        return;
    }
    std::cout << "TEST PASSED: Incremental updates working as expected !!" << std::endl;
}

void testRuleFiltering(const std::string& ruleFile, const std::string& cmd_str) {
    uint64_t nPktCnt, nBytes;
    std::vector<uint32_t> m_rules;
    std::vector<uint32_t> r_rules;
    
    // send rules to the kernel
    bool res = updateAllRules(ruleFile, m_rules);
    if (!res) return;
    std::cout << "Updated rules !!" << std::endl;
    
    // char tch;
    // std::cout << "Enter any character to verify rules !!\n> ";
    // std::cin >> tch;
    // Read back rules from kernel
    readBackRules(r_rules);
    // verify readback rules
    verifyRuleReadBack(m_rules, r_rules);
    // run ntg script to end packets and verify filtering as per iptable rules
    system(cmd_str.c_str());
    // test incremental updates
    // std::cout << "Enter any character to verify incremental rule updates !!\n> ";
    // std::cin >> tch;
    testIncrementalUpdates(m_rules);
    // show stats before exit
    // std::cout << "Enter any character to display statistics and Finish !!\n> ";
    // std::cin >> tch;
    showStats();
    std::cout << "---- Test Complete ----\n";
}

void interactiveFiltering() {
    std::string ruleFile;
    std::string cmd;
    // execute show statistics command to start the emulation
    showStats();
    std::cout << "----------------- Usage -----------------";
    std::cout << "Interactive console for rule updates and stats viewing !!\n";
    std::cout << "Format\n<cmd> <args>\n";
    std::cout << "Supported commands:\n";
    std::cout << "\tf <rule_file_path>: update rules from rule file\n";
    std::cout << "\tc [i] [a|d]: Config default action for default rule or invalid frames\n";
    std::cout << "\t\tc a: Config accept packets as default rule for valid frames\n";
    std::cout << "\t\tc i d: Config drop packets as default rule for invalid frames\n";
    std::cout << "\ts : statistics for all rules\n\te : exit\n";
    
    std::cout << "-----------------------------------------\n";
    while(1) {
        std::cout << "$> ";
        // take input
        getline(std::cin, cmd);
        if (cmd.length() == 1 && cmd[0] == 'e') {
            // press e to exit
            break;
        } else if (cmd[0] == 'f' && cmd[1] == ' ' && cmd.length() > 3) {
            // f <rule_file>
            // take rule files path (in cmd) and apply the rules
            ruleFile = cmd.substr(2, cmd.length() - 1);
            std::vector<uint32_t> m_rules;
            // send rules to the kernel
            bool res = updateAllRules(ruleFile, m_rules);
            if (res) std::cout << "\tRules updated !!";
            std::cout << std::endl;
        } else if (cmd.length() == 1 && cmd[0] == 's') {
            // s
            // fetch stats and display them for each rule
            showStats();
        } else if (cmd[0] == 'c' && cmd[1] == ' ' && cmd.length() < 6) {
            std::cout << cmd[2] << ":" << std::endl;
            bool is_def = (cmd[2] != 'i');
            ipTgtAction tgt_act = IPT_DROP;
            if (is_def) {
                if (cmd[2] == 'a') {
                    tgt_act =  IPT_ACCEPT;
                }
            } else {
                if ((cmd[3] == ' ' && cmd[4] == 'a') || cmd[3] == 'a') tgt_act =  IPT_ACCEPT;
            }
            // set default config
            setDefaultAction(is_def, tgt_act);
        }
    }
    std::cout << "Exit interactive mode.\n";
}

int main(int argc, char** argv) {
    sda::utils::CmdLineParser m_parser;
    m_parser.addSwitch("--flow", "-f", "Test(0) or interactive flow (>= 1)", "");
    m_parser.addSwitch("--iptable_rules", "-r", "IPTableV4 rules file", "");
    m_parser.addSwitch("--pcap", "-p", "pcap input file to enable pcap flow", "");
    m_parser.addSwitch("--ntg", "-n", "python script to send/rcv network data", "");
    
    m_parser.parse(argc, argv);
    
    int flow = std::stoi(m_parser.value("flow"));
    std::string ruleFile = m_parser.value("iptable_rules");
    std::string pcap = m_parser.value("pcap");
    std::string ntg = m_parser.value("ntg");
    if (flow) {
        interactiveFiltering();
    } else {
        if (ruleFile.empty() && ntg.empty() && pcap.empty()) {
            std::cout<<"Please enter all arguments"<<std::endl;
            return 0;
        }
        std::string cmd_str = "python3 " + ntg  + " -p " + pcap + " -f " + std::to_string(flow);
        testRuleFiltering(ruleFile, cmd_str);
    }
    return 0;
}
