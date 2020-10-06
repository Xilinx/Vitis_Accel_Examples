/**
* Copyright (C) 2020 Xilinx, Inc
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

#include "swift_mt103_check.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <future>
#include <cstdlib>

using namespace xf::aml;

int min(int a, int b) {
    return (a < b ? a : b);
}

int abs(int a, int b) {
    return (a < b ? (b - a) : (a - b));
}

float similarity(std::string str1, std::string str2) {
    const int n = str1.length();
    const int m = str2.length();
    if (n == 0 || m == 0) return 0.0;

    int maxDistance = (int)(0.1 * min(n, m));

    if (maxDistance < abs(m - n)) return 0.0;

    std::vector<int> p(n + 1, 0);
    std::vector<int> d(n + 1, 0);

    for (int i = 0; i <= n; i++) p[i] = i;

    for (int j = 1; j <= m; j++) {
        int bestPossibleEditDistance = m;
        char t_j = str2.at(j - 1);
        d[0] = j;

        for (int i = 1; i <= n; i++) {
            if (t_j != str1.at(i - 1))
                d[i] = min(min(d[i - 1], p[i]), p[i - 1]) + 1;
            else
                d[i] = min(min(d[i - 1] + 1, p[i] + 1), p[i - 1]);
            bestPossibleEditDistance = min(bestPossibleEditDistance, d[i]);
        }

        if (j > maxDistance && bestPossibleEditDistance > maxDistance) return 0.0;

        std::swap_ranges(p.begin(), p.end(), d.begin());
    }

    return (1.0 - ((float)p[n] / (float)min(m, n)));
}

int getMaxDistance(int len) {
    if (len < 10 && len >= 0)
        return 0;
    else if (len < 20 && len >= 10)
        return 1;
    else if (len < 30 && len >= 20)
        return 2;
    else
        return 3;
}

void preSort(std::vector<std::string>& vec_str, std::vector<std::vector<std::string> >& vec_grp_str) {
    for (std::vector<std::string>::iterator it = vec_str.begin(); it != vec_str.end(); ++it) {
        int len = it->length();
        if (len < 36) vec_grp_str[len].push_back(*it);
    }
}

bool fuzzyTask(int thread_id,
               int thread_num,
               std::string& pattern,
               std::vector<std::vector<std::string> >& vec_grp_str) {
    bool match = false;
    std::string istr = pattern.length() > 35 ? pattern.substr(0, 35) : pattern; // thunk to <35
    int len = istr.length();
    int med = getMaxDistance(len);

    for (int n = len - med; n <= len + med; n++) {
        std::vector<std::string> deny_list = vec_grp_str[n];
        int step = (deny_list.size() + thread_num - 1) / thread_num;
        int size = size_t(thread_id * step + step) > deny_list.size() ? (deny_list.size() - thread_id * step) : step;
        for (int i = thread_id * step; i < (thread_id * step + size); i++) {
            float sim = similarity(istr, deny_list.at(i));
            if (sim >= 0.9) {
                match = true;
                break;
            }
        }

        if (match) break;
    }

    return match;
}

void strEqual(const std::string& code1,
              const std::string& code2,
              const std::vector<std::string>& vec_str,
              bool result[2]) {
    bool match[2] = {false, false};
    for (std::vector<std::string>::const_iterator it = vec_str.begin(); it != vec_str.end(); ++it) {
        if (code1.compare(*it) == 0) match[0] = true;
        if (code2.compare(*it) == 0) match[1] = true;
    }

    result[0] = match[0];
    result[1] = match[1];
}

bool strContain(const std::string& info, const std::vector<std::string>& vec_str) {
    bool match = false;
    for (std::vector<std::string>::const_iterator it = vec_str.begin(); it != vec_str.end(); ++it) {
        if (info.find(*it) != std::string::npos) match = true;
    }

    return match;
}

void load_dat_cpu(const std::string& stopKeywordsFileName,
                  const std::string& peopleFileName,
                  const std::string& entitiesFileName,
                  const std::string& BICRefDataFileName,
                  std::vector<std::string>& vec_people,
                  std::vector<std::string>& vec_stopword,
                  std::vector<std::string>& vec_entity,
                  std::vector<std::string>& vec_bic) {
    // Read Watch List data for cpu-only mode
    int nerror = 0;
    std::cout << "Loading people.csv...";
    nerror = load_csv(-1U, max_fuzzy_len, peopleFileName, 1, vec_people);
    if (nerror) {
        std::cout << "Failed to load file: people.csv\n";
        exit(1);
    } else
        std::cout << "completed\n";
    std::cout << "Loading stopkeywords.csv...";
    nerror = load_csv(-1U, max_contain_len, stopKeywordsFileName, 1, vec_stopword);
    if (nerror) {
        std::cout << "Failed to load file: stopkeywords.csv\n";
        exit(1);
    } else
        std::cout << "completed\n";
    std::cout << "Loading entities.csv...";
    nerror = load_csv(-1U, max_fuzzy_len, entitiesFileName, 1, vec_entity);
    if (nerror) {
        std::cout << "Failed to load file: entities.csv\n";
        exit(1);
    } else
        std::cout << "completed\n";
    std::cout << "Loading BIC_ref_data.csv...";
    nerror = load_csv(-1U, max_equan_len, BICRefDataFileName, 2, vec_bic);
    if (nerror) {
        std::cout << "Failed to load file: BIC_ref_data.csv\n";
        exit(1);
    } else
        std::cout << "completed\n";
}

const std::string KO[2] = {"OK", "KO"};
const std::string DES[2] = {"", "Description"};
const std::string SWC[2] = {"", "SwiftCode"};
const std::string ENT[2] = {"", "Entity"};
const std::string SND[2] = {"", "Sender"};
std::string print_result(SwiftMT103CheckResult& r) {
    std::string res = "";
    res += std::to_string(r.id);
    res += ",";
    res += KO[r.isMatch];
    res += ",";
    res += DES[r.matchField[0]];
    if (r.matchField[0]) res += ":";
    res += SWC[r.matchField[1]];
    if (r.matchField[0] || r.matchField[1]) res += ":";
    res += ENT[r.matchField[4] || r.matchField[5]];
    if (r.matchField[0] || r.matchField[1] || r.matchField[4] || r.matchField[5]) res += ":";
    res += SND[r.matchField[2] || r.matchField[3]];
    res += ",";
    res += std::to_string(r.timeTaken);
    if (r.isSkip) res += "\t(Skipped)";

    return res;
}

class ArgParser {
   public:
    ArgParser(int& argc, const char** argv) {
        for (int i = 1; i < argc; ++i) mTokens.push_back(std::string(argv[i]));
    }
    bool getCmdOption(const std::string option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->mTokens.begin(), this->mTokens.end(), option);
        if (itr != this->mTokens.end()) {
            return true;
        }
        return false;
    }
    bool getCmdOption(const std::string option, std::string& value) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->mTokens.begin(), this->mTokens.end(), option);
        if (itr != this->mTokens.end() && ++itr != this->mTokens.end()) {
            value = *itr;
            return true;
        }
        return false;
    }

   private:
    std::vector<std::string> mTokens;
};

int main(int argc, const char* argv[]) {
    ArgParser parser(argc, argv);

    std::string xclbin_path;
    std::string in_dir;
    unsigned int work_mode = 0; // FPGA-only mode
    if (parser.getCmdOption("-h")) {
        std::cout << "Usage:\n\ttest.exe -xclbin XCLBIN_PATH -d WATCH_LIST_PATH [-c (0|1|2)]\n" << std::endl;
        std::cout
            << "Option:\n\t-xclbin XCLBIN_PATH\t\trequired, path to xclbin binary\n\t-d WATCH_LIST_PATH\t\trequired, "
               "the folder of watch list csv files\n\t-c 0|1|2\t\t\toptional, default 0 for FPAG only, 1 for CPU only, "
               "2 for both and comparing results\n";
        return 0;
    }

    if (!parser.getCmdOption("-xclbin", xclbin_path)) {
        std::cout << "ERROR: xclbin path is not set!\n";
        return -1;
    }
    if (!parser.getCmdOption("-d", in_dir)) {
        std::cout << "ERROR: input watch list csv file path is not set!\n";
        return -1;
    }
    std::string is_check_str;
    if (parser.getCmdOption("-c", is_check_str)) {
        try {
            work_mode = std::stoi(is_check_str);
        } catch (...) {
            work_mode = 0;
        }
    }

    if (work_mode == 0)
        std::cout << "Select FPGA-only work mode\n";
    else if (work_mode == 1)
        std::cout << "Select CPU-only work mode\n";
    else if (work_mode == 2)
        std::cout << "Select both FPGA and CPU checker\n";
    else {
        std::cout << "ERROR: work mode out of range [0,2]" << std::endl;
        return -1;
    }

    // Path for full dataset
    std::cerr << "----------------------------------------------------------------\n"
                 " NOTICE: The people.csv included in the repo has been tailored.\n"
                 " To download full deny-list for hardware test, please refer to\n"
                 " the README file in data folder.\n"
                 "----------------------------------------------------------------"
              << std::endl;

    // Add Watch List CSV Files
    std::ifstream f;
    const std::string stopKeywordFile = in_dir + "/" + "stopkeywords.csv";
    f.open(stopKeywordFile);
    if (f.good()) {
        f.close();
        f.clear();
    } else {
        std::cout << "Error: " << stopKeywordFile << " cannot be found, please check and re-run.\n\n";
        exit(1);
    }

    const std::string peopleFile = in_dir + "/" + "people.csv";
    f.open(peopleFile);
    if (f.good()) {
        f.close();
        f.clear();
    } else {
        std::cout << "Error: File " << peopleFile << " cannot be found, please check and re-run.\n\n";
        exit(1);
    }

    const std::string entityFile = in_dir + "/" + "entities.csv";
    f.open(stopKeywordFile);
    if (f.good()) {
        f.close();
        f.clear();
    } else {
        std::cout << "Error: File " << entityFile << " cannot be found, please check and re-run.\n\n";
        exit(1);
    }

    const std::string BICRefFile = in_dir + "/" + "BIC_ref_data.csv";
    f.open(BICRefFile);
    if (f.good()) {
        f.close();
        f.clear();
    } else {
        std::cout << "Error: File " << BICRefFile << " cannot be found, please check and re-run.\n\n";
        exit(1);
    }

    // Read some transactions
    const int trans_num = 100;
    std::string test_input = in_dir + "/" + "txdata.csv";
    f.open(test_input);
    if (f.good()) {
        f.close();
        f.clear();
    } else {
        std::cout << "Error: <Input transaction> File " << test_input
                  << " cannot be found, please check and re-run.\n\n";
        exit(1);
    }

    std::vector<std::vector<std::string> > list_trans(7);
    load_csv(trans_num, -1U, test_input, 10, list_trans[0]); // TransactionDescription
    load_csv(trans_num, -1U, test_input, 11, list_trans[1]); // SwiftCode1
    load_csv(trans_num, -1U, test_input, 12, list_trans[2]); // Bank1
    load_csv(trans_num, -1U, test_input, 13, list_trans[3]); // SwiftCode2
    load_csv(trans_num, -1U, test_input, 14, list_trans[4]); // Bank2
    load_csv(trans_num, -1U, test_input, 15, list_trans[5]); // NombrePersona1
    load_csv(trans_num, -1U, test_input, 18, list_trans[6]); // NombrePersona2

    std::vector<SwiftMT103> test_transaction(trans_num);
    for (int i = 0; i < trans_num; i++) {
        test_transaction[i].id = i;
        test_transaction[i].transactionDescription = list_trans[0][i];
        test_transaction[i].swiftCode1 = list_trans[1][i];
        test_transaction[i].bank1 = list_trans[2][i];
        test_transaction[i].swiftCode2 = list_trans[3][i];
        test_transaction[i].bank2 = list_trans[4][i];
        test_transaction[i].nombrePersona1 = list_trans[5][i];
        test_transaction[i].nombrePersona2 = list_trans[6][i];
    }

    SwiftMT103Checker checker;
    std::vector<SwiftMT103CheckResult> result_set(trans_num);

    // Begin to analyze if on mode 0 or 2
    if (work_mode == 0 || work_mode == 2) {
        checker.initialize(xclbin_path, stopKeywordFile, peopleFile, entityFile, BICRefFile, 0); // card 0

        int ccnt = 0;
        float min = std::numeric_limits<float>::max(), max = 0.0, sum = 0.0;
        for (int i = 0; i < trans_num; i++) {
            auto ts = std::chrono::high_resolution_clock::now();
            result_set[i] = checker.check(test_transaction[i]);
            auto te = std::chrono::high_resolution_clock::now();
            float timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(te - ts).count() / 1000.0f;
            result_set[i].timeTaken = timeTaken;

            if (min > timeTaken) min = timeTaken;
            if (max < timeTaken) max = timeTaken;
            sum += timeTaken;

            if (result_set[i].isSkip == 0) ccnt += 1;
        }

        // print the result
        std::cout << "\nTransaction Id, OK/KO, Field of match, Time taken(:ms)" << std::endl;
        for (int i = 0; i < trans_num; i++) {
            std::string s = print_result(result_set[i]);
            std::cout << s << std::endl;
        }

        std::cout << "\nFor FPGA, ";
        if (ccnt < trans_num)
            std::cout
                << ccnt << " transactions were processed, " << (trans_num - ccnt)
                << " were skipped due to the existence of empty field(s) in these transaction.\n(Details are given "
                   "above)\n";
        else
            std::cout << trans_num << " transactions were processed.\n";

        std::cout << "Min(ms)\t\tMax(ms)\t\tAvg(ms)\n";
        std::cout << "----------------------------------------" << std::endl;
        std::cout << min << "\t\t" << max << "\t\t" << sum / ccnt << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }

    // check the result
    if (work_mode == 1 || work_mode == 2) {
        if (work_mode == 2) std::cout << "\nStart to check...\n";

        int nerror = 0;
        std::vector<std::vector<std::string> > vec_grp_people(40);
        std::vector<std::vector<std::string> > vec_grp_entity(40);
        std::vector<std::string> people, entity, bank, stopword;
        if (work_mode == 1)
            load_dat_cpu(stopKeywordFile, peopleFile, entityFile, BICRefFile, people, stopword, entity, bank);
        else {
            people = checker.getVecPeople();
            entity = checker.getVecEntity();
            bank = checker.getVecBIC();
            stopword = checker.getVecStop();
        }

        if (work_mode == 1) std::cout << "Pre-sorting...";
        preSort(people, vec_grp_people);
        preSort(entity, vec_grp_entity);
        if (work_mode == 1) std::cout << "completed\n";

        unsigned int thread_num = std::thread::hardware_concurrency();
        // std::cout << "Thread Number: " << thread_num << std::endl;
        std::future<bool> worker[100];
        bool sw_result[100] = {false};
        int ccnt = 0;
        float min = std::numeric_limits<float>::max(), max = 0.0, sum = 0.0;
        for (int n = 0; n < trans_num; n++) {
            if (work_mode == 1 || result_set[n].isSkip == 0) {
                auto ts = std::chrono::high_resolution_clock::now();
                // check for field nombrePersona1
                for (unsigned i = 0; i < thread_num; i++) {
                    worker[i] = std::async(std::launch::async, fuzzyTask, i, thread_num,
                                           std::ref(test_transaction[n].nombrePersona1), std::ref(vec_grp_people));
                }
                for (unsigned i = 0; i < thread_num; i++) sw_result[i] = worker[i].get();
                bool sw_match = false;
                for (unsigned i = 0; i < thread_num; i++) sw_match |= sw_result[i];
                if (work_mode == 2 && sw_match != (result_set[n].matchField[2] == 1)) {
                    nerror += 1;
                }

                // check for field nombrePersona2
                for (unsigned i = 0; i < thread_num; i++) {
                    worker[i] = std::async(std::launch::async, fuzzyTask, i, thread_num,
                                           std::ref(test_transaction[n].nombrePersona2), std::ref(vec_grp_people));
                }
                for (unsigned i = 0; i < thread_num; i++) sw_result[i] = worker[i].get();
                sw_match = false;
                for (unsigned i = 0; i < thread_num; i++) sw_match |= sw_result[i];
                if (work_mode == 2 && sw_match != (result_set[n].matchField[3] == 1)) {
                    nerror += 1;
                }

                // check for field bank1
                for (unsigned i = 0; i < thread_num; i++) {
                    worker[i] = std::async(std::launch::async, fuzzyTask, i, thread_num,
                                           std::ref(test_transaction[n].bank1), std::ref(vec_grp_entity));
                }
                for (unsigned i = 0; i < thread_num; i++) sw_result[i] = worker[i].get();
                sw_match = false;
                for (unsigned i = 0; i < thread_num; i++) sw_match |= sw_result[i];
                if (work_mode == 2 && sw_match != (result_set[n].matchField[4] == 1)) {
                    nerror += 1;
                }

                // check for field bank2
                for (unsigned i = 0; i < thread_num; i++) {
                    worker[i] = std::async(std::launch::async, fuzzyTask, i, thread_num,
                                           std::ref(test_transaction[n].bank2), std::ref(vec_grp_entity));
                }
                for (unsigned i = 0; i < thread_num; i++) sw_result[i] = worker[i].get();
                sw_match = false;
                for (unsigned i = 0; i < thread_num; i++) sw_match |= sw_result[i];
                if (work_mode == 2 && sw_match != (result_set[n].matchField[5] == 1)) {
                    nerror += 1;
                }

                // check for field swiftCode1 & swiftCode2
                bool sw_equal[2];
                strEqual(test_transaction[n].swiftCode1, test_transaction[n].swiftCode2, bank, sw_equal);

                // check for field TransactionDescription
                bool sw_contain = strContain(test_transaction[n].transactionDescription, stopword);
                (void)(sw_contain);

                auto te = std::chrono::high_resolution_clock::now();
                float timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(te - ts).count() / 1000.0f;
                if (min > timeTaken) min = timeTaken;
                if (max < timeTaken) max = timeTaken;
                sum += timeTaken;
                ccnt += 1;
            }
        }

        std::cout << "\nFor CPU, ";
        if (ccnt < trans_num)
            std::cout
                << ccnt << " transactions were processed, " << (trans_num - ccnt)
                << " were skipped due to the existence of empty field(s) in these transaction.\n(Details are given "
                   "above)\n";
        else
            std::cout << trans_num << " transactions were processed.\n";
        std::cout << "Min(ms)\t\tMax(ms)\t\tAvg(ms)\n";
        std::cout << "----------------------------------------" << std::endl;
        std::cout << min << "\t\t" << max << "\t\t" << sum / ccnt << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        if (work_mode == 2) {
            if (nerror != 0) {
                std::cout << "Error: faild to check " << nerror << " transactions!\n";
                return -1;
            } else {
                std::cout << "Check passed!\n";
            }
        }
    }

    return 0;
}
