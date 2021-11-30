#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include "keystone.h"
#include "edge_dispatch.h"

int main(int argc, char** argv)
{
    keystone_init();

    size_t untrusted_size = 4*1024*1024;
    size_t freemem_size = 640*1024*1024;
    uintptr_t utm_ptr = (uintptr_t)DEFAULT_UNTRUSTED_PTR;

    printf("Untrusted size = %lu, Freemem size = %lu\n", untrusted_size, freemem_size);
    fflush(stdout);

    Keystone enclave;
    Params params;

    params.setFreeMemSize(freemem_size);
    params.setUntrustedMem(utm_ptr, untrusted_size);

    enclave.init("./tut3-enclave", "../eyrie-rt" , params);

    DefaultEdgeCallDispatcher dispatcher;
    enclave.registerOcallDispatch(&dispatcher);

    enclave.run();
    return 0;
}
