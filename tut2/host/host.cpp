#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include "keystone.h"
#include "rpc.h"

#define OCALL_GET_SERVER_EID 7
#define OCALL_GET_RECORD_SIZE 8

#define ENCLAVE_MAX_COUNT 8

#define RECORD_SIZE (1<<13)

static Keystone* enclaves[ENCLAVE_MAX_COUNT];
static int enclave_n;
static pid_t pid;

static void* slave_run(void* _d){
    enclaves[1]->run();
}

static int get_record_size_handler(Keystone* encalve, void* buffer, struct shared_region* shared_region){
    struct edge_call* edge_call = (struct edge_call*)buffer;
    uintptr_t data_section = edge_call_data_ptr(shared_region);
    *(size_t*)data_section = RECORD_SIZE;
    if(edge_call_setup_ret(edge_call,
                (void*)data_section, sizeof(size_t), shared_region)){
        edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    } else{
        edge_call->return_data.call_status = CALL_STATUS_OK;
    }
    return 0;
}

static int get_server_eid_handler(Keystone* enclave, void* buffer, struct shared_region* shared_region){
    struct edge_call* edge_call = (struct edge_call*)buffer;

    uintptr_t* data_section = (uintptr_t*)edge_call_data_ptr(shared_region);
    *data_section = enclaves[0]->getSID();
    if( edge_call_setup_ret(edge_call, (void*) data_section, \
                sizeof(int), shared_region)){
        edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
    }
    else{
        edge_call->return_data.call_status = CALL_STATUS_OK;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    size_t untrusted_size = 2*1024*1024;
    size_t freemem_size = 48*1024*1024;
    uintptr_t utm_ptr = (uintptr_t)DEFAULT_UNTRUSTED_PTR;

    Keystone enclave1, enclave2;
    Params params;

    params.setFreeMemSize(freemem_size);
    params.setUntrustedMem(utm_ptr, untrusted_size);

    enclave1.init("tut2-server", "../eyrie-rt", params); 

    enclave2.init("tut2-client", "../eyrie-rt", params);

    DefaultEdgeCallDispatcher dispatcher1, dispatcher2;
    enclave1.registerOcallDispatch(&dispatcher1);
    RPCServerInit(&dispatcher1, &enclave1);
    dispatcher1.register_call(OCALL_GET_RECORD_SIZE, get_record_size_handler, NULL);

    enclave2.registerOcallDispatch(&dispatcher2);
    RPCClientInit(&dispatcher2, &enclave2);
    dispatcher2.register_call(OCALL_GET_SERVER_EID, get_server_eid_handler, NULL);
    dispatcher2.register_call(OCALL_GET_RECORD_SIZE, get_record_size_handler, NULL);

    enclaves[0] = &enclave1;
    enclaves[1] = &enclave2;
    enclave_n = 2;

    pthread_t slave_thread;
    pthread_create(&slave_thread, 0, slave_run, NULL);

    enclaves[0]->run();

    pthread_join(slave_thread, NULL);

    return 0;
}
