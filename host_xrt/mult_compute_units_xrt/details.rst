Multiple Compute Units XRT (XRT Native API's)
=============================================

This example explains how to create multiple instances of a kernel and
execute them concurrently using xrt native api's.

For the same kernel to be instantiated into multiple compute units,
``nk`` flag is used to specify the number of compute units in a ``.cfg``
config file as shown below:

::

   [connectivity]
   nk=vadd:4
