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
/**
 * @file enums.hpp
 * @brief Header of Common enums
 */
#ifndef XF_FINTECH_ENUMS_H
#define XF_FINTECH_ENUMS_H

namespace xf {
namespace fintech {

namespace enums {
/**
 * @brief Option Style
 */
enum OptionStyle {
    LongstaffSchwartz,
    American,
    European,
    Digital,
    BarrierBiased,
    BarrierNoBiased,
    Cliquet,
    Asian_AP,
    Asian_AS,
    Asian_GP,
    EuropeanBypass
};
} // namespace enums
using namespace enums;
} // namespace fintech
} // namespace xf

#endif // define XF_FINTECH_ENUMS_H
