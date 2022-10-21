#ifndef IPT_PARSER_HPP
#define IPT_PARSER_HPP

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <string>
#include <vector>
#include <set>

class IPTTypes {
public:
    static const std::array<std::string, 6> p_protocol;
    static const std::array<std::string, 5> p_connstate;
    static const std::array<std::string, 4> p_target;
    
    enum protocolType {NPL = 0, TCP, UDP, ARP, ICMP, PRT_ALL};
    enum connectionState {ANYSTATE = 0, NEW, ESTABLISHED, RELATED, INVALID};
    enum targetAction {LOG = 0, ACCEPT, REJECT, DROP};
    
    static protocolType protocol (const std::string &istr) {
        for (int i = 0; i < p_protocol.size(); ++i) {
            if (istr == p_protocol[i]) return static_cast<protocolType>(i);
        }
        return NPL;
    }
    
    static connectionState connstate (const std::string &istr) {
        for (int i = 0; i < p_connstate.size(); ++i) {
            if (istr == p_connstate[i]) return static_cast<connectionState>(i);
        }
        return ANYSTATE;
    }
    
    static targetAction target (const std::string &istr) {
        for (int i = 0; i < p_target.size(); ++i) {
            if (istr == p_target[i]) return static_cast<targetAction>(i);
        }
        return LOG;
    }
    
    static std::string target (const targetAction &tgt) { return p_target[static_cast<int>(tgt)]; }
    static std::string protocol (const protocolType &prtc) { return p_protocol[static_cast<int>(prtc)]; }
    
    static std::string getIPString(uint32_t ip) {
        std::string ips{""};
        auto pfx = ".";
        for (uint8_t i = 0; i < 4; ++i) {
            if (i == 3) pfx = "";
            uint32_t tmp = ((ip >> (i * 8)) & 0xFF);
            ips.append(std::to_string(tmp) + pfx);
        }
        return ips;
    }
    
    static uint32_t getIPFromString(const std::string &ips) {
        uint32_t ip{0};
        int i{0}, pos{0};
        char delim = '.';
        for (uint8_t z = 0; z < 4; ++z) {
            if (z == 3) delim = ' ';
            pos = ips.find(delim, i);
            auto ipsub = ips.substr(i, pos - i);
            i = pos + 1;
            uint32_t tmp = std::stoi(ipsub);
            ip += (tmp << (z * 8));
        }
        return ip;
    }
    
};

const std::array<std::string, 6> IPTTypes::p_protocol = {"NPL", "tcp", "udp", "arp", "icmp", "all"};
const std::array<std::string, 5> IPTTypes::p_connstate = {"ANYSTATE", "NEW", "ESTABLISHED", "RELATED", "INVALID"};
const std::array<std::string, 4> IPTTypes::p_target = {"LOG", "ACCEPT", "REJECT", "DROP"};


class IPV4NFRule {

private:
    int m_ruleId{0};
    bool m_default {false};
    uint32_t m_ipv4[2] {static_cast<uint32_t>(-1), static_cast<uint32_t>(-1)};
    uint8_t m_ipMask[2] {32, 32};   // by default set to use all subnets for exact matching
    uint16_t m_port[2] {0, 0};
    IPTTypes::protocolType m_prtc {IPTTypes::PRT_ALL};
    uint8_t m_interface {0};
    std::set<IPTTypes::connectionState> m_states;
    IPTTypes::targetAction m_action {IPTTypes::LOG};
    uint8_t m_match{0};     // 0: Protocol, 1: State (connection tracking)
    bool m_enableCount {false};
    uint64_t m_count {0};
    uint64_t m_bytes {0};
    
public:

    IPV4NFRule (bool isdef, IPTTypes::targetAction action, uint64_t packetCount = 0, uint64_t byteCount = 0)
    : m_default(isdef), m_action(action), m_count(packetCount), m_bytes(byteCount) {
        m_enableCount = isdef;
        if (isdef) {
            m_prtc = IPTTypes::PRT_ALL;
        }
    }

