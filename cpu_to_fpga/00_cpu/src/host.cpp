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

/*
This is a simple example of Matrix Multiplication.
*/

#include<iostream>
#include<stdlib.h>

//Array size to access
#define DATA_SIZE 32 

void mmult_cpu( int *in1,   // Input matrix 1
                int *in2,   // Input matrix 2
                int *out,   // Output matrix (out = A x B)
                int dim     // Matrix size of one dimension
              )  
{
    //Performs matrix multiplication out = in1 x in2
    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            for (int k = 0; k < dim; k++){
                out[i * dim + j] += in1[i * dim + k] * in2[k * dim  + j];
            }
        }
    }
}

int main(int argc, char** argv)
{
    int size = DATA_SIZE;
    size_t matrix_size_bytes = sizeof(int) * size * size;

    //Allocate memory
    int *source_in1 = (int *) malloc(matrix_size_bytes);
    int *source_in2 = (int *) malloc(matrix_size_bytes);
    int *source_cpu_results = (int *) malloc(matrix_size_bytes);

    //Creates the data
    for(int index = 0; index < size * size; index++){
        source_in1[index] = rand() % size;
        source_in2[index] = rand() % size;
        source_cpu_results[index] = 0;
    }

    //Function call to perform matrix multiplication
    mmult_cpu(source_in1, source_in2, source_cpu_results, size);

    std::cout << "Matrix Multiplication completed." << std::endl;

    return 0;
}
