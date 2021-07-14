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

// This program is created to test peer-to-peer data transfer b/w SSD and FPGA.
//
// Two test cases can be executed:
// Read test: SSD -> p2p -> FPGA -> kernel -> FPGA -> XDMA -> host
// Write test: host -> XDMA -> FPGA -> kernel -> FPGA -> p2p -> SSD
//
// The data movement is done in pipeline for optimized performance, which will
// be reported at the end of each test.
//
// In order to make sure the measurement of performance numbers are as
// accurate as possible, the sync points (via OpenCL events) are carefully
// designed so that no two operations will be running at the same time on the
// same DDR bank.
//
// One cycle of data process includes: p2p data transfer (p), kernel copy (c)
// and XDMA (x). Conceptually, x, c and p happens consecutively. The pipeline
// is designed so that c(n) and x(n) will happen in parallel with p(n+1).

#include "cmdlineparser.h"
#include <chrono>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <unistd.h>

#ifdef ASYNC_READ
#include <libaio.h>
#endif

#include "xcl2.hpp"

// Comment out below macro for testing P2P IO bandwidth b/w FPGA and SSD only.
#define FULLCYCLE

using namespace std;

// Amount of data for each kernel loop
const size_t kernel_unit = 64 * 1024;
// Total amount of data to be processed.
size_t total_size = 1024 * 1024 * 1024;
// Total number of chunks/pipelines of data.
const int num_chunks = 8;
// Number of bytes per kernel process.
size_t chunk_size = total_size / num_chunks;

int nvmeFd = -1;
cl_command_queue command_queue;
cl_context context;
chrono::high_resolution_clock::time_point p2pReadStart;
chrono::high_resolution_clock::time_point p2pReadEnd;
cl_ulong p2pReadTime = 0;

class Chunk {
   public:
    ~Chunk() {
        cl_int err;
        if (p2pEvt) OCL_CHECK(err, err = clReleaseEvent(p2pEvt));
        if (kernelEvt) OCL_CHECK(err, err = clReleaseEvent(kernelEvt));
        if (xdmaEvt) OCL_CHECK(err, err = clReleaseEvent(xdmaEvt));
        if (memsetEvt) OCL_CHECK(err, err = clReleaseEvent(memsetEvt));
        OCL_CHECK(err, err = clEnqueueUnmapMemObject(command_queue, p2pBo, p2pPtr, 0, nullptr, nullptr));
        clFinish(command_queue);
        OCL_CHECK(err, err = clReleaseMemObject(p2pBo));
        OCL_CHECK(err, err = clReleaseMemObject(hostBo));
        free(hostPtr);
    }