    IPV4NFRule (bool ifcount, uint8_t intf, IPTTypes::protocolType proto, uint8_t match, IPTTypes::targetAction action)
    : m_enableCount(ifcount), m_interface(intf),
      m_prtc(proto), m_action(action), m_match(match) {
        // other stuff if needed
    }
    
    int& ruleId () { return m_ruleId; }
    bool& enableCounting () { return m_enableCount; }
    uint8_t& interface () { return m_interface; }
    uint8_t& match () { return m_match; }
    IPTTypes::targetAction& action () {return m_action; }
    IPTTypes::protocolType& protocol () {return m_prtc; }
    
    uint32_t& srcIP () { return m_ipv4[0]; }
    uint16_t& srcPort () { return m_port[0]; }
    uint8_t& srcIPMask () { return m_ipMask[0]; }
    uint32_t& dstIP () { return m_ipv4[1]; }
    uint16_t& dstPort () { return m_port[1]; }
    uint8_t& dstIPMask () { return m_ipMask[1]; }
    
    uint64_t& matchCount() { return m_count; }
    uint64_t& matchBytes() { return m_bytes; }
    
    void setConnState (IPTTypes::connectionState state) {
         m_states.insert(state);
    }
    
    void getConnStates (IPTTypes::connectionState states[4]) {
        int i = 0;
        for(IPTTypes::connectionState st: m_states) states[i++] = st;
    }
    
    IPV4NFRule& operator= (const IPV4NFRule& iVal) {
        // replicate rules
        m_ruleId = iVal.m_ruleId;
        m_default = iVal.m_default;
        m_match = iVal.m_match;
        m_prtc = iVal.m_prtc;
        m_interface = iVal.m_interface;
        m_action = iVal.m_action;
        m_enableCount = iVal.m_enableCount;
        m_count = iVal.m_count;
        m_bytes = iVal.m_bytes;
        
        for (auto i = 0; i < 2; ++i) {
            m_ipv4[i] = iVal.m_ipv4[i];
            m_port[i] = iVal.m_port[i];
            m_ipMask[i] = iVal.m_ipMask[i];
        }
        m_states = iVal.m_states;
        
        return *this;
    }
    
    bool getRuleArray(uint32_t (&bcamWord)[6]) {
        // prepare and return a word to be written to offload engine
        // output rule bitstream format
        // > Each rule uses 192-bits (24-bytes)
        // > Bit Order: LSB--->---MSB
        // > Word 1 : <default rule><interface><protocol><src IP mask><dest IP mask><connStates><target><count>
        //                 1-bit       3-bit     4-bits      6-bit         6-bit        7-bit    4-bit   1-bit
        // > Word 2 : <port_arrangement><match_flags_in_order: <match_1><match_2>..<match_4> >
        //               16-bit            16-bits(optional) : 4-bit each       // Word 2 is unused at present
        // > Word 3 : <source IP>
        //              32-bit
        // > Word 4 : <destination IP>
        //                 32-bit
        // > Word 5 : <Src_Port_Low><Src_Port_High>
        //                 16-bit      16-bit
        // > Word 6 : <Dst_Port_Low><Dst_Port_High>
        //                 16-bit      16-bit
        // ----------------------------
        // 16-bit port arrangement: <range_flag><src_port_cnt><dst_port_cnt><unused>
        //                            2-bits        3-bits        3-bits     8-bits
        uint32_t word = 0;
        // Word 1
        if (m_default) word = 1;    // default rule flag
        word += ((m_interface << 1) & 0x0F);  // interface number, 3-bits
        word += ((static_cast<uint32_t>(m_prtc) << 4) & 0xFF);    // protocol, 4-bits
        word += ((uint32_t(m_ipMask[0]) << 8) & 0x3F00) + ((uint32_t(m_ipMask[1]) << 14) & 0xFC000); // src,dst IP mask
        // word += ....connstates....9-bits....not supported at present
        word += ((static_cast<uint32_t>(m_action) << 27) & 0x78000000);    // target
        if (m_enableCount) word += (1 << 31); // enable packet counting
        bcamWord[0] = word;
        // Word 2
        word = 0;
        word += ((1 << 2) + (1 << 5));  // number of ports specified in src and dst, not used at present
        // port range and multple ports not supported at present
        // match flag -m not supported at present
        bcamWord[1] = word;
        // Word 3 & 4
        bcamWord[2] = m_ipv4[0];  // src IP address
        bcamWord[3] = m_ipv4[1];  // dst IP address
        // Word 5 & 6
        bcamWord[4] = (uint32_t(m_port[0]) & 0x0000FFFF);   // upto 2 src ports or 1 range
        bcamWord[5] = (uint32_t(m_port[1]) & 0x0000FFFF);   // upto 2 dst ports or 1 range
        
        return true;
    }
    
