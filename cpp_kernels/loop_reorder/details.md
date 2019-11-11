Loop Reordering
================

This example demonstrates how changing the order of loop can improve the performance of the kernel. In this case, loop reordering helps us achieve better `Initiation Interval`(II).

If matrix multiplication is performed with the commented code as shown below, Initiation Interval achieved is 64.
```c++
// lreorder1: for (int i = 0; i < size; i++) {
//     lreorder2: for (int j = 0; j < size; j++) {
//     #pragma HLS PIPELINE
//         lreorder3: for (int k = 0; k < MAX_SIZE; k++) {
//             int result = (k == 0) ? 0 : temp_sum[j];
//             result += A[i][k] * B[k][j];
//             temp_sum[j] = result;
//             if (k== size -1) C[i][j] = result;
//         }
//     }
// }

```

With the reordered loop as shown below, II achieved is 1. Reordering avoids the adder tree for the sum(output) of a single output element. 


```c++
lreorder1:
    for (int i = 0; i < size; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
    lreorder2:
        for (int k = 0; k < size; k++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
           #pragma HLS PIPELINE II=1
        lreorder3:
            for (int j = 0; j < MAX_SIZE; j++) {
                int result = (k == 0) ? 0 : temp_sum[j];
                result += A[i][k] * B[k][j];
                temp_sum[j] = result;
                if (k == size - 1)
                    C[i][j] = result;
            }
        }
    }
```     
