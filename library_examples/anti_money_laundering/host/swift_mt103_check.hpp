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

#ifndef _XILINX_AML_HEADER_
#define _XILINX_AML_HEADER_

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1

#include <CL/cl2.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace xf {
namespace aml {

const int max_validated_people = 6000000;
const int max_validated_entity = 9474;
const int max_validated_BIC = 30;
const int max_validated_stopword = 148;

const size_t max_fuzzy_len = 35;
const size_t max_contain_len = -1;
const size_t max_equan_len = 12;

enum UnscopedEnum { PU_NUM = 8 };

// extract select column from CSV file.
// column id starts from 0.
// pass -1 to max_entry_num to get all lines.
int load_csv(const size_t max_entry_num,
             const size_t max_field_len,
             const std::string& file_path,
             const unsigned col,
             std::vector<std::string>& vec);

struct SwiftMT103 {
    int id;
    std::string company;
    std::string channel;
    std::string operationType;
    std::string contract;
    std::string product;
    std::string productSubtype;
    std::string operationTypeForAML;
    std::string currency;
    std::string amount;
    std::string transactionDescription;
    std::string swiftCode1;
    std::string bank1;
    std::string swiftCode2;
    std::string bank2;
    std::string nombrePersona1;
    std::string tipoPersona1;
    std::string codigoPersona1;
    std::string nombrePersona2;
    std::string tipoPersona2;
    std::string codigoPersona2;
    std::string fechaDeCorte;
    std::string fechaDeValor;
};

struct SwiftMT103CheckResult {
    int id;
    int isSkip;                  // empty or too long field will be skipped
    int isMatch;                 // 0-unmatched, 1-matched
    std::vector<int> matchField; // description - swiftCode - bank - nombrePersona
    float timeTaken;
};

class SwiftMT103Checker {
   private:
    int nerror;
    int boost;
    cl::Context ctx;
    cl::Program prg;
    cl::CommandQueue queue;
    cl::Kernel fuzzy[4];

    std::vector<std::vector<int> > vec_base = std::vector<std::vector<int> >(2);
    std::vector<std::vector<int> > vec_offset = std::vector<std::vector<int> >(2);

    int sum_line[2];

    cl::Buffer buf_field_i1[2];
    cl::Buffer buf_field_i2[2];
    cl::Buffer buf_field_i3[2];
    cl::Buffer buf_field_i4[2];
    cl::Buffer buf_csv[4 * PU_NUM];
    cl::Buffer buf_field_o1[2];
    cl::Buffer buf_field_o2[2];
    cl::Buffer buf_field_o3[2];
    cl::Buffer buf_field_o4[2];

    std::vector<std::vector<cl::Event> > events_write = std::vector<std::vector<cl::Event> >(2);
    std::vector<std::vector<cl::Event> > events_kernel = std::vector<std::vector<cl::Event> >(2);
    std::vector<std::vector<cl::Event> > events_read = std::vector<std::vector<cl::Event> >(2);

    std::vector<std::string> vec_stopword;
    std::vector<std::string> vec_people;
    std::vector<std::string> vec_entity;
    std::vector<std::string> vec_bic;

   public:
    SwiftMT103Checker() {}

    std::vector<std::string> getVecStop() { return vec_stopword; }
    std::vector<std::string> getVecPeople() { return vec_people; }
    std::vector<std::string> getVecEntity() { return vec_entity; }
    std::vector<std::string> getVecBIC() { return vec_bic; }

    // The intialize process will download FPGA binary to FPGA card, and initialize the HBM/DDR FACTIVA tables.
    int initialize(const std::string& xclbinPath,
                   const std::string& stopKeywordsFileName,
                   const std::string& peopleFileName,
                   const std::string& entitiesFileName,
                   const std::string& BICRefDataFileName,
                   const unsigned int cardID);

    // The check method returns whether the transaction is okay, and triggering condition if any.
    SwiftMT103CheckResult check(const SwiftMT103& t);
};

} // namespace aml
} // namespace xf

#endif