    void print() {
        // pretty print
        if (m_default) std::cout << "Default";
        else std::cout << m_ruleId;
        std::cout << "--------------------------------------\n";
        std::cout << "Protocol: " << IPTTypes::protocol(m_prtc) << "\n";
        //    std::cout << "1 Done" << std::endl;
        std::cout << "Total Packets: " << m_count << "\n";
        //    std::cout << "2 Done" << std::endl;
        std::cout << "Total Bytes: " << m_bytes << "\n";
        //    std::cout << "3 Done" << std::endl;
        if (!m_default) {
            std::cout << "Src: " << IPTTypes::getIPString(m_ipv4[0]) << "/" << unsigned(m_ipMask[0]) << ":";
            std::cout << m_port[0] << "\n";
            //    std::cout << "4 Done" << std::endl;
            std::cout << "Dst: " << IPTTypes::getIPString(m_ipv4[1]) << "/" << unsigned(m_ipMask[1]) << ":";
            std::cout << m_port[1] << "\n";
            //    std::cout << "5 Done" << std::endl;
        }
        std::cout << "Target: " << IPTTypes::target(m_action) << "\n";
        //    std::cout << "6 Done" << std::endl;
    }
    
};

/*
 *  IPRuleTable class
 *  Following are the requirements
 *  > Parse input command and check if input chain is modified or not
 *  > Convert input command into a rule entry
 *  > Parse saved iptable rules file
 */

class IPTableRules {
private:
    uint8_t m_ipVersion{4};
    std::vector<IPV4NFRule> m_inputRuleChain{};
    std::vector<std::string> m_interfaces{};
    
    int spaceEnd(std::string str, int beg) {
        int nwIdx{beg};
        for (int i = beg; i < str.size(); ++i) {
            nwIdx = i;
            if (str[i] != ' ') break;
        }
        return nwIdx;
    }
    
    std::string removeConstSpace(const std::string &str) {
        std::string newStr{""};
        for (int i = 0; i < str.length(); ++i) {
            if (str[i] == ' ') {
                if (i == 0 || (i > 0 && str[i - 1] == ' ')) continue;
            }
            
            newStr += str[i];
        }
        return newStr;
    }
    
    std::string firstSubstr(const std::string &str, int beg, char delim) {
        std::string nstr;
        int len = 0;
        for (int i = beg; i < str.size(); ++i) {
            if (str[i] == delim) break;
            ++len;
        }
        nstr.assign(str, beg, len);
        return nstr;
    }
    
public:
    
    IPTableRules(const std::string &ruleSrc) {
        // parse input string to load input chain rules
        parseRules(ruleSrc);
        // delete first rule, since it has no use
        m_inputRuleChain.erase(m_inputRuleChain.begin());
    }

    void appendRule(IPV4NFRule rule) {
        // append Rule to rule chain
        //std::cout << "Append Rule....\n";
        //rule.print();
        m_inputRuleChain.push_back(rule);
    }
    
    void addInterface(std::string intf) {
        m_interfaces.push_back(intf);
    }
    
