#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include "keystone.h"

int main(int argc, char** argv)
{
    keystone_init();

    size_t untrusted_size = 4*1024*1024;
    size_t freemem_size = 640*1024*1024;
    uintptr_t utm_ptr = (uintptr_t)DEFAULT_UNTRUSTED_PTR;

    Keystone enclave;
    Params params;

    params.setFreeMemSize(freemem_size);
    params.setUntrustedMem(utm_ptr, untrusted_size);

    enclave.init("./tut1-enclave", "../eyrie-rt", params);

    DefaultEdgeCallDispatcher dispatcher;
    enclave.registerOcallDispatch(&dispatcher);

    enclave.run();

    return 0;
}
