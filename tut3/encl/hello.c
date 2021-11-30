#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include "string.h"
#include "syscall.h"
#include "malloc.h"

#ifndef PERM_R
#define PERM_R 1
#endif

#ifndef PERM_W
#define PERM_W 2
#endif

int main(){
    uid_t uid = elasticlave_create(4096);
    printf("%d\n", (int)uid);
    void* region;
    region = elasticlave_map(uid);
    printf("Region address: %lx\n", (uintptr_t)region); 
    elasticlave_change(uid, PERM_R | PERM_W);
    strcpy((char*)region, "Hello Elasticlave!");
    elasticlave_unmap(region);
    region = elasticlave_map(uid);
    printf("Region address (remapped): %lx\n", (uintptr_t)region); 
    printf("%s\n", (char*)region); 
    elasticlave_unmap(region);
    elasticlave_destroy(uid);

    return 0;
}