    bool removeInterface(const std::string &intf) {
        for (int i = 0; i < m_interfaces.size(); ++i) {
            if (m_interfaces[i] == intf) {
                m_interfaces.erase(m_interfaces.begin() + i);
                return true;
            }
        }
        return false; // return false if interface not found
    }
    
    bool parseCount (const std::string& rule, uint64_t &packetCnt, uint64_t &byteCnt) {
        auto lcl_rule = rule;
        if (rule[0] != '[') return false;
        int idx = 1;
        std::string pkt_cnt_str = firstSubstr(lcl_rule, idx, ':');
        std::string byt_cnt_str = firstSubstr(lcl_rule, (idx + pkt_cnt_str.length() + 1), ']');
        packetCnt = std::stoll(pkt_cnt_str);
        byteCnt = std::stoll(byt_cnt_str);
        //std::cout << "pkt_cnt_str: " << pkt_cnt_str << " " << packetCnt << std::endl;
        //std::cout << "byt_cnt_str: " << byt_cnt_str << " " << byteCnt << std::endl;
        return true;
    }
    
    bool parseRule (const std::string& rule) {
        // parse single rule command
        auto c_rule = removeConstSpace(rule);   // exactly 1 space as separator, none at beginning
        // check if this is INPUT rule
        auto inpRIdx = c_rule.find("-A INPUT", 0);
        if (inpRIdx == std::string::npos) return false;
        //std::cout << "Parsing rule:-> " << c_rule << std::endl;
        
        uint64_t packetCnt{0}, byteCnt{0};
        size_t i{0};
        size_t pos{0};
        if (c_rule[0] == '[') { // get packet/byte count
            pos = c_rule.find(']');
            std::string cntstr = c_rule.substr(i, pos);
            i = pos + 1;
            parseCount(cntstr, packetCnt, byteCnt);
        }
        // parse rest of the command
        pos = c_rule.find(' ', i);
        if (pos == i) pos = c_rule.find(' ', ++i);
        IPV4NFRule newRule(false, IPTTypes::LOG, packetCnt, byteCnt);
        
        while (pos != std::string::npos) {
            auto key = c_rule.substr(i, pos - i);
            i = pos + 1;
            if (key[0] != '-') return false;    // confirm that it is key
            
            pos = c_rule.find(' ', i);
            auto eidx = ((pos == std::string::npos) ? (c_rule.length() - 1) : (pos - i));
            auto val = c_rule.substr(i, eidx);
            i = pos + 1;
            //std::cout << "Key:" << key << ", Value:" << val << " pos: " << std::endl;
            if (key == "-A") {
                if (val != "INPUT") return false;
                
            } else if (key == "-i") {
                bool found = false;
                for (int idx = 0; idx < m_interfaces.size(); ++idx) {
                    if (m_interfaces[idx] == val) { // check if already available
                        newRule.interface() = uint8_t(idx);
                        found = true;
                    }
                }
                if (!found) {
                    m_interfaces.push_back(val);
                    newRule.interface() = uint8_t(m_interfaces.size() - 1);
                }
                
            } else if (key == "-p") {
                newRule.protocol() = IPTTypes::protocol(val);
                
            } else if (key == "-m") {
                //skip for now
            } else if (key == "--sport") {
                newRule.srcPort() = std::stoi(val);
                
            } else if (key == "--dport") {
                newRule.dstPort() = std::stoi(val);
                
            } else if (key == "-s") {
                size_t mp = val.find('/');
                size_t ln = val.length();
                if (mp != std::string::npos) {
                    ln = mp;
                    // get mask if given
                    newRule.srcIPMask() = std::stoi(val.substr(mp+1, 2));
                }
                // get IP
                newRule.srcIP() = IPTTypes::getIPFromString(val.substr(0, ln));
                
            } else if (key == "-d") {
                size_t mp = val.find('/');
                size_t ln = val.length();
                if (mp != std::string::npos) {
                    ln = mp;
                    // get mask if given
                    newRule.dstIPMask() = std::stoi(val.substr(mp+1, 2));
                }
                // get IP
                newRule.dstIP() = IPTTypes::getIPFromString(val.substr(0, ln));
                
            } else if (key == "--ctstate") {
                // skip for now
            } else if (key == "-j") {
                newRule.action() = IPTTypes::target(val);
                
            }
            // find next pos
            if (pos != std::string::npos) pos = c_rule.find(' ', i);
        }
        newRule.ruleId() = m_inputRuleChain.size();
        appendRule(newRule);
        return true;
    }
    