    Chunk(int idx) {
        id = idx;

        // Adjust buffer allocation flags based on idx.
        cl_mem_ext_ptr_t p2pBoExt = {XCL_MEM_EXT_P2P_BUFFER, nullptr, 0};
        cl_mem_ext_ptr_t hostBoExt = {0};
        hostPtr = (char*)aligned_alloc(4096, chunk_size);
        hostBoExt.obj = hostPtr;

        // Allocate BOs.
        p2pBo = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, chunk_size, &p2pBoExt, nullptr);
        hostBo = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR | CL_MEM_EXT_PTR_XILINX, chunk_size,
                                &hostBoExt, nullptr);

        // Map P2P device buffers to host access pointers
        p2pPtr = (char*)clEnqueueMapBuffer(command_queue, p2pBo, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, chunk_size, 0,
                                           nullptr, nullptr, nullptr);

        // Make sure data on disk is different than in memory.
        memset((void*)hostPtr, 'y', chunk_size);
        if (pwrite(nvmeFd, (void*)hostPtr, chunk_size, id * chunk_size) <= 0) {
            cout << "ERR: pwrite failed for chunk " << id << ", err: " << strerror(errno) << endl;
        }
        memset((void*)hostPtr, 'n', chunk_size);

        p2pEvt = clCreateUserEvent(context, nullptr);
        memsetEvt = clCreateUserEvent(context, nullptr);
        kernelEvt = xdmaEvt = nullptr;
        kernel = nullptr;
    }

    bool verify(void) {
        bool matched = true;
        void* tmp = aligned_alloc(4096, chunk_size);

        if (pread(nvmeFd, tmp, chunk_size, id * chunk_size) <= 0) {
            cout << "ERR: pread failed for chunk " << id << ", err: " << strerror(errno) << endl;
            matched = false;
        } else if (memcmp((void*)hostPtr, tmp, chunk_size) != 0) {
            matched = false;
        }

        free(tmp);

        if (!matched) cout << "ERR: verify failed for chunk " << id << endl;

        return matched;
    }

    cl_ulong p2pTime() { return (chrono::duration_cast<chrono::microseconds>(p2pEnd - p2pStart)).count(); }

    cl_ulong kernelTime() {
        cl_int err;
        cl_ulong s, e;

        OCL_CHECK(err, err = clGetEventProfilingInfo(kernelEvt, CL_PROFILING_COMMAND_START, sizeof(s), &s, nullptr));
        OCL_CHECK(err, err = clGetEventProfilingInfo(kernelEvt, CL_PROFILING_COMMAND_END, sizeof(e), &e, nullptr));
        return (e - s) / 1000;
    }

    cl_ulong xdmaTime() {
        cl_int err;
        cl_ulong s, e;

        OCL_CHECK(err, err = clGetEventProfilingInfo(xdmaEvt, CL_PROFILING_COMMAND_START, sizeof(s), &s, nullptr));
        OCL_CHECK(err, err = clGetEventProfilingInfo(xdmaEvt, CL_PROFILING_COMMAND_END, sizeof(e), &e, nullptr));
        return (e - s) / 1000;
    }

    int id;

    cl_mem p2pBo;
    cl_mem hostBo;
    char* hostPtr;
    char* p2pPtr;

    cl_event p2pEvt;
    cl_event kernelEvt;
    cl_event xdmaEvt;
    cl_event memsetEvt;

    cl_kernel kernel;

    chrono::high_resolution_clock::time_point p2pStart;
    chrono::high_resolution_clock::time_point p2pEnd;
} * chunks[num_chunks] = {0};

void report(string label, cl_ulong totalTime, cl_ulong curTime) {
    double total = total_size;

    total *= 1000000;     // convert us to s
    total /= 1024 * 1024; // convert to MB

    cout << setw(8) << label << "\t" << fixed << setprecision(2) << setw(8) << curTime << "ms\t" << setw(8)
         << float(curTime) * 100 / totalTime << "%\t" << setw(8) << total / curTime << "MB/s\t" << std::endl;
}

void setup_write_dependency(Chunk* c) {
    cl_int err;
    int size = chunk_size / sizeof(int);

    // The XDMA h2c cannot start until memset has done to populate
    // desired content in host mem.
    OCL_CHECK(err, err = clEnqueueMigrateMemObjects(command_queue, 1, &c->hostBo, 0, 1, &c->memsetEvt, &c->xdmaEvt));
    int narg = 0;
    clSetKernelArg(c->kernel, narg++, sizeof(cl_mem), &c->hostBo);
    clSetKernelArg(c->kernel, narg++, sizeof(cl_mem), &c->p2pBo);
    clSetKernelArg(c->kernel, narg++, sizeof(int), &size);

    // Kernel cannot start until the XDMA h2c has finished.
    clEnqueueTask(command_queue, c->kernel, 1, &c->xdmaEvt, &c->kernelEvt);
}

void setup_read_dependency(Chunk* c) {
    cl_int err;
    int size = chunk_size / sizeof(int);

    int narg = 0;
    clSetKernelArg(c->kernel, narg++, sizeof(cl_mem), &c->p2pBo);
    clSetKernelArg(c->kernel, narg++, sizeof(cl_mem), &c->hostBo);
    clSetKernelArg(c->kernel, narg++, sizeof(int), &size);

    // Kernel cannot start until the P2P read operation is done.
    clEnqueueTask(command_queue, c->kernel, 1, &c->p2pEvt, &c->kernelEvt);

    // XDMA c2h cannot start until the kernel is done.
    OCL_CHECK(err, err = clEnqueueMigrateMemObjects(command_queue, 1, &c->hostBo, CL_MIGRATE_MEM_OBJECT_HOST, 1,
                                                    &c->kernelEvt, &c->xdmaEvt));
}

