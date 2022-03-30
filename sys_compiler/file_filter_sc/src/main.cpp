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

#include <stdio.h>
#include <fcntl.h> // O_RDWR, O_DIRECT, ...
#include <fstream> // tellg
#include <sys/time.h>
#include "filter.hpp"

inline int tvdiff(struct timeval* tv0, struct timeval* tv1) {
    return (tv1->tv_sec - tv0->tv_sec) * 1000000 + (tv1->tv_usec - tv0->tv_usec);
}

// for SSD P2P change this to the host mounted SSD card directory
#define DATA "/tmp/data"

size_t filesize(const char* filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    assert(in.good());
    return in.tellg();
}

void create_input_files(int numIter, int numChunks, int chunkSz, int dice) {
    // create a unique input file for each iteration, for each cunk
    for (int iter = 0; iter < numIter; ++iter) {
        for (int chunk = 0; chunk < numChunks; ++chunk) {
            std::stringstream nm;
            nm << DATA << iter << '-' << chunk << ".orig";
            printf("writing %s (%dB)\n", nm.str().c_str(), chunkSz * sizeof(int));
            fflush(0);
            int fd = open(nm.str().c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            assert(fd > 2);
            int* in = new int[chunkSz];
            for (int i = 0; i < chunkSz; i++) {
                in[i] = rand() % dice;
            }
            write(fd, in, chunkSz * sizeof(int));
            close(fd);
            delete[] in;
        }
    }
    printf("Done writing\n");
}

int* read_file(const char* nm, int& cnt) {
    // read the contents of given file into a allocated buffer
    size_t sz = filesize(nm);
    printf("reading %s (%luB)...\n", nm, sz);
    fflush(0);
    assert(sz % sizeof(int) == 0);
    cnt = sz / sizeof(int);
    int* data = new int[cnt];
    int fd = open(nm, O_RDONLY);
    assert(fd > 2);
    int rc = read(fd, data, sz);
    assert(rc == sz);
    close(fd);
    return data;
}

void verify(int numIter, int numChunks, bool p2p) {
    // verify that for each iteration the filtered output file equals the original input
    // chunk files, but with all zero's filtered out.
    for (int iter = 0; iter < numIter; iter++) {
        std::stringstream fnm;
        fnm << DATA << iter << ".filt";
        int fi = 0, fcnt;
        int* filt = read_file(fnm.str().c_str(), fcnt);
        for (int chunk = 0; chunk < numChunks; ++chunk) {
            std::stringstream nm;
            nm << DATA << iter << '-' << chunk << ".orig";
            int ocnt;
            int* orig = read_file(nm.str().c_str(), ocnt);
            for (int oi = 0; oi < ocnt; ++oi) {
                if (orig[oi] != 0) {
                    if (filt[fi] != orig[oi]) {
                        printf("ERROR: MISMATCH iter %d chunk %d: %d (@%d) != %d (@%d)\n", iter, chunk, filt[fi], fi,
                               orig[oi], oi);
                        abort();
                    }
                    ++fi;
                }
            }
            if (p2p) {
                // filt numChunks are aligned to 4k, must skip to aligned boundary
                fi = (fi + 0xfff) & ~0xfff;
            }
            delete[] orig;
        }
        if (fi < fcnt) {
            printf("ERROR: MISMATCH: filt (@%d < %d)\n", fi, fcnt);
            abort();
        }
        delete[] filt;
    }

    printf("TESTCASE PASSED\n");
}

struct job_t {
    int iter;
    int ofd;
};

///
/// For num_iter times:
/// Read all numChunks files of random values between zero (incl) and dice (excl),
/// filter out all zero values, and write the cummulated result in an output file
///

void filter(int numIter, int numChunks, int chunkSz, int dice, bool p2p) {
    auto inBP = xfilter::create_bufpool(vpp::input, p2p ? vpp::p2p : vpp::h2c);
    auto outBP = xfilter::create_bufpool(vpp::output, p2p ? vpp::p2p : vpp::h2c);
    auto outSzBP = xfilter::create_bufpool(vpp::output);

    srand(numChunks * chunkSz + (dice << 2) + (numIter << 4));

    struct timeval tr0, tr1;
    gettimeofday(&tr0, 0);

    int rd_o_flags = O_RDONLY;
    if (p2p) rd_o_flags |= O_DIRECT;

    int wr_o_flags = O_WRONLY | O_CREAT | O_TRUNC;
    if (p2p) wr_o_flags |= O_DIRECT;

    int total_out_size = 0;

    xfilter::send_while([=, &total_out_size]() {
        static int iter = 0;

        int* in;
        // collect all numChunks input files into one "in" buffer
        for (int chunk = 0; chunk < numChunks; ++chunk) {
            std::stringstream nm;
            nm << DATA << iter << '-' << chunk << ".orig";
            int ifd = open(nm.str().c_str(), rd_o_flags);
            assert(ifd > 2);
            // map the file to "in" buffer at offset = chunk * chunkSz
            in = xfilter::file_buf<int>(inBP, ifd, chunkSz, 0, chunk * chunkSz);
        }
        // prepare output buffer to be able to hold all numChunks
        int* out = xfilter::file_buf<int>(outBP, 0, numChunks * chunkSz, 0);
        // outSz is an output buffer to provide the actual filtered size of each chunk
        int* outSz = xfilter::alloc_buf<int>(outSzBP, numChunks);

        std::stringstream nm;
        nm << DATA << iter << ".filt";
        // output file to hold all filtered numChunks
        int ofd = open(nm.str().c_str(), wr_o_flags, S_IRUSR | S_IWUSR);
        assert(ofd > 2);
        job_t job = {iter, ofd};
        xfilter::set_handle<job_t>(job);

        // provide lambda function to custom sync the outputs
        xfilter::custom_sync_outputs([ =, iter = iter, &total_out_size ]() {
            // first sync the filtered sizes
            auto fut = xfilter::sync_output<int>(outSz, numChunks, 0);
            // wait for the sync to complete
            int fd_offset = 0;
            fut.get();
            // now sync all the filtered chunks to the "out" buffer
            for (int chunk = 0; chunk < numChunks; ++chunk) {
                size_t sz = outSz[chunk];
                if (p2p) {
                    // when using p2p, we cannot store the filtered outputs back to back, because
                    // in p2p mode all transfers have to align to the file system block size (4k)
                    sz = (sz + 0xfff) & ~0xfff;
                }
                // sync the chunk to the out file at fd_offset (do not have to wait for it to complete)
                auto fut = xfilter::sync_output_to_file<int>(out, ofd, sz, fd_offset, chunk * chunkSz);
                fd_offset += sz;
                // for p2p mode, as an alternative to creating a gap (because of the required
                // alignment), we could avoid to create gaps by setting the next offset to the
                // actual: "fd_offset += outSz[chunk];". But then the above sync and the next
                // sync will be in a race condition writing data to that gap. This could be
                // prevented by adding a "fut.get()" here, but that would degrade performance.
            }
            total_out_size += fd_offset;
            printf("%d: filter size = %dB\n", iter, fd_offset);
        });

        xfilter::compute(numChunks, chunkSz, in, out, outSz);
        return (++iter < numIter);
    });

    xfilter::receive_all_in_order([=]() {
        job_t job = xfilter::get_handle<job_t>();
        close(job.ofd);
    });

    xfilter::join();

    gettimeofday(&tr1, 0);
    double total_ms = (double)tvdiff(&tr0, &tr1) / 1000;

    printf("total_time     = %9.3f ms\n", total_ms);
    printf("total_in_size  = %9.3f MB\n", (double)numIter * numChunks * chunkSz * sizeof(int) / (1 << 20));
    printf("in throughput  = %9.3f MB/s\n",
           (double)numIter * numChunks * chunkSz * sizeof(int) / total_ms * 1e3 / (1 << 20));
    printf("total_out_size = %9.3f MB\n", (double)total_out_size * sizeof(int) / (1 << 20));
    printf("out throughput = %9.3f MB/s\n", (double)total_out_size * sizeof(int) / total_ms * 1e3 / (1 << 20));
}

void usage(const char* main, const char* arg) {
    printf("ERROR: Unknown argument \"%s\"\n", arg);
    printf("Usage: %s [-p] [-x] [-c <numChunks>] [ -s <chunkSz>] [ -d <dice> ] [-n <numIter> ]\n", main);
}

int main(int argc, const char** argv) {
    int numChunks = 5;
    int chunkSz = 1024 * 1024 * 2;
    int dice = 10;
    int numIter = 4;
    if (vpp::flow == vpp::hw_emu) {
        numChunks = 2;
        chunkSz = 4096;
        dice = 10;
        numIter = 1;
    }
    bool create = true;
    bool p2p = false;
    for (int arg = 1; arg < argc; ++arg) {
        if (argv[arg][0] == '-' && argv[arg][2] == '\0') {
            switch (argv[arg][1]) {
                case 'c':
                    numChunks = atoi(argv[++arg]);
                    break;
                case 's':
                    chunkSz = atoi(argv[++arg]);
                    break;
                case 'd':
                    dice = atoi(argv[++arg]);
                    break;
                case 'n':
                    numIter = atoi(argv[++arg]);
                    break;
                case 'x':
                    create = false;
                    break;
                case 'p':
                    p2p = true;
                    break;
                default:
                    usage(argv[0], argv[arg]);
            }
        } else
            usage(argv[0], argv[arg]);
    }
    printf("Running filter with numChunks=%d, chunkSz=%d, dice=%d, numIter=%d in %s mode\n", numChunks, chunkSz, dice,
           numIter, p2p ? "P2P" : "H2C");
    if (p2p && chunkSz % 0x1000) {
        printf(
            "NOTE: the chunkSz (%d) is not aligned to the file system block size (%d), "
            "this will lead to runtime errors\n",
            chunkSz, 0x1000);
    }
    if (create) {
        create_input_files(numIter, numChunks, chunkSz, dice);
    } else {
        printf("Reusing existing input files\n");
    }
    filter(numIter, numChunks, chunkSz, dice, p2p);
    verify(numIter, numChunks, p2p);
}
