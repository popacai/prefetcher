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

Queue::Queue() {
    //pass
}

int Queue::push(queue_item_t item) {
    this->data.push(item);
    return 0;
}

queue_item_t Queue::pop(int index) {
    queue_item_t item;
    item = (queue_item_t)this->data.front();
    this->data.pop();
    return item;
}

int Queue::has_request() {
    return (!this->data.empty());
}

int Queue::destroy() {
    while (!(this->data).empty()) {
        (this->data).pop();
    }
    return 0;
}

Prefetcher::Prefetcher() {
   _ready = false; 
}
bool Prefetcher::hasRequest(u_int32_t cycle) { 
    _ready =  (this->prefetch_queue).has_request();
    return _ready;
}

Request Prefetcher::getRequest(u_int32_t cycle) { 
    Request req;
    queue_item q_item;
    //u_int32_t addr;

    //req.fromCPU = 0;

    q_item = this->prefetch_queue.pop();
    req.addr = q_item.addr;
    return req;
}

void Prefetcher::completeRequest(u_int32_t cycle) { 
    //_ready = false;
    //return this->prefetch_queue.has_request();
}

void Prefetcher::cpuRequest(Request req) { 
    queue_item_t q_item;

    this->prefetch_queue.destroy();

    q_item.addr = trim_16(req.addr + 32);
    this->prefetch_queue.push(q_item);
    q_item.addr = trim_16(req.addr + 32 + 16);
    this->prefetch_queue.push(q_item);
    q_item.addr = trim_16(req.addr + 32 + 32);
    this->prefetch_queue.push(q_item);
    q_item.addr = trim_16(req.addr + 32 + 32 + 16);
    this->prefetch_queue.push(q_item);
    q_item.addr = trim_16(req.addr + 32 + 32 + 32);
    this->prefetch_queue.push(q_item);
    q_item.addr = trim_16(req.addr + 32 + 32 + 32 + 16);
    this->prefetch_queue.push(q_item);
    q_item.addr = trim_16(req.addr + 32 + 32 + 32 + 32);
    this->prefetch_queue.push(q_item);

    //if (!_ready) {
        //_nextReq.addr = req.addr + 64;
        //_ready = true;
    //}

}

/*void Predictor::update() {
}
void Predictor::record() {
}*/