void exec_write_test() {
    cout << "HOST -> FPGA(host BO) -> FPGA(p2p BO) -> SSD" << endl;
    // All memset for host mem is done as part of Chunk's constructor.
    // We use memset event as a trigger point to really start data
    // process for a single chunk.
    clSetUserEventStatus(chunks[0]->memsetEvt, CL_COMPLETE);

    for (int idx = 0; idx < num_chunks; idx++) {
        Chunk* c = chunks[idx];

        // P2P write to SSD cannot start until the kernel has finished.
        clWaitForEvents(1, &c->kernelEvt);

        // Kick off the process of data for the next chunk on another
        // DDR bank so that it can run in parallel with P2P write on
        // the current DDR BANK.
        if (idx < num_chunks - 1) {
            clSetUserEventStatus(chunks[idx + 1]->memsetEvt, CL_COMPLETE);
        }
        c->p2pStart = chrono::high_resolution_clock::now();
        // Now start P2P write to SSD.
        if (pwrite(nvmeFd, (void*)c->p2pPtr, chunk_size, c->id * chunk_size) <= 0) {
            cerr << "ERR: pwrite failed: "
                 << "index: " << idx << ", error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        c->p2pEnd = chrono::high_resolution_clock::now();
        clSetUserEventStatus(c->p2pEvt, CL_COMPLETE);
    }

    // When all P2P writes are done, the whole test is done.
    for (int idx = 0; idx < num_chunks; idx++) clWaitForEvents(1, &chunks[idx]->p2pEvt);
}

#ifdef ASYNC_READ
void exec_async_read_test(Chunk** chunks) {
    io_context_t myctx;
    memset(&myctx, 0, sizeof(myctx));
    struct io_event events[num_chunks];
    int queue_depth = 64;
    int q_created = io_setup(queue_depth, &myctx);
    if (q_created != 0) cout << "ERROR: Not Able to Create Queue For I/O Submission" << endl;
    struct iocb *iocbp[num_chunks], *io;
    int io_job_submit = 0;
    int chunkid_store[num_chunks];
    cout << "SSD -> FPGA(p2p BO) -> FPGA(host BO) -> HOST" << endl;
    p2pReadStart = chrono::high_resolution_clock::now();
    for (int idx = 0; idx < num_chunks; idx++) {
        Chunk* c = chunks[idx];
        iocbp[idx] = (struct iocb*)malloc(sizeof(struct iocb));
        io = iocbp[idx];
        io_prep_pread(io, nvmeFd, (void*)c->p2pPtr, chunk_size, idx * chunk_size);
        chunkid_store[idx] = idx;
        io->data = &chunkid_store[idx];
        c->p2pStart = chrono::high_resolution_clock::now();
        io_job_submit = io_submit(myctx, 1, &io);
        if (io_job_submit != 1) cout << "ERROR: Not Able to Submit Job for chunk id" << idx << endl;
    }
    int io_completion = 0;
    int* temp;
    int no_of_loop = 0;
    while (no_of_loop < num_chunks) {
        io_completion = io_getevents(myctx, 1, 1, events, nullptr);
        temp = (int*)events->data;
        Chunk* c = chunks[*temp];
        c->p2pEnd = chrono::high_resolution_clock::now();
        clSetUserEventStatus(c->p2pEvt, CL_COMPLETE);
        no_of_loop = no_of_loop + 1;
    }
    p2pReadEnd = chrono::high_resolution_clock::now();
    p2pReadTime = chrono::duration_cast<chrono::microseconds>(p2pReadEnd - p2pReadStart).count();

#ifdef FULLCYCLE
    for (int idx = 0; idx < num_chunks; idx++) clWaitForEvents(1, &chunks[idx]->xdmaEvt);
#else
    for (int idx = 0; idx < num_chunks; idx++) clWaitForEvents(1, &chunks[idx]->p2pEvt);
#endif
}
#endif

void exec_read_test() {
    cout << "SSD -> FPGA(p2p BO) -> FPGA(host BO) -> HOST" << endl;
    for (int idx = 0; idx < num_chunks; idx++) {
        Chunk* c = chunks[idx];

        // Start the process of data for each chunk by loading data
        // into P2P device memory first.
        c->p2pStart = chrono::high_resolution_clock::now();
        if (pread(nvmeFd, (void*)c->p2pPtr, chunk_size, c->id * chunk_size) <= 0) {
            cerr << "ERR: pread failed: "
                 << "index: " << idx << ", error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        c->p2pEnd = chrono::high_resolution_clock::now();

        // Kick off kernel and XDMA operation for this chunk.
        clSetUserEventStatus(c->p2pEvt, CL_COMPLETE);
    }
#ifdef FULLCYCLE
    for (int idx = 0; idx < num_chunks; idx++) clWaitForEvents(1, &chunks[idx]->xdmaEvt);
#else
    for (int idx = 0; idx < num_chunks; idx++) clWaitForEvents(1, &chunks[idx]->p2pEvt);
#endif
}

void usage() {
    cerr << "Options: <-m>  <r|w> <path-to-SSD> <-d> <device id>" << endl;
    exit(EXIT_FAILURE);
}

std::vector<unsigned char> readBinary(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (file) {
        file.seekg(0, std::ios::end);
        streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<unsigned char> buffer(size);
        file.read((char*)buffer.data(), size);
        return buffer;
    } else {
        return std::vector<unsigned char>(0);
    }
}

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--file_path", "-p", "file path string", "");
    parser.addSwitch("--input_file", "-f", "input file string", "");
    parser.addSwitch("--device", "-d", "device id", "0");
    parser.addSwitch("--mode", "-m", "mode r/w", "r");
    parser.parse(argc, argv);

    // Read settings
    auto binaryFile = parser.value("xclbin_file");
    std::string filepath = parser.value("file_path");
    std::string filename;
    cl_uint dev_id = stoi(parser.value("device"));
    auto mode = parser.value("mode");

    if (xcl::is_emulation()) {
        total_size = 512 * 1024;
        chunk_size = total_size / num_chunks;
    }

    if (argc < 5) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    if (filepath.empty()) {
        std::cout << "\nWARNING: As file path is not provided using -p option, going with -f option which is local "
                     "file testing. Please use -p option, if looking for actual p2p operation on NVMe drive.\n";
        filename = parser.value("input_file");
    } else {
        std::cout << "\nWARNING: Ignoring -f option when -p options is set. -p has high precedence over -f.\n";
        filename = filepath;
    }

    bool isWrite;

    if (mode == "r")
        isWrite = false;
    else if (mode == "w")
        isWrite = true;
    else
        usage();

    // Each chunk of data needs to be multiple of 64k, which is one
    // process unit for each kernel run.
    if (chunk_size < kernel_unit || (chunk_size % kernel_unit) != 0) {
        cerr << "Data chunk size is misaligned" << endl;
        return EXIT_FAILURE;
    }

    // Get access to the NVME SSD.
    nvmeFd = open(filename.c_str(), O_RDWR | O_DIRECT);
    if (nvmeFd < 0) {
        cerr << "ERR: open " << filename << "failed: " << strerror(errno) << endl;
        return EXIT_FAILURE;
    }
    cout << "INFO: Successfully opened NVME SSD " << filename << endl;

    cl_int err = CL_SUCCESS;
    int error;
    cl_platform_id platform = nullptr;
    error = clGetPlatformIDs(1, &platform, nullptr);

    cl_uint num_devices = 0;
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ACCELERATOR, 0, nullptr, &num_devices);
    if (error != CL_SUCCESS) {
        printf("Error: no devices\n");
        exit(EXIT_FAILURE);
    }
    std::vector<cl_device_id> devices(num_devices);
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ACCELERATOR, num_devices, devices.data(), nullptr);
    if (error != CL_SUCCESS) {
        printf("Error: could not determine device name\n");
        exit(EXIT_FAILURE);
    }
    cl_device_id device = devices.front();
    if (dev_id <= num_devices)
        device = devices[dev_id];
    else
        cout << "The device_id provided using -d flag is outside the range of "
                "available devices\n";

    context = clCreateContext(0, 1, &device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) std::cout << "clCreateContext call: Failed to create a compute context" << err << std::endl;

    command_queue =
        clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
    if (err != CL_SUCCESS) std::cout << "clCreateCommandQueue call: Failed to create command queue" << err << std::endl;

    // Read xclbin and create program
    std::vector<unsigned char> binary = readBinary(binaryFile);
    size_t binary_size = binary.size();
    const unsigned char* binary_data = binary.data();
    cl_program program = clCreateProgramWithBinary(context, 1, &device, &binary_size, &binary_data, nullptr, &err);

