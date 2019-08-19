Critical Path
===============

This example demonstrates the considerations in coding style to avoid critical paths in kernels.

`Apply_watermark` kernel is processing the image's pixels concurrently by using `HLS UNROLL` however, a global variable `x` is being updated in every iteration which nullifies the speedup offered by unrolling the loop and leads to a critical path.

```c++
watermark:
       for (int i = 0; i < DATA_SIZE; i++, x++) {
          #pragma HLS UNROLL
          if (x > width) {
               x = x - width;
               y += 1;
           }

           uint w_idy = y % WATERMARK_HEIGHT;
           uint w_idx = x % WATERMARK_WIDTH;
           tmp.data[i] = saturatedAdd(tmp.data[i], watermark[w_idy][w_idx]);
       }
```

Using local variables and just referencing the value of `x` in every iteration and updating it outside the loop can remove this critical path 
and thus improve the performance and timing of kernel execution.

```c++
for (int i = 0; i < DATA_SIZE; i++) {
           #pragma HLS UNROLL
            uint tmp_x = x + i;
            uint tmp_y = y;
            if (tmp_x > width) {
                tmp_x = tmp_x - width;
                tmp_y += 1;
            }

            uint w_idy = tmp_y % WATERMARK_HEIGHT;
            uint w_idx = tmp_x % WATERMARK_WIDTH;
            tmp.data[i] = saturatedAdd(tmp.data[i], watermark[w_idy][w_idx]);
        }
        x += DATA_SIZE;       
```
