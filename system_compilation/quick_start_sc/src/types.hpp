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

#pragma once
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef signed char I8;
typedef signed short I16;
typedef signed int I32;
struct YUVImage {
    unsigned char* yChannel;
    unsigned char* uChannel;
    unsigned char* vChannel;
};
