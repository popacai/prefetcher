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
/*Predictor::record(u_int32_t pc, u_int32_t addr, u_int32_t cycle)
{

}*/
int Queue::recovery() {
    queue_item_t item;

    while (!(this->temp).empty()) {
        item = (this->temp).front();
        (this->temp).pop();

        if ((this->data).size() < MAX_QUEUE_SIZE){
            (this->data).push(item);
        }else{
            continue;
        }
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

    int stride;

    queue_item_t q_items[10];
    int count;

    req.addr = trim_16(req.addr);


    count = this->predictor.record(req.pc, req.addr, req.issuedAt, (q_items));

    int i;
    for (i = 0; i < (count - 1); i++){
        this->prefetch_queue.push(q_items[i]);
    }
    /*
    int max_fetch_number = 7;
    for (stride = 1; stride < max_fetch_number; stride++)
    {
        q_item.addr = trim_16(req.addr + stride * 16 + 16);
        this->prefetch_queue.push(q_item);
    } 
    */
    this->prefetch_queue.recovery();
    return;


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

Predictor::Predictor()
{
    int i;
    for (i=0; i<MAX_PREDS_TABLE; i++)
    {
        memset(&(preds[i]), 0, sizeof(prediction));
        /*
        preds[i].pc = 0;
        preds[i].last_access = 0;
        */
    }
    this->prev_pc = 0;
    this->prev_addr = 0;
}


int Predictor::record(u_int32_t pc, u_int32_t addr, u_int32_t cycle, queue_item_t* q_items) {

    long diff;

    diff = addr - this->prev_addr;
    if (this->prev_addr == 0){
        diff = 0;
    }

    if (this->prev_pc == 0) {
        //pass
    }else{
        //this->update(prev_pc, diff, cycle);
    }

    //update previous addr and pc
    this->prev_pc = pc;
    this->prev_addr = addr;

    int stride;
    int max_fetch_number = 4;
    for (stride = 1; stride < max_fetch_number; stride++){
        (q_items[stride - 1]).addr = addr + stride * 32 ;
    }
    return stride;
}

prediction* Predictor::update(u_int32_t pc, short diff, u_int32_t cycle)
{
    int i,j,appeared;
    int tokick = 0;
    int tochange = -1;

    //Find the positions
    u_int32_t min = 0xffffffff;
    for (i=0; i<MAX_PREDS_TABLE; i++)
    {
        //Found the pc.
        if (preds[i].pc == pc)
        {
            tochange = i;
        }

        //If pc not found, then the entry to be replaced
        if (preds[i].last_access < min)
        {
            min = preds[i].last_access;
            tokick = i;
        }

    }

    if (tochange == -1) //insert new node
    {
        i = tokick;
        preds[i].pc = pc;
        preds[i].last_access = cycle;
        preds[i].nextaddr[0] = diff;
        preds[i].count[0] = 0;

        for (j = 1; j < PREDICTION_NUM; j++)
        {
            preds[i].nextaddr[j] = 0;
            preds[i].count[j] = 0;
        }
    }

    else // update on an existing node
    {
        i = tochange;
        preds[i].last_access = cycle;

        appeared = 0; // if diff already appears in the list
        for (j = 0; j < PREDICTION_NUM; j++)
        {
            // this entry is this address
            if (preds[i].nextaddr[j] == diff)
            {
                preds[i].count[j]++;
                appeared = 1;
            }
        }
        for (j = 0; j < PREDICTION_NUM; j++)
        {
           //replace the entry with the address
            if (preds[i].count[j] == 0 && !appeared)
            {
                preds[i].nextaddr[j] = diff;
                preds[i].count[j] = 1;
                appeared = 1;
            }
            //decrease the count of other addresses
            else
            {
                preds[i].count[j]--;
            }
        }
    }

    return &preds[i];
}  


/*void Predictor::update() {
}
void Predictor::record() {
}*/
