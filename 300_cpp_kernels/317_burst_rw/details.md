Burst Read/Write (C)
======================

The AXI4 master interface can be used for array or pointer arguments, which is implemented in one of the following modes:

	 1. Individual data transfers
	 2. Burst mode data transfers

This is a simple example of vector addition using AXI4-master interface for burst read and write

Maximum bandwidth can be used between kernel and global memory by performing 512-bit wide bursts.
With `burst mode` transfers, host application reads or writes data using a single base address followed
by multiple sequential data samples, which makes this mode capable of higher data throughput.

The INTERFACE pragma is used in the example to specify how RTL ports are created from the function definition during interface synthesis.

`#pragma HLS INTERFACE m_axi port=a  offset=slave bundle=gmem max_read_burst_length=256 max_write_burst_length=256`

The option `max_read_burst_length` is used for AXI4 (m_axi) interfaces to specify the maximum number of data values read during a burst transfer and,
similarly, `max_write_burst_length` is used for specifying the maximum number of data values written during a burst transfer.

Burst mode of operation is shown using a pipelined for loop which is used to transfer data to or from a top-level function(vadd) argument
specified with an AXI4 master interface. In this example, local memory(burstbuffer) is also being used to store data in the device for further computation.
```c
	     // Top-level Vector Addition function
	     void vadd(int *a, int size, int inc_value);

	     //Burst read using a pipelined for loop
	     read: for(int j=0; j < chunk_size; j++){
	     #pragma HLS PIPELINE II=1
	         burstbuffer[j] = a[i+j];
	     }
```
The for loops used have the following requirements to implement burst read/write:
- Pipeline the loop
- Access addresses in increasing order

This example also uses a pipelined for loop to implement burst writes:
```c
	     calc_write: for(int j=0; j < chunk_size; j++) {
	     #pragma HLS PIPELINE II=1
	         a[i+j] = burstbuffer[j];
	     }
```
__Note__: Only one read and one write is allowed in a for loop unless the ports are bundled in different AXI ports.

