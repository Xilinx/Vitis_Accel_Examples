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
#include <iostream>
#include "ucode_utils.hpp"
#include "aie_runtime_control_wrap.hpp"

int main(int argc, const char* argv[]) {
    // cmd parser
    std::cout << "Micro Code Generate...\n";
    ArgParser parser(argc, argv);

    std::string json_file;
    // get input json file
    if (!parser.getCmdOption("-json", json_file)) {
        std::cout << "ERROR: json file path is not set!\n";
        return -1;
    }
    std::string work_path;
    // get input json file
    if (!parser.getCmdOption("-work", work_path)) {
        std::cout << "ERROR: work path is not set!\n";
        return -1;
    }
    std::cout << "work_path=" << work_path << std::endl;
    pl_ucode_gen_impl::work_path = work_path;

    std::string out_file;
    // get output file
    if (!parser.getCmdOption("-out", out_file)) {
        std::cout << "ERROR: output file path is not set!\n";
        return -1;
    }

    pl_ucode_gen_impl::parseControlConfig(json_file);
    // run executeRuntimeControl on x86 to generate ucode
    executeRuntimeControl();
    // dispatch ucode into suitable channel, and event trigger & wait command
    // pl_ucode_gen_impl::addSyncPoint();
    // output ucode into binary file
    pl_ucode_gen_impl::write2Output(out_file);
    return 0;
}