#ifdef ASYNC_READ
    io_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    io_queue_init(128, &ctx);
#endif

    // Setting up OpenCL runtime environment.
    cl_kernel kernel = clCreateKernel(program, "copy", &err);

    // Create all data chunks and set up all OpenCL operation dependencies.
    cout << "INFO: Preparing " << total_size / 1024 << "KB test data "
         << "in " << num_chunks << " pipelines" << endl;
    // Running Synchronous P2P
    std::cout << "############################################################\n";
    std::cout << "                     Synchronous P2P                         \n";
    std::cout << "############################################################\n";
    for (int idx = 0; idx < num_chunks; idx++) {
        Chunk* c = new Chunk(idx);

        chunks[idx] = c;
        c->kernel = kernel;
#ifdef FULLCYCLE
        if (isWrite) {
            setup_write_dependency(c);
        } else {
            setup_read_dependency(c);
        }
#endif
    }

    // Start of computation.
    cout << "INFO: Kick off test" << endl;
    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
    if (isWrite) {
        exec_write_test();
    } else {
        exec_read_test();
    }
    // End of computation.
    cl_ulong totalTime =
        (chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start)).count();

    // Report data
    report("overall", totalTime, totalTime);
    cl_ulong p2pTime = 0;
    for (int idx = 0; idx < num_chunks; idx++) p2pTime += chunks[idx]->p2pTime();

    report("p2p", totalTime, p2pTime);

