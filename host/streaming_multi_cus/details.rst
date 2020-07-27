Stream Multiple Compute Units
=============================

This is a simple vadd Kernel design in which multiple compute units of
kernel are directly communicating with host using stream interfaces.

For this examples, four compute units of a kernel is created.
``krnl_stream_vadd.ini`` file specifies number of compute units along
with name of each compute unit as below:

::

   [connectivity]
   nk=krnl_stream_vadd:4:vadd_1.vadd_2.vadd_3.vadd_4

This file is included to v++ command line using following statement:

::

   --config krnl_stream_vadd.ini 

Host is written configurable for number of compute units using top level
const variable ``NCU``:

::

   auto constexpr NCU = 4;

User just needs to change NCU to some other value to make the host code
ready for other numbers. But user still needs to manually update the
``krnl_stream_vadd.ini`` file for other configuration.

For multiple compute units case, host codes needs to create kernel
object using specific compute unit name as below:

.. code:: cpp

   std::string cu_id;
   std::string krnl_name = "krnl_stream_vadd";
   std::vector<cl::Kernel> krnls(NCU);
   for (int i = 0; i < NCU; i++) 
   {
       cu_id = std::to_string(i + 1);
       auto krnl_name_full = krnl_name + ":{" + "vadd_" + cu_id + "}"; //kernel name with compute unit name
       krnls[i] = cl::Kernel(program, krnl_name_full.c_str());
   }

Here compute unit should match to name provided in
``krnl_stream_vadd.ini`` file.

Once we have unique kernel object for each compute unit, host code can
easily creates stream object for respective kernel object and transfer
data accordingly. For More details about streaming kernel usage, please
refer streaming_simple example.
