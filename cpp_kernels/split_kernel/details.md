Split Kernel
============

This example demonstrates how a complex kernel can be split to get better performance. Here streams are used to get better performance 
from a kernel performing three operations; boost filter, median filter and sketch filter  on an image.

 `Boost` and `median filter` operations are done on the same input and `sketch` operation needs the outputs from both these operations.

`Sketch_Bad` kernel performs these operations one after the other by populating the results from one function into a buffer and then
using it for the next stage.
```c++
readImg:
    for (int i = 0; i < width * height; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_width*c_height max=c_width*c_height
       #pragma HLS PIPELINE II=1
        image[i] = input[i];
    }

boostHeight:
    for (int row = 0; row < height; row++) {
       #pragma HLS LOOP_TRIPCOUNT min=height max=height
    boostWidth:
        for (int col = 0; col < width; col++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
           #pragma HLS PIPELINE
            getWindow(image,
                      rgbWindow,
                      row,
                      col,
                      width,
                      height); // Get pixels within 3x3 aperture

            // Boost Value of the 3x3 rgbWindow
            // getBoost() is defined in kernels/boost_helper.h
            temp_res[row * width + col] = getBoost(rgbWindow);
        }
    }

medianHeight:
    for (int row = 0; row < height; row++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_height max=c_height
    medianWidth:
        for (int col = 0; col < width; col++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
           #pragma HLS PIPELINE
            getWindow(image,
                      rgbWindow,
                      row,
                      col,
                      width,
                      height); // Get pixels within 3x3 aperture

            // Median of the 3x3 rgbWindow
            // getMedian() is defined in kernels/median_helper.h
            med_out[row * width + col] = getMedian(rgbWindow);
        }
    }
    
sketchLoop:
    for (int i = 0; i < width * height; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_width*c_height max=c_width*c_height
       #pragma HLS PIPELINE II=1
        // Sketch operation on the current pixel of boost and median outputs
        // Boost operation output is in temp_res and
        // Median operation output is in med_out.
        // getSketch() is defined in kernels/sketch_helper.h
        temp_res[i] = getSketch(temp_res[i], med_out[i]);
    }
```

`Sketch_GOOD` kernel implementation provides input to both Boost and Median filter stages via streams and both these functions can be
executed concurrently with `#HLS DATAFLOW`. Then Sketch operation is performed by taking output streams from previous two stages.
Streams also save the excessive memory usage required by temporary buffers for each operation and allow for concurrency.
```c++
 #pragma HLS DATAFLOW
 hls::stream<uint> boost_in("boost_inputstream"),
       med_in("median_inputstream");
  #pragma HLS STREAM variable=boost_in depth=16
  #pragma HLS STREAM variable=med_in depth=16

  hls::stream<uint> boost_out("boost_outputstream"),
  med_out("median_outputstream"), sketch_out("sketch_outputstream");
  #pragma HLS STREAM variable=boost_out depth=16
  #pragma HLS STREAM variable=med_out depth=16
  #pragma HLS STREAM variable=sketch_out depth=16

   int size = width * height;

   input_stage(input, boost_in, med_in, size);
   boost_stage(boost_in, boost_out, width, height);
   median_stage(med_in, med_out, width, height);
   sketch_stage(boost_out, med_out, sketch_out, size);
   output_stage(output, sketch_out, width, height);
```
