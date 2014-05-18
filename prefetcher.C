/*
 *
 * File: prefetcher.C
 * Author: Sat Garcia (sat@cs)
 * Description: This simple prefetcher waits until there is a D-cache miss then 
 * requests location (addr + 16), where addr is the address that just missed 
 * in the cache.
 *
 */

#include "prefetcher.h"
#include <stdio.h>
#include <memory.h>

u_int32_t trim_16(u_int32_t data){
    return data & 0xfffffff0;
}

u_int32_t trim_32(u_int32_t data){
    return data & 0xffffffe0;
}

u_int32_t trim_64(u_int32_t data){
    return data & 0xffffffc0;
}

u_int32_t trim_128(u_int32_t data){
    return data & 0xffffffe0;
}

Prefetcher::Prefetcher() {
   _ready = false; 
}
bool Prefetcher::hasRequest(u_int32_t cycle) { 
    return _ready; 
}

Request Prefetcher::getRequest(u_int32_t cycle) { 
    return _nextReq; 
}

void Prefetcher::completeRequest(u_int32_t cycle) { 
    _ready = false; 
}

void Prefetcher::cpuRequest(Request req) { 
    /*
    if (!req.HitL1){
        printf("-----------------------------------\n");
        printf("addr=%u\n", req.addr);
        printf("pc=%u\n", req.pc);
        printf("load=%u\n", req.load);
        printf("fromCPU=%u\n", req.fromCPU);
        printf("issueAt=%u\n", req.issuedAt);
        printf("HitL1=%u\n", req.HitL1);
        printf("HitL2=%u\n", req.HitL2);
    }
    */
    //add the pc

    //if (!_ready) {
    {
        _nextReq.addr = req.addr + 64;
        _ready = true;
    }

}
