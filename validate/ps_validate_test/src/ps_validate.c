#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <syslog.h>
#include <sys/mman.h>
#include "xrt/xrt_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "sk_types.h"

// User private data structure container (context object) definition
class xrtHandles : public pscontext
{
public:
};


__attribute__((visibility("default")))
int hello_world(int *input, int *output, int count, struct xrtHandles *xrtHandle)
{
    openlog("new_kernel_source", LOG_PID | LOG_CONS | LOG_NDELAY, LOG_NEWS);
    syslog(LOG_INFO, "%s: Started new kernel\n", __func__);

    for (int i = 0; i < count; i++) {
      output[i] = input[i];
    }
    syslog(LOG_INFO, "input[%0d] = %d, output[%d] = %d\n",0,input[0],0,output[0]);

    syslog(LOG_INFO, "%s: Stopped new kernel\n", __func__);
    closelog();

    return 0;
}

#ifdef __cplusplus
}
#endif

