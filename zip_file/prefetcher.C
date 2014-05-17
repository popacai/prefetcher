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
    int i;
    for (i = 0; i < MAX_QUEUE_SIZE; i++){
        memset(&(p_queue[i]), 0, sizeof(p_item_t));
    }
    previous_pc = 0;
    previous_addr = 0;

    _ready = false; 
}

int Prefetcher::find_pc(u_int32_t pc) {
    p_item_t* p;
    int free_p;
    free_p = 0;
    int i;

    int min_count;
    min_count = 1;
    for (i = 0; i < MAX_QUEUE_SIZE; i++){
        p = &(p_queue[i]);
        if (p->pc_addr == pc){
            return i;
        }
        if (p->count < min_count){
            free_p = i;
            min_count = p->count;
        }
    }
    return free_p;
}

void Prefetcher::decrease_count() {
    int i;
    p_item_t* p;
    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
        p = &(p_queue[i]);
        p->count--;
    }
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

    
    /*
    int pos;
    u_int32_t offset;
    p_item_t* p;
    pos = find_pc(previous_pc);
    p = &(p_queue[pos]);
    {
        if ((p->pc_addr == req.addr)) {
            _nextReq.addr = req.addr + p->offset;
            _ready = true;
            p->total++;
        }else{
            _nextReq.addr = req.addr + 16;
            _ready = true;
        }
    }

    //If hit the p->prediction
    p->pc_addr = previous_pc;
    p->count = MAX_QUEUE_SIZE; //init value
    p->offset = req.addr - previous_addr;
    p->offset = p->offset < 16 ? 16 : p->offset;


    previous_pc = req.pc;
    previous_addr = req.addr;

    this->decrease_count();
    */

    /*
	if(!_ready && !req.HitL1) {
		_nextReq.addr = req.addr + 16;
		_ready = true;
	}
    */
}
