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

int static_stride_list[20] = {1, -1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

u_int32_t trim_16(u_int32_t data){
    data = data / 16 * 16;
    return data;
    return data & 0xfffffff0;
}

HistoryLog::HistoryLog(){
    this->pos = 0;
    memset(this->addrs, 0, sizeof(u_int32_t) * MAX_HISTORY_SIZE);
}

int HistoryLog::check_addr(u_int32_t addr){
    int i;
    for (i = 0; i < MAX_HISTORY_SIZE; i++){
        if ((addr % (32 * 1024)) == (this->addrs[i] % (32 * 1024))){
            return -1;
        }
    }
    return 0;

}
int HistoryLog::add_new(u_int32_t addr){
    int i;
    for (i = 0; i < MAX_HISTORY_SIZE; i++){
        if (addr == this->addrs[i]){
            return -1;
        }
    }
    this->addrs[pos] = addr;

    pos++;
    if (pos >= MAX_HISTORY_SIZE){
        pos = 0;
    }
    return 0;
}

int HistoryLog::remove_addr(u_int32_t addr){
    int i;
    for (i = 0; i < MAX_HISTORY_SIZE; i++){
        if (addr == this->addrs[i]){
            this->addrs[i] = 0;
            return 1;
        }
    }
    return 0;
}

Queue::Queue() {
    //pass
}

int Queue::push(queue_item_t item) {
    if (this->data.size() > MAX_QUEUE_SIZE){
        return 1; // error 
    }
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
    queue_item_t item;

    //empty the temp;
    while (!(this->temp).empty()) {
        (this->temp).pop();
    }

    while (!(this->data).empty()) {
        item = (this->data).front();
        (this->data).pop();
        temp.push(item);
    }
    return 0;
}

queue<queue_item_t> Queue::recovery() {
    queue_item_t item;

    while (!(this->temp).empty()) {
        item = (this->temp).front();

        if ((this->data).size() < MAX_QUEUE_SIZE){
            (this->data).push(item);
        }else{
            break;
        }

        (this->temp).pop();
    }

    return this->temp;
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

    q_item = this->prefetch_queue.pop();
    req.addr = q_item.addr;
    req.fromCPU = false;

    return req;
}

void Prefetcher::completeRequest(u_int32_t cycle) { 

}

void Prefetcher::cpuRequest(Request req) { 
    queue_item_t q_item;
    int max_fetch_number;
    int stride;
    int i;
    u_int32_t addr;
    u_int32_t cycle;



    addr = req.addr;
    cycle = req.issuedAt;


    if (req.fromCPU == true) {
        this->prefetch_queue.destroy();

        int stride;

        queue_item_t q_items[20];
        memset(q_items, 0, 20 * sizeof(queue_item_t));
        queue_item_t temp_item;
        int count;

        req.fromCPU = false;


        count = 0;
        stride = count;
        count = 0;

        int step;
        max_fetch_number = 8;
        step = 0;
        int b_found;
        b_found = 0;
        u_int32_t temp_addr;
        int fetch_from_mem = 0;
        if (req.HitL1) {
            max_fetch_number = 7;
            fetch_from_mem = 1;
            /*
            q_items[stride++].addr = addr + 32;
            q_items[stride++].addr = addr - 32;
            q_items[stride++].addr = addr + 32 * 2;
            q_items[stride++].addr = addr + 32 * 3;
            q_items[stride++].addr = addr + 32 * 4;
            q_items[stride++].addr = addr + 32 * 5;
            q_items[stride++].addr = addr + 32 * 6;
            */
        }

        else if (req.HitL2) {
            max_fetch_number = 5;
            /*
            q_items[stride++].addr = addr + 32;
            q_items[stride++].addr = addr - 32;
            q_items[stride++].addr = addr + 32 * 2;
            q_items[stride++].addr = addr + 32 * 3;
            q_items[stride++].addr = addr + 32 * 4;
            */
        }

        else {
            max_fetch_number = 2;
            //q_items[stride++].addr = addr + 32;
            //q_items[stride++].addr = addr + 32 * 2;
            //q_items[stride++].addr = addr + 32 * 3;
            
        }

        for (;stride < max_fetch_number;){
                if (step > 19){
                    break;
                }
                temp_addr = addr + 32 * static_stride_list[step];
                b_found = 0;
                for (i = 0; i < max_fetch_number; i++){
                    if (temp_addr == q_items[i].addr){
                        b_found = 1;
                    }
                }

                if (!b_found){
                    q_items[stride].addr = temp_addr;
                    stride++;
                }
                step++;
            }

        count = stride;

        for (i = 0; i < (count - 1); i++){
            if (fetch_from_mem)
            {
                if (this->history.add_new(q_items[i].addr)){
                    continue;
                }
            }
            this->prefetch_queue.push(q_items[i]);
        }

        this->prefetch_queue.recovery();
        return;
    } else {
        return;
    }

}
