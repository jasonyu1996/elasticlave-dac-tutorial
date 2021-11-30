#include<stdio.h>
#include "edge_common.h"
#include "eapp_utils.h"
#include "string.h"
#include "syscall.h"
#include "malloc.h"
#include "callnums.h"

static void *in_buf, *out_buf;
static uid_t in_uid, out_uid;
static size_t record_size;

static size_t ocall_get_record_size(){
    size_t rs;
    ocall(OCALL_GET_RECORD_SIZE, NULL, 0, &rs, sizeof(size_t));
    return rs;
}


static int set_buffer_handler(int eid, void* buffer, struct shared_region* shared_region){
    struct edge_call* edge_call = (struct edge_call*)buffer;

    uintptr_t call_args;
    size_t args_len;
    if(edge_call_args_ptr(edge_call, &call_args, &args_len, shared_region) != 0){
        edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
        return;
    }

    struct buffer_info* pk = (struct buffer_info*)call_args;
    uid_t uid = pk->uid;
    void* buf = elasticlave_map(uid);
    if(!buf)
        goto set_buffer_failed;


    switch(pk->buffer_type){
        case BUFFER_IN:
            in_uid = uid;
            in_buf = buf;
            break;
        case BUFFER_OUT:
            out_uid = uid;
            out_buf = buf;
            break;
        default:
            printf("Set buffer failed with type %d\n", pk->buffer_type);
            goto set_buffer_type_failed;
    }

    return 0;

set_buffer_type_failed:
    elasticlave_unmap(buf);
set_buffer_failed:
    return 0;
}

static int work_buffer_handler(int eid, void* buffer, struct shared_region* shared_region){
    struct edge_call* edge_call = (struct edge_call*)buffer;

    elasticlave_change(in_uid, 9);
    elasticlave_change(out_uid, 11);

    // dumb

    elasticlave_change(out_uid, 0);
    elasticlave_change(in_uid, 0);

    edge_call->return_data.call_status = CALL_STATUS_OK;

    return 0;
}

static int quit_handler(int eid, void* buffer, struct shared_region* shared_region)
{
    struct edge_call* edge_call = (struct edge_call*)buffer;
    edge_call->return_data.call_status = CALL_STATUS_OK;

    icall_server_stop();

    return 0;
}

int main(){
    printf("Server launched\n"); 
    record_size = ocall_get_record_size();
    printf("Server record size %d\n", (int)record_size); 

    icall_server_init();
    printf("Server inited\n"); 

    icall_server_register_handler(ICALL_QUIT, quit_handler);
    icall_server_register_handler(ICALL_WORK_BUFFER, work_buffer_handler);
    icall_server_register_handler(ICALL_SET_BUFFER, set_buffer_handler);

    printf("Server handlers register\n"); 

    icall_server_launch_async();

    printf("Server about to exit!\n");


    return 0;
}