    void parseRules (const std::string &inRuleSrc) {
        auto rule_src = inRuleSrc;
        std::stringstream rule_stream {rule_src};
        std::string line{};
        bool inTable = false;
        
        while (std::getline(rule_stream, line, '\n')) {
            // parse each line
            // find filter table and search for input rules
            bool parse_res = false;
            auto rule_line = removeConstSpace(line);
            if (rule_line[0] == '#') continue;
            if (rule_line[0] >= 'A' && rule_line[0] <= 'Z') continue;
            if (rule_line[0] >= 'a' && rule_line[0] <= 'z') continue;
            if (rule_line[0] == '*') { // is table
                inTable = false;
                std::string tableName;
                tableName.assign(rule_line, 1, 6);
                // check if it is filter table
                inTable = (tableName == "filter");
                parse_res = true;
            } else {
                if (inTable) {
                    if (rule_line[0] == ':') {    // default rule
                        int idx = 1, pos = 0;
                        pos = rule_line.find(' ', idx);
                        std::string chain = rule_line.substr(idx, pos - 1);
                        idx = pos + 1;
                        // check if it is INPUT chain
                        parse_res = false;
                        if (chain == "INPUT") {
                            //std::cout << "Chain: " << chain << std::endl;
                            // find target
                            pos = rule_line.find(' ', idx);
                            std::string l_target_str = rule_line.substr(idx, pos - idx);
                            idx = pos + 1;
                            auto tgt = IPTTypes::target(l_target_str);
                            // parse packet and byte count, ignoring parsing errors
                            uint64_t pCnt{0}, bCnt{0};
                            parseCount(rule_line.substr(idx, rule_line.length() - pos - 1), pCnt, bCnt);
                            // create a rule entry
                            IPV4NFRule defRule(true, tgt, pCnt, bCnt);
                            appendRule(defRule);
                            parse_res = true;
                        }
                    } else {    // regular rules
                        parse_res = parseRule(rule_line);
                    }
                }// inside filter table
            } //check line
            //if(inTable && parse_res) std::cout << "Parsed---> " << rule_line << "\n";
        }// read and parse lines
    }
    
    void print() {
        // print list of rules
        for (IPV4NFRule rule: m_inputRuleChain) {
            rule.print();
            std::cout << "Interface: " << m_interfaces[rule.interface()] << "\n";
        }
    }
    
    void getRuleArray(int idx, uint32_t (&bcamWord)[6]) {
        m_inputRuleChain[idx].getRuleArray(bcamWord);
    }
    
    void printRuleWords() {
        uint32_t bcamWord[6];
        for (int i = 0; i < m_inputRuleChain.size(); ++i) {
            getRuleArray(i, bcamWord);
            std::cout << "Rule: " << i << ". ";
            for (int k = 0; k < 6; ++k) {
                std::cout << bcamWord[k] << ", ";
            }
            std::cout << bcamWord[6] << "\n";
        }
        std::cout << std::endl;
    }
    
    void getAllRules(std::vector<uint32_t> &bcamWords) {
        // get all rule arrays
        uint32_t bcmWord[6];
        for (int i = 0; i < m_inputRuleChain.size(); ++i) {
            getRuleArray(i, bcmWord);
            for (int k = 0; k < 6; ++k) bcamWords.push_back(bcmWord[k]);
        }
    }
    
    uint32_t getRuleCount() {
        return m_inputRuleChain.size();
    }
    
    ~IPTableRules() {
        // empty the vector
        m_inputRuleChain.clear();
    }
    
};

#endif  // IPT_PARSER_HPP

