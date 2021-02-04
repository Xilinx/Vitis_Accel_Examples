Validate Kernel (C)
================================

This is an extremely simple HLS C Kernel and accompanying host code to verify that the platform has basic functionality. It will make a call to the kernel with an empty global buffer. The kernel will then write the string of characters "Hello World\n\0" into the buffer and return. The host will copy this buffer locally and then print out the resulting buffer contents.
