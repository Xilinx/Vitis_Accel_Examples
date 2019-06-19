/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#include <CL/opencl.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

using std::array;
using std::pair;
using std::string;
using std::strstr;
using std::strtok;
using std::vector;

// Wrap any OpenCL API calls that return error code(cl_int)
// with the below macro to quickly check for an error
#define OCL_CHECK(call)                                                        \
    do {                                                                       \
        cl_int err = call;                                                     \
        if (err != CL_SUCCESS) {                                               \
            printf("Error from " #call ", error code is %d\n", err);           \
            exit(1);                                                           \
        }                                                                      \
    } while (0);

#define ENUM_CASE(ENUM)                                                        \
    case ENUM:                                                                 \
        printf(#ENUM "\n");                                                    \
        break

#define BITFIELD_SETUP(type)                                                   \
    auto value = convert<type>(field.data());                                  \
    printf("[ ")

#define BITFIELD_PRINT(FIELD)                                                  \
    if (value & FIELD) {                                                       \
        printf(#FIELD " ");                                                    \
    }

#define BITFIELD_END() printf("]\n")

pair<int, const char *> platform_info[] = {
    {CL_PLATFORM_PROFILE, "profile"},
    {CL_PLATFORM_VERSION, "version"},
    {CL_PLATFORM_NAME, "name"},
    {CL_PLATFORM_VENDOR, "vendor"},
    {CL_PLATFORM_EXTENSIONS, "extensions"}};

pair<int, const char *> device_info[] = {
    {CL_DEVICE_TYPE, "type"},
    {CL_DEVICE_VENDOR_ID, "vendor id"},
    {CL_DEVICE_MAX_COMPUTE_UNITS, "max compute units"},
    {CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, "max work item dimensions"},
    {CL_DEVICE_MAX_WORK_GROUP_SIZE, "max work group size"},
    {CL_DEVICE_MAX_WORK_ITEM_SIZES, "max work item sizes"},
    {CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, "preferred vector width char"},
    {CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, "preferred vector width short"},
    {CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, "preferred vector width int"},
    {CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, "preferred vector width long"},
    {CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, "preferred vector width float"},
    {CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, "preferred vector width double"},
    {CL_DEVICE_MAX_CLOCK_FREQUENCY, "max clock frequency"},
    {CL_DEVICE_ADDRESS_BITS, "address bits"},
    {CL_DEVICE_MAX_READ_IMAGE_ARGS, "max read image args"},
    {CL_DEVICE_MAX_WRITE_IMAGE_ARGS, "max write image args"},
    {CL_DEVICE_MAX_MEM_ALLOC_SIZE, "max mem alloc size"},
    {CL_DEVICE_IMAGE2D_MAX_WIDTH, "image2d max width"},
    {CL_DEVICE_IMAGE2D_MAX_HEIGHT, "image2d max height"},
    {CL_DEVICE_IMAGE3D_MAX_WIDTH, "image3d max width"},
    {CL_DEVICE_IMAGE3D_MAX_HEIGHT, "image3d max height"},
    {CL_DEVICE_IMAGE3D_MAX_DEPTH, "image3d max depth"},
    {CL_DEVICE_IMAGE_SUPPORT, "image support"},
    {CL_DEVICE_MAX_PARAMETER_SIZE, "max parameter size"},
    {CL_DEVICE_MAX_SAMPLERS, "max samplers"},
    {CL_DEVICE_MEM_BASE_ADDR_ALIGN, "mem base addr align"},
    {CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, "min data type align size"},
    {CL_DEVICE_SINGLE_FP_CONFIG, "single fp config"},
    {CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, "global mem cache type"},
    {CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, "global mem cacheline size"},
    {CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, "global mem cache size"},
    {CL_DEVICE_GLOBAL_MEM_SIZE, "global mem size"},
    {CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, "max constant buffer size"},
    {CL_DEVICE_MAX_CONSTANT_ARGS, "max constant args"},
    {CL_DEVICE_LOCAL_MEM_TYPE, "local mem type"},
    {CL_DEVICE_LOCAL_MEM_SIZE, "local mem size"},
    {CL_DEVICE_ERROR_CORRECTION_SUPPORT, "error correction support"},
    {CL_DEVICE_PROFILING_TIMER_RESOLUTION, "profiling timer resolution"},
    {CL_DEVICE_ENDIAN_LITTLE, "endian little"},
    {CL_DEVICE_AVAILABLE, "available"},
    {CL_DEVICE_COMPILER_AVAILABLE, "compiler available"},
    {CL_DEVICE_EXECUTION_CAPABILITIES, "execution capabilities"},
    {CL_DEVICE_QUEUE_PROPERTIES, "queue properties"},
    {CL_DEVICE_NAME, "name"},
    {CL_DEVICE_VENDOR, "vendor"},
    {CL_DRIVER_VERSION, "version"},
    {CL_DEVICE_PROFILE, "profile"},
    {CL_DEVICE_VERSION, "version"},
    {CL_DEVICE_EXTENSIONS, "extensions"},
    {CL_DEVICE_PLATFORM, "platform"},
    {CL_DEVICE_DOUBLE_FP_CONFIG, "double fp config"},
    {CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, "preferred vector width half"},
    {CL_DEVICE_HOST_UNIFIED_MEMORY, "host unified memory"},
    {CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, "native vector width char"},
    {CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, "native vector width short"},
    {CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, "native vector width int"},
    {CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, "native vector width long"},
    {CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, "native vector width float"},
    {CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, "native vector width double"},
    {CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, "native vector width half"},
    {CL_DEVICE_OPENCL_C_VERSION, "opencl c version"},
    {CL_DEVICE_LINKER_AVAILABLE, "linker available"},
    {CL_DEVICE_BUILT_IN_KERNELS, "built in kernels"},
    {CL_DEVICE_IMAGE_MAX_BUFFER_SIZE, "image max buffer size"},
    {CL_DEVICE_IMAGE_MAX_ARRAY_SIZE, "image max array size"},
    {CL_DEVICE_PARENT_DEVICE, "parent device"},
    {CL_DEVICE_PARTITION_MAX_SUB_DEVICES, "partition max sub devices"},
    {CL_DEVICE_PARTITION_PROPERTIES, "partition properties"},
    {CL_DEVICE_PARTITION_AFFINITY_DOMAIN, "partition affinity domain"},
    {CL_DEVICE_PARTITION_TYPE, "partition type"},
    {CL_DEVICE_REFERENCE_COUNT, "reference count"},
    {CL_DEVICE_PREFERRED_INTEROP_USER_SYNC, "preferred interop user sync"},
    {CL_DEVICE_PRINTF_BUFFER_SIZE, "printf buffer size"}};

template <typename T, size_t N> int sizeof_array(T (&)[N]) { return N; }

std::string field(1024, '\0');

void print_platform_info(cl_platform_id platform) {
    for (int i = 0; i < sizeof_array(platform_info); i++) {
        clGetPlatformInfo(platform,
                          platform_info[i].first,
                          field.size(),
                          (void *)field.data(),
                          nullptr);
        printf("platform %-11s: %s\n", platform_info[i].second, field.c_str());
    }
}

template <typename T> T convert(const char *data) {
    return *reinterpret_cast<const T *>(data);
}

void print_device_info(cl_device_id device) {
    for (int i = 0; i < sizeof_array(device_info); i++) {
        clGetDeviceInfo(device,
                        device_info[i].first,
                        field.size(),
                        (void *)field.data(),
                        nullptr);

        printf("  device %-32s: ", device_info[i].second);
        switch (device_info[i].first) {
        case CL_DEVICE_ADDRESS_BITS:
        case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
        case CL_DEVICE_MAX_CLOCK_FREQUENCY:
        case CL_DEVICE_MAX_COMPUTE_UNITS:
        case CL_DEVICE_MAX_CONSTANT_ARGS:
        case CL_DEVICE_MAX_READ_IMAGE_ARGS:
        case CL_DEVICE_MAX_SAMPLERS:
        case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
        case CL_DEVICE_MAX_WRITE_IMAGE_ARGS:
        case CL_DEVICE_MEM_BASE_ADDR_ALIGN:
        case CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF:
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR:
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT:
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_INT:
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG:
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT:
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE:
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF:
        case CL_DEVICE_VENDOR_ID:
        case CL_DEVICE_PARTITION_MAX_SUB_DEVICES:
        case CL_DEVICE_REFERENCE_COUNT:
            printf("%d\n", convert<cl_uint>(field.data()));
            break;

        case CL_DEVICE_AVAILABLE:
        case CL_DEVICE_COMPILER_AVAILABLE:
        case CL_DEVICE_ENDIAN_LITTLE:
        case CL_DEVICE_ERROR_CORRECTION_SUPPORT:
        case CL_DEVICE_IMAGE_SUPPORT:
        case CL_DEVICE_HOST_UNIFIED_MEMORY:
        case CL_DEVICE_LINKER_AVAILABLE:
        case CL_DEVICE_PREFERRED_INTEROP_USER_SYNC:
            printf("%s\n", convert<cl_bool>(field.data()) ? "true" : "false");
            break;

        case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:
        case CL_DEVICE_GLOBAL_MEM_SIZE:
        case CL_DEVICE_LOCAL_MEM_SIZE:
        case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:
        case CL_DEVICE_MAX_MEM_ALLOC_SIZE:
            printf("%lu\n", convert<cl_ulong>(field.data()));
            break;

        case CL_DEVICE_IMAGE2D_MAX_HEIGHT:
        case CL_DEVICE_IMAGE2D_MAX_WIDTH:
        case CL_DEVICE_IMAGE3D_MAX_DEPTH:
        case CL_DEVICE_IMAGE3D_MAX_HEIGHT:
        case CL_DEVICE_IMAGE3D_MAX_WIDTH:
        case CL_DEVICE_MAX_PARAMETER_SIZE:
        case CL_DEVICE_MAX_WORK_GROUP_SIZE:
        case CL_DEVICE_PROFILING_TIMER_RESOLUTION:
        case CL_DEVICE_PRINTF_BUFFER_SIZE:
        case CL_DEVICE_IMAGE_MAX_BUFFER_SIZE:
        case CL_DEVICE_IMAGE_MAX_ARRAY_SIZE:
            printf("%zu\n", convert<size_t>(field.data()));
            break;

        case CL_DEVICE_MAX_WORK_ITEM_SIZES: {
            size_t max_dim = 0;
            clGetDeviceInfo(device,
                            CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                            sizeof(size_t),
                            &max_dim,
                            nullptr);
            printf("[  ");
            for (int i = 0; i < (int)max_dim; i++) {
                printf("\b%zu  ", convert<size_t>(&field[i * sizeof(size_t)]));
            }
            printf("\b]\n");
        } break;

        case CL_DEVICE_EXTENSIONS:
        case CL_DEVICE_NAME:
        case CL_DEVICE_PROFILE:
        case CL_DEVICE_VENDOR:
        case CL_DEVICE_VERSION:
        case CL_DRIVER_VERSION:
        case CL_DEVICE_BUILT_IN_KERNELS:
        case CL_DEVICE_OPENCL_C_VERSION:
            printf("%s\n", field.data());
            break;

        case CL_DEVICE_TYPE:
            switch (convert<cl_device_type>(field.data())) {
                ENUM_CASE(CL_DEVICE_TYPE_ACCELERATOR);
                ENUM_CASE(CL_DEVICE_TYPE_CPU);
                ENUM_CASE(CL_DEVICE_TYPE_GPU);
                ENUM_CASE(CL_DEVICE_TYPE_DEFAULT);
            default:
                printf("UNKNOWN\n");
            }
            break;
        case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:
            switch (convert<cl_device_mem_cache_type>(field.data())) {
                ENUM_CASE(CL_NONE);
                ENUM_CASE(CL_READ_ONLY_CACHE);
                ENUM_CASE(CL_READ_WRITE_CACHE);
            default:
                printf("UNKNOWN\n");
            }
            break;
        case CL_DEVICE_LOCAL_MEM_TYPE:
            switch (convert<cl_device_mem_cache_type>(field.data())) {
                ENUM_CASE(CL_LOCAL);
                ENUM_CASE(CL_GLOBAL);
            default:
                printf("UNKNOWN\n");
            }
            break;
        case CL_DEVICE_PARTITION_PROPERTIES:
            switch (convert<cl_device_mem_cache_type>(field.data())) {
            case 0:
                printf("NONE\n");
                break;
                ENUM_CASE(CL_DEVICE_PARTITION_EQUALLY);
                ENUM_CASE(CL_DEVICE_PARTITION_BY_COUNTS);
                ENUM_CASE(CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN);
            default:
                printf("UNKNOWN\n");
            }
            break;
        case CL_DEVICE_QUEUE_PROPERTIES: {
            BITFIELD_SETUP(cl_command_queue_properties);
            BITFIELD_PRINT(CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
            BITFIELD_PRINT(CL_QUEUE_PROFILING_ENABLE);
            BITFIELD_END();
        } break;
        case CL_DEVICE_DOUBLE_FP_CONFIG:
        case CL_DEVICE_HALF_FP_CONFIG:
        case CL_DEVICE_SINGLE_FP_CONFIG: {
            BITFIELD_SETUP(cl_device_fp_config);
            BITFIELD_PRINT(CL_FP_DENORM);
            BITFIELD_PRINT(CL_FP_INF_NAN);
            BITFIELD_PRINT(CL_FP_ROUND_TO_ZERO);
            BITFIELD_PRINT(CL_FP_ROUND_TO_INF);
            BITFIELD_PRINT(CL_FP_FMA);
            BITFIELD_PRINT(CL_FP_SOFT_FLOAT);
            BITFIELD_END();
        } break;
        case CL_DEVICE_EXECUTION_CAPABILITIES: {
            BITFIELD_SETUP(cl_device_exec_capabilities);
            BITFIELD_PRINT(CL_EXEC_KERNEL);
            BITFIELD_PRINT(CL_EXEC_NATIVE_KERNEL);
            BITFIELD_END();
        } break;
        case CL_DEVICE_PARTITION_AFFINITY_DOMAIN: {
            BITFIELD_SETUP(cl_device_affinity_domain);
            BITFIELD_PRINT(CL_DEVICE_AFFINITY_DOMAIN_NUMA);
            BITFIELD_PRINT(CL_DEVICE_AFFINITY_DOMAIN_L4_CACHE);
            BITFIELD_PRINT(CL_DEVICE_AFFINITY_DOMAIN_L3_CACHE);
            BITFIELD_PRINT(CL_DEVICE_AFFINITY_DOMAIN_L2_CACHE);
            BITFIELD_PRINT(CL_DEVICE_AFFINITY_DOMAIN_L1_CACHE);
            BITFIELD_PRINT(CL_DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE);
            BITFIELD_END();
        } break;
        case CL_DEVICE_PLATFORM: {
            auto platform = convert<cl_platform_id>(field.data());
            clGetPlatformInfo(platform,
                              CL_PLATFORM_NAME,
                              field.size(),
                              (void *)field.data(),
                              nullptr);
            printf("%s\n", field.c_str());
        } break;
        default:
            printf("N/A \n");
            continue;
        }
        field[0] = '\0';
    }
}

// This example prints devices available on this machine and their
// corresponding capabilities.
int main(int argc, char **argv) {

    // The following call retrieves the total number of platforms available
    cl_uint platform_count;
    OCL_CHECK(clGetPlatformIDs(0, nullptr, &platform_count));
    vector<cl_platform_id> platforms(platform_count);
    OCL_CHECK(clGetPlatformIDs(platform_count, platforms.data(), nullptr));
    for (int p = 0; p < (int)platform_count; ++p) {
        print_platform_info(platforms[p]);
        cl_uint device_count = 0;
        OCL_CHECK(clGetDeviceIDs(
            platforms[p], CL_DEVICE_TYPE_ALL, 0, nullptr, &device_count));
        vector<cl_device_id> devices(device_count);
        OCL_CHECK(clGetDeviceIDs(platforms[p],
                                 CL_DEVICE_TYPE_ALL,
                                 device_count,
                                 devices.data(),
                                 nullptr));
        for (int d = 0; d < (int)device_count; ++d) {
            clGetDeviceInfo(devices[d],
                            CL_DEVICE_NAME,
                            field.size(),
                            (void *)field.data(),
                            nullptr);
            printf("Device %d: %s\n", d, field.c_str());
            print_device_info(devices[d]);
            continue;
        }
    }
    printf("TEST PASSED\n");

    return EXIT_SUCCESS;
}
