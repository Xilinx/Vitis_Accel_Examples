/**
* Copyright (C) 2019-2021 Xilinx, Inc
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

#include "xtlm.h"
#include "xtlm_ap_ctrl.h"

class vadd : public xsc::xtlm_ap_ctrl_hs {
   public:
    SC_HAS_PROCESS(vadd);
    vadd(sc_module_name name, xsc::common_cpp::properties& _properties) : xsc::xtlm_ap_ctrl_hs(name) {
        DEFINE_XTLM_AXIMM_MASTER_IF(in1, 32);
        DEFINE_XTLM_AXIMM_MASTER_IF(in2, 32);
        DEFINE_XTLM_AXIMM_MASTER_IF(out_r, 32);

        ADD_MEMORY_IF_ARG(in1, 0x10, 0x8);
        ADD_MEMORY_IF_ARG(in2, 0x18, 0x8);
        ADD_MEMORY_IF_ARG(out_r, 0x20, 0x8);
        ADD_SCALAR_ARG(size, 0x28, 0x4);

        SC_THREAD(main_thread);
    }

    //! Declare aximm interfaces..
    DECLARE_XTLM_AXIMM_MASTER_IF(in1);
    DECLARE_XTLM_AXIMM_MASTER_IF(in2);
    DECLARE_XTLM_AXIMM_MASTER_IF(out_r);

    //! Declare scalar args...
    unsigned int size;

    void main_thread() {
        wait(ev_ap_start); //! Wait on ap_start event...

        //! Copy kernel args configured by host...
        uint64_t in1_base_addr = kernel_args[0];
        uint64_t in2_base_addr = kernel_args[1];
        uint64_t out_r_base_addr = kernel_args[2];
        size = kernel_args[3];

        unsigned data1, data2, data_r;
        for (int i = 0; i < size; i++) {
            in1->read(in1_base_addr + (i * 4), (unsigned char*)&data1); //! Read from in1 interface
            in2->read(in2_base_addr + (i * 4), (unsigned char*)&data2); //! Read from in2 interface

            //! Add data1 & data2 and write back result
            data_r = data1 + data2;                                           //! Add
            out_r->write(out_r_base_addr + (i * 4), (unsigned char*)&data_r); //! Write the result

            //! Workaround to avoid max. delta cycles in xsim
            if (i % 100 == 0) wait(1, SC_NS);
        }

        ap_done(); //! completed Kernel computation...
    }
};