#ifdef FULLCYCLE
    cl_ulong kernelTime = 0;
    cl_ulong xdmaTime = 0;
    for (int idx = 0; idx < num_chunks; idx++) {
        kernelTime += chunks[idx]->kernelTime();
        xdmaTime += chunks[idx]->xdmaTime();
    }
    report("kernel", totalTime, kernelTime);
    report("XDMA", totalTime, xdmaTime);

    cout << "INFO: Evaluating test result" << endl;
    bool matched = true;
    for (int idx = 0; idx < num_chunks; idx++) {
        if (!chunks[idx]->verify()) matched = false;
    }
    cout << "INFO: Test " << (matched ? "passed" : "failed") << endl;
#endif

    // Clean up.
    for (int idx = 0; idx < num_chunks; idx++) delete chunks[idx];

#ifdef ASYNC_READ
    if (!isWrite) {
        // Running Asynchronous P2P
        std::cout << "############################################################\n";
        std::cout << "                     Asynchronous P2P                         \n";
        std::cout << "############################################################\n";
        for (int idx = 0; idx < num_chunks; idx++) {
            Chunk* c = new Chunk(idx);

            chunks[idx] = c;
            c->kernel = kernel;
#ifdef FULLCYCLE
            setup_read_dependency(c);
#endif
        }

        // Start of computation.
        cout << "INFO: Kick off test" << endl;
        start = chrono::high_resolution_clock::now();
        exec_async_read_test(chunks);
        // End of computation.
        totalTime = (chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start)).count();

        // Report data
        report("overall", totalTime, totalTime);
        report("p2p", totalTime, p2pReadTime);

#ifdef FULLCYCLE
        kernelTime = 0;
        xdmaTime = 0;
        for (int idx = 0; idx < num_chunks; idx++) {
            kernelTime += chunks[idx]->kernelTime();
            xdmaTime += chunks[idx]->xdmaTime();
        }
        report("kernel", totalTime, kernelTime);
        report("XDMA", totalTime, xdmaTime);

        cout << "INFO: Evaluating test result" << endl;
        matched = true;
        for (int idx = 0; idx < num_chunks; idx++) {
            if (!chunks[idx]->verify()) matched = false;
        }
        cout << "INFO: Test " << (matched ? "passed" : "failed") << endl;
#endif
        // Clean up.
        for (int idx = 0; idx < num_chunks; idx++) delete chunks[idx];
    }
#endif

    clReleaseKernel(kernel);
    clReleaseContext(context);
    clReleaseProgram(program);

    (void)close(nvmeFd);

    return EXIT_SUCCESS;
}
