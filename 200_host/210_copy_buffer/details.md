Copy Buffer
============
This example illustrates the use of `clEnqueueCopyBuffer` API which is used to copy the contents of a buffer into another buffer.

Contents of `buffer_a` are copied into `buffer_b`.
```c++
OCL_CHECK(err, err = q.enqueueCopyBuffer(buffer_a, buffer_b,0,0, size_in_bytes));
```

 Then contents of these buffers are added and result is verified by the host(should be twice the elements of buffer_a).
 ```c++
 for (int i = 0; i < DATA_SIZE; i++) {
       int host_result = source_a[i] + source_a[i];
       if (source_results[i] != host_result) {
           printf(error_message.c_str(), i, host_result, source_results[i]);
           match = 1;
           break;
 ```

