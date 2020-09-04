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

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>

#include "swift_mt103_check.hpp"
#include "xcl2.hpp"
#include "CL/cl_ext_xilinx.h"

namespace xf {
namespace aml {

int load_csv(const size_t max_entry_num,
             const size_t max_field_len,
             const std::string& file_path,
             const unsigned col,
             std::vector<std::string>& vec) {
    std::ifstream file(file_path.c_str());
    if (!file.is_open()) {
        std::cerr << "\nError:" << file_path << " file cannot be opened for reading!" << std::endl;
        return -1;
    }

    size_t n = 0;
    std::string line;
    // skip the header
    std::getline(file, line);
    while (std::getline(file, line)) {
        unsigned cur_col = 0;
        bool in_escape_str = false;
        std::stringstream ins(line), outs;
        char c;
        while (ins.get(c)) {
            if (c == '"') {
                if (ins.peek() == '"') {
                    // escaped, not state change, just unescape
                    ins.get(c);
                    outs.put(c);
                } else {
                    // toggle state
                    in_escape_str = !in_escape_str;
                }
            } else if (c == ',' && !in_escape_str) {
                if (cur_col == col) {
                    // already got our column
                    break;
                } else {
                    // empty buffer and continue to next column
                    cur_col++;
                    outs = std::stringstream();
                }
            } else if (c == '\r') {
                // in-case we are working with MS new line...
            } else {
                outs.put(c);
            }
        }
        assert(cur_col == col && "not enough column!");
        std::string data = outs.str();
        if (data.length() <= max_field_len) {
            vec.push_back(data);
            // std::cerr << line << "\n\t{" << data << "}" << std::endl;
            n++;
        }
        if (n > max_entry_num) {
            file.close();
            return 0;
        }
    }
    file.close();
    return 0;
}

int group_len(int group_num,
              std::vector<std::string>& vec_str,
              std::vector<std::vector<std::string> >& vec_grp_str,
              std::vector<int>& vec_base,
              std::vector<int>& vec_offset) {
    for (std::vector<std::string>::iterator it = vec_str.begin(); it != vec_str.end(); ++it) {
        int len = it->length();
        vec_offset[len]++;
        vec_grp_str[len].push_back(*it);
    }

    int base_addr = 0;
    for (uint32_t i = 0; i < vec_grp_str.size(); i++) {
        int size = vec_grp_str[i].size();
        int delta = (size + group_num - 1) / group_num;
        vec_base[i] = base_addr;
        vec_offset[i] = delta;

        base_addr += delta;
    }

#ifdef XVERBOSE
    for (uint32_t i = 0; i < vec_offset.size(); i++)
        std::cout << i << ": " << vec_base[i] << " " << vec_offset[i] << std::endl;
#endif

    return base_addr;
}

size_t getMaxDistance(size_t len) {
    if (len < 10)
        return 0;
    else if (len < 20 && len >= 10)
        return 1;
    else if (len < 30 && len >= 20)
        return 2;
    else
        return 3;
}

int getRange(std::string& input, std::vector<int>& vec_base, std::vector<int>& vec_offset, int& base, int& nrow) {
    int cnt = 0;
    size_t len = input.length();
    if (len == 0) {
        return -1;
    }
    size_t med = getMaxDistance(len);

    base = vec_base[len - med] * 3;
    for (unsigned i = len - med; i <= len + med; i++) {
        cnt += vec_offset[i];
    }

    nrow = cnt * 3;

    return 0;
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

int SwiftMT103Checker::initialize(const std::string& xclbinPath,
                                  const std::string& stopKeywordsFileName,
                                  const std::string& peopleFileName,
                                  const std::string& entitiesFileName,
                                  const std::string& BICRefDataFileName,
                                  const unsigned int cardID) {
    // Read Watch List data from given csv files
    std::cout << "Loading people.csv...";
    nerror = load_csv(max_validated_people, max_fuzzy_len, peopleFileName, 1, vec_people);
    if (nerror) {
        std::cout << "failed\n";
        exit(1);
    } else
        std::cout << "completed\n";
    std::cout << "Loading stopkeywords.csv...";
    nerror = load_csv(max_validated_stopword, max_contain_len, stopKeywordsFileName, 1, vec_stopword);
    if (nerror) {
        std::cout << "failed\n";
        exit(1);
    } else
        std::cout << "completed\n";
    std::cout << "Loading entities.csv...";
    nerror = load_csv(max_validated_entity, max_fuzzy_len, entitiesFileName, 1, vec_entity);
    if (nerror) {
        std::cout << "failed\n";
        exit(1);
    } else
        std::cout << "completed\n";
    std::cout << "Loading BIC_ref_data.csv...";
    nerror = load_csv(max_validated_BIC, max_equan_len, BICRefDataFileName, 2, vec_bic);
    if (nerror) {
        std::cout << "failed\n";
        exit(1);
    } else
        std::cout << "completed\n";

    std::cout << "All watch-lists have been loaded.\n";

    // get devices
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    if (cardID >= devices.size()) {
        std::cout << "ERROR: device index out of range [0, " << devices.size() << ")" << std::endl;
        exit(1);
    }
    cl::Device device = devices[cardID];

    // Creating Context and Command Queue for selected Device
    ctx = cl::Context(device);
    queue = cl::CommandQueue(ctx, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
    std::string devName = device.getInfo<CL_DEVICE_NAME>();
#ifdef XVERBOSE
    std::cout << "INFO: Found Device=" << devName << std::endl;
#endif

    // Create program with given xclbin file
    cl::Program::Binaries xclBins = xcl::import_binary_file(xclbinPath);
    devices.resize(1);
    devices[0] = device;
    prg = cl::Program(ctx, devices, xclBins);

    fuzzy[0] = cl::Kernel(prg, "fuzzy_kernel_1");
    fuzzy[1] = cl::Kernel(prg, "fuzzy_kernel_2");
    boost = 0;
#ifdef AML_ENABLE_U250_SUPPORT
    {
        cl_int err_code;
        cl::Kernel k = cl::Kernel(prg, "fuzzy_kernel_3", &err_code);
        if (err_code == CL_SUCCESS)
            boost = 1;
        else
            boost = 0;
    }
#endif

    if (boost) {
        fuzzy[2] = cl::Kernel(prg, "fuzzy_kernel_3");
        fuzzy[3] = cl::Kernel(prg, "fuzzy_kernel_4");
    }

    // Do pre-group process for People deny list to boost Fuzzy function
    for (int i = 0; i < 2; i++) {
        vec_base[i].resize(40, 0);
        vec_offset[i].resize(40, 0);
    }

    std::cout << "Pre-sorting...";
    std::vector<std::vector<std::string> > vec_grp_str1(40);
    std::vector<std::vector<std::string> > vec_grp_str2(40);
    sum_line[0] = group_len(PU_NUM, vec_people, vec_grp_str1, vec_base[0], vec_offset[0]);
    sum_line[1] = group_len(PU_NUM, vec_entity, vec_grp_str2, vec_base[1], vec_offset[1]);
    int sum_line_num = sum_line[0] + sum_line[1];
    std::cout << "completed\n";

    char* csv_part[PU_NUM];
    for (int i = 0; i < PU_NUM; i++) {
        csv_part[i] = aligned_alloc<char>(16 * 3 * sum_line_num);
    }

    for (uint32_t i = 0; i < vec_grp_str1.size(); i++) {
        for (int j = 0; j < vec_offset[0][i]; j++) {
            for (int p = 0; p < PU_NUM; p++) {
                std::string str = "";
                char len = 0;
                if (unsigned(PU_NUM * j + p) < vec_grp_str1[i].size()) {
                    str = vec_grp_str1[i][PU_NUM * j + p];
                    len = i;
                }

                char tmp[48] = {len, 0};
                for (int j = 1; j < 48; j++) {
                    if (j - 1 < len)
                        tmp[j] = str.at(j - 1);
                    else
                        tmp[j] = 0;
                }

                for (int m = 0; m < 3; m++) {
                    for (int k = 0; k < 16; k++)
                        csv_part[p][16 * (3 * (vec_base[0][i] + j) + m) + 15 - k] = tmp[m * 16 + k];
                }
            }
        }
    }

    for (uint32_t i = 0; i < vec_grp_str2.size(); i++) {
        for (int j = 0; j < vec_offset[1][i]; j++) {
            for (int p = 0; p < PU_NUM; p++) {
                std::string str = "";
                char len = 0;
                if (unsigned(PU_NUM * j + p) < vec_grp_str2[i].size()) {
                    str = vec_grp_str2[i][PU_NUM * j + p];
                    len = i;
                }

                char tmp[48] = {len, 0};
                for (int j = 1; j < 48; j++) {
                    if (j - 1 < len)
                        tmp[j] = str.at(j - 1);
                    else
                        tmp[j] = 0;
                }

                for (int m = 0; m < 3; m++) {
                    for (int k = 0; k < 16; k++)
                        csv_part[p][16 * (3 * (sum_line[0] + vec_base[1][i] + j) + m) + 15 - k] = tmp[m * 16 + k];
                }
            }
        }
    }

    // Create device Buffer
    cl_mem_ext_ptr_t mext_i1[2], mext_i2[2], mext_o1[2], mext_o2[2];
    cl_mem_ext_ptr_t mext_i3[2], mext_i4[2], mext_o3[2], mext_o4[2];
    for (int i = 0; i < 2; i++) { // call kernel twice
        mext_i1[i] = {2, nullptr, fuzzy[0]()};
        mext_o1[i] = {11, nullptr, fuzzy[0]()};
        mext_i2[i] = {2, nullptr, fuzzy[1]()};
        mext_o2[i] = {11, nullptr, fuzzy[1]()};

        // create device buffer
        buf_field_i1[i] = cl::Buffer(ctx, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY, sizeof(uint32_t) * 9, &mext_i1[i]);
        buf_field_o1[i] = cl::Buffer(ctx, CL_MEM_EXT_PTR_XILINX | CL_MEM_WRITE_ONLY, sizeof(uint32_t), &mext_o1[i]);
        buf_field_i2[i] = cl::Buffer(ctx, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY, sizeof(uint32_t) * 9, &mext_i2[i]);
        buf_field_o2[i] = cl::Buffer(ctx, CL_MEM_EXT_PTR_XILINX | CL_MEM_WRITE_ONLY, sizeof(uint32_t), &mext_o2[i]);

        if (boost) { // set for kernel 3 & 4
            mext_i3[i] = {2, nullptr, fuzzy[2]()};
            mext_o3[i] = {11, nullptr, fuzzy[2]()};
            mext_i4[i] = {2, nullptr, fuzzy[3]()};
            mext_o4[i] = {11, nullptr, fuzzy[3]()};

            buf_field_i3[i] =
                cl::Buffer(ctx, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY, sizeof(uint32_t) * 9, &mext_i3[i]);
            buf_field_o3[i] = cl::Buffer(ctx, CL_MEM_EXT_PTR_XILINX | CL_MEM_WRITE_ONLY, sizeof(uint32_t), &mext_o3[i]);
            buf_field_i4[i] =
                cl::Buffer(ctx, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY, sizeof(uint32_t) * 9, &mext_i4[i]);
            buf_field_o4[i] = cl::Buffer(ctx, CL_MEM_EXT_PTR_XILINX | CL_MEM_WRITE_ONLY, sizeof(uint32_t), &mext_o4[i]);
        }
    }

    cl_mem_ext_ptr_t mext_t[4 * PU_NUM];
    for (int i = 0; i < PU_NUM; i++) { // kernel-0
        mext_t[i] = {unsigned(i + 3), csv_part[i], fuzzy[0]()};
    }
    for (int i = 0; i < PU_NUM; i++) { // kernel-1
        mext_t[PU_NUM + i] = {unsigned(i + 3), csv_part[i], fuzzy[1]()};
    }
    if (boost) {
        for (int i = 0; i < PU_NUM; i++) { // kernel-2
            mext_t[2 * PU_NUM + i] = {unsigned(i + 3), csv_part[i], fuzzy[2]()};
        }
        for (int i = 0; i < PU_NUM; i++) { // kernel-3
            mext_t[3 * PU_NUM + i] = {unsigned(i + 3), csv_part[i], fuzzy[3]()};
        }
    }

    int dup = (boost == 0) ? 2 : 4;
    for (int i = 0; i < dup * PU_NUM; i++) {
        buf_csv[i] = cl::Buffer(ctx, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                (size_t)16 * 3 * sum_line_num, &mext_t[i]);
    }

    // h2d Transfer
    std::vector<cl::Memory> ob_in;
    for (int i = 0; i < dup * PU_NUM; i++) ob_in.push_back(buf_csv[i]);
    queue.enqueueMigrateMemObjects(ob_in, 0, nullptr, nullptr);
    queue.finish();

    // free memory
    for (int i = 0; i < PU_NUM; i++) free(csv_part[i]);

    return 0;
}

SwiftMT103CheckResult SwiftMT103Checker::check(const SwiftMT103& t) {
    uint32_t buf_f_i0[2][9];
    uint32_t buf_f_i1[2][9];
    uint32_t buf_f_o0[2] = {0, 0};
    uint32_t buf_f_o1[2] = {0, 0};
    uint32_t buf_f_o2[2] = {0, 0};
    uint32_t buf_f_o3[2] = {0, 0};

    std::string name_list[4] = {"nombrePersona1", "nombrePersona2", "bank1", "bank2"};
    std::string vec_str1[4] = {t.nombrePersona1, t.nombrePersona2, t.bank1, t.bank2};
    for (int i = 0; i < 4; i++) {
        std::string str = vec_str1[i];
        char len = str.length() > 35 ? 35 : str.length(); // truncate
        char tmp[36] = {len, str[0], str[1], str[2], 0};
        for (int j = 4; j < 36; j++) {
            if (j - 1 < len)
                tmp[j] = str.at(j - 1);
            else
                tmp[j] = 0;
        }

        for (int m = 0; m < 9; m++) {
            char str_tmp[4];
            for (int k = 0; k < 4; k++) str_tmp[3 - k] = tmp[m * 4 + k];
            if (i % 2 == 0)
                memcpy(&buf_f_i0[i / 2][m], str_tmp, 4);
            else
                memcpy(&buf_f_i1[i / 2][m], str_tmp, 4);
        }
    }

    int base_trans[2 * 2], nrow_trans[2 * 2];
    for (int i = 0; i < 2 * 2; i++) {
        if (vec_str1[i].length() == 0) {
            std::cout << "Transaction-" << t.id << ", Field<" << name_list[i]
                      << "> is empty and this transaction will be ignored\n";
        } else if (vec_str1[i].length() > 35) {
            std::cout << "Transaction-" << t.id << ", length of Field<" << name_list[i] << "> is "
                      << vec_str1[i].length() << " and this field will be truncate to 35\n";
            vec_str1[i] = vec_str1[i].substr(0, 35); // truncate
        }

        int error = getRange(vec_str1[i], vec_base[i / 2], vec_offset[i / 2], base_trans[i], nrow_trans[i]);
        SwiftMT103CheckResult r = {t.id, 1, 0, {0}, 0.0};
        if (error) return r;
    }
    base_trans[2] += sum_line[0] * 3;
    base_trans[3] += sum_line[0] * 3;

    int dup = (boost == 0) ? 2 : 4;
    for (int i = 0; i < 2; i++) {
        events_write[i].resize(dup);
        events_kernel[i].resize(dup);
        events_read[i].resize(dup);
    }

    // struct timeval start_time, end_time;
    // std::cout << "INFO: kernel start------" << std::endl;
    // gettimeofday(&start_time, 0);

    // launch kernel and calculate kernel execution time
    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            queue.enqueueWriteBuffer(buf_field_i1[i], CL_FALSE, 0, sizeof(uint32_t) * 9, buf_f_i0[i], nullptr,
                                     &events_write[i][0]);
            queue.enqueueWriteBuffer(buf_field_i2[i], CL_FALSE, 0, sizeof(uint32_t) * 9, buf_f_i1[i], nullptr,
                                     &events_write[i][1]);
            if (boost) {
                queue.enqueueWriteBuffer(buf_field_i3[i], CL_FALSE, 0, sizeof(uint32_t) * 9, buf_f_i0[i], nullptr,
                                         &events_write[i][2]);
                queue.enqueueWriteBuffer(buf_field_i4[i], CL_FALSE, 0, sizeof(uint32_t) * 9, buf_f_i1[i], nullptr,
                                         &events_write[i][3]);
            }
        } else {
            queue.enqueueWriteBuffer(buf_field_i1[i], CL_FALSE, 0, sizeof(uint32_t) * 9, buf_f_i0[i],
                                     &events_read[i - 1], &events_write[i][0]);
            queue.enqueueWriteBuffer(buf_field_i2[i], CL_FALSE, 0, sizeof(uint32_t) * 9, buf_f_i1[i],
                                     &events_read[i - 1], &events_write[i][1]);
            if (boost) {
                queue.enqueueWriteBuffer(buf_field_i3[i], CL_FALSE, 0, sizeof(uint32_t) * 9, buf_f_i0[i],
                                         &events_read[i - 1], &events_write[i][2]);
                queue.enqueueWriteBuffer(buf_field_i4[i], CL_FALSE, 0, sizeof(uint32_t) * 9, buf_f_i1[i],
                                         &events_read[i - 1], &events_write[i][3]);
            }
        }

        if (boost) {
            int nrow00 = (1 + (nrow_trans[i * 2] / 3)) / 2 * 3;
            int nrow01 = nrow_trans[i * 2] - nrow00;
            int nrow10 = (1 + (nrow_trans[i * 2 + 1] / 3)) / 2 * 3;
            int nrow11 = nrow_trans[i * 2 + 1] - nrow10;

            int j = 0;
            fuzzy[0].setArg(j++, base_trans[i * 2]);
            fuzzy[0].setArg(j++, nrow00);
            fuzzy[0].setArg(j++, buf_field_i1[i]);
            for (int k = 0; k < PU_NUM; k++) fuzzy[0].setArg(j++, buf_csv[k]);
            fuzzy[0].setArg(j++, buf_field_o1[i]);

            j = 0;
            fuzzy[1].setArg(j++, base_trans[i * 2 + 1]);
            fuzzy[1].setArg(j++, nrow10);
            fuzzy[1].setArg(j++, buf_field_i2[i]);
            for (int k = PU_NUM; k < 2 * PU_NUM; k++) fuzzy[1].setArg(j++, buf_csv[k]);
            fuzzy[1].setArg(j++, buf_field_o2[i]);

            j = 0;
            fuzzy[2].setArg(j++, (base_trans[i * 2] + nrow00));
            fuzzy[2].setArg(j++, nrow01);
            fuzzy[2].setArg(j++, buf_field_i3[i]);
            for (int k = 2 * PU_NUM; k < 3 * PU_NUM; k++) fuzzy[2].setArg(j++, buf_csv[k]);
            fuzzy[2].setArg(j++, buf_field_o3[i]);

            j = 0;
            fuzzy[3].setArg(j++, (base_trans[i * 2 + 1] + nrow10));
            fuzzy[3].setArg(j++, nrow11);
            fuzzy[3].setArg(j++, buf_field_i4[i]);
            for (int k = 3 * PU_NUM; k < 4 * PU_NUM; k++) fuzzy[3].setArg(j++, buf_csv[k]);
            fuzzy[3].setArg(j++, buf_field_o4[i]);
        } else {
            int j = 0;
            fuzzy[0].setArg(j++, base_trans[i * 2]);
            fuzzy[0].setArg(j++, nrow_trans[i * 2]);
            fuzzy[0].setArg(j++, buf_field_i1[i]);
            for (int k = 0; k < PU_NUM; k++) fuzzy[0].setArg(j++, buf_csv[k]);
            fuzzy[0].setArg(j++, buf_field_o1[i]);

            j = 0;
            fuzzy[1].setArg(j++, base_trans[i * 2 + 1]);
            fuzzy[1].setArg(j++, nrow_trans[i * 2 + 1]);
            fuzzy[1].setArg(j++, buf_field_i2[i]);
            for (int k = PU_NUM; k < 2 * PU_NUM; k++) fuzzy[1].setArg(j++, buf_csv[k]);
            fuzzy[1].setArg(j++, buf_field_o2[i]);
        }

        queue.enqueueTask(fuzzy[0], &events_write[i], &events_kernel[i][0]);
        queue.enqueueTask(fuzzy[1], &events_write[i], &events_kernel[i][1]);
        if (boost) {
            queue.enqueueTask(fuzzy[2], &events_write[i], &events_kernel[i][2]);
            queue.enqueueTask(fuzzy[3], &events_write[i], &events_kernel[i][3]);
        }

        queue.enqueueReadBuffer(buf_field_o1[i], CL_FALSE, 0, sizeof(uint32_t), &buf_f_o0[i], &events_kernel[i],
                                &events_read[i][0]);
        queue.enqueueReadBuffer(buf_field_o2[i], CL_FALSE, 0, sizeof(uint32_t), &buf_f_o1[i], &events_kernel[i],
                                &events_read[i][1]);
        if (boost) {
            queue.enqueueReadBuffer(buf_field_o3[i], CL_FALSE, 0, sizeof(uint32_t), &buf_f_o2[i], &events_kernel[i],
                                    &events_read[i][2]);
            queue.enqueueReadBuffer(buf_field_o4[i], CL_FALSE, 0, sizeof(uint32_t), &buf_f_o3[i], &events_kernel[i],
                                    &events_read[i][3]);
        }
    }

    bool sw_equal[2], sw_contain;
    strEqual(t.swiftCode1, t.swiftCode2, vec_bic, sw_equal);
    sw_contain = strContain(t.transactionDescription, vec_stopword);
    queue.flush();
    queue.finish();

    SwiftMT103CheckResult r;
    r.id = t.id;
    r.isSkip = 0;
    r.isMatch = 0;
    r.matchField.resize(7);
    for (int i = 0; i < 7; i++) r.matchField[i] = 0;

    if (sw_contain) r.matchField[0] = 1;                           // description
    if (sw_equal[0] || sw_equal[1]) r.matchField[1] = 1;           // swiftcode
    if (buf_f_o0[0] == 1 || buf_f_o2[0] == 1) r.matchField[2] = 1; // person1
    if (buf_f_o1[0] == 1 || buf_f_o3[0] == 1) r.matchField[3] = 1; // person2
    if (buf_f_o0[1] == 1 || buf_f_o2[1] == 1) r.matchField[4] = 1; // bank1
    if (buf_f_o1[1] == 1 || buf_f_o3[1] == 1) r.matchField[5] = 1; // bank2
    if (sw_contain || sw_equal[0] || sw_equal[1] || buf_f_o0[0] == 1 || buf_f_o1[0] == 1 || buf_f_o0[1] == 1 ||
        buf_f_o1[1] == 1 || buf_f_o2[0] == 1 || buf_f_o3[0] == 1 || buf_f_o2[1] == 1 || buf_f_o3[1] == 1)
        r.isMatch = 1;

    return r;
}

} // namespace aml
} // namespace xf
