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

HistoryLog::HistoryLog(){
    this->pos = 0;
    memset(this->addrs, 0, sizeof(u_int32_t) * MAX_HISTORY_SIZE);
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
    int i;
    for (i = 0; i < MAX_HEALTH_COUNT; i++) {
        this->mem_start_time[i] = 0;
        this->mem_end_time[i] = 0;
    }
    this->mem_start_pos = 0;
    this->mem_end_pos = 0;
   _ready = false; 
}
bool Prefetcher::hasRequest(u_int32_t cycle) { 
    queue_item q_temp;
    u_int64_t sum;
    u_int32_t avg_pending;
    sum = 0;

    int i;
    int count;
    count = 0;
    for (i = mem_end_pos; i != mem_start_pos; ){
        sum += cycle - mem_start_time[i];
        i++;
        if (i >= MAX_HEALTH_COUNT) {
            i = 0;
        }
        count++;
    }

    if (count != 0) {
        avg_pending = sum / count;
    }else{
        avg_pending = 0;
    }

    //printf("%u\n", avg_pending);

    _ready =  (this->prefetch_queue).has_request();
    /*
    if (_ready){

        if (avg_pending < 30){
            if (this->mem_queue.has_request()){
                q_temp = this->mem_queue.pop();
                this->prefetch_queue.push(q_temp);
            }
        }
        if (avg_pending > 2000) {
            return false;
        }
    }
    */
    return _ready;
}

Request Prefetcher::getRequest(u_int32_t cycle) { 
    Request req;
    queue_item q_item;
    //u_int32_t addr;

    //req.fromCPU = 0;

    q_item = this->prefetch_queue.pop();
    req.addr = q_item.addr;
    req.fromCPU = false;


    mem_start_time[mem_start_pos] = cycle;
    mem_start_pos++;
    if (mem_start_pos >= MAX_HEALTH_COUNT) {
        mem_start_pos = 0;
    }
    return req;
}

void Prefetcher::completeRequest(u_int32_t cycle) { 

    mem_end_time[mem_end_pos] = cycle;

    mem_end_time[mem_end_pos] = cycle;
    mem_end_pos++;
    if (mem_end_pos >= MAX_HEALTH_COUNT) {
        mem_end_pos = 0;
    }


    //_ready = false;
    //return this->prefetch_queue.has_request();
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

    //this->history.add_new(req.addr);

    if (req.fromCPU == true) {
        this->prefetch_queue.destroy();

        int stride;

        queue_item_t q_items[20];
        memset(q_items, 0, 20 * sizeof(queue_item_t));
        queue_item_t temp_item;
        int count;

        req.addr = trim_16(req.addr);
        req.fromCPU = false;


        //count = this->predictor.record(req.pc, req.addr, req.issuedAt, (q_items), req);
        count = 0;
        stride = count;
        count = 0;

        int step;
        max_fetch_number = 8;
        this->mem_queue.destroy();
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
            /*
            for (i = 0; i < max_fetch_number; i++) {
                temp_item.addr = addr + 32 * (i + max_fetch_number+ 1);
                temp_item.cycle = cycle;
                temp_item.HitL1 = req.HitL1;
                temp_item.HitL2 = req.HitL2;
                this->mem_queue.push(temp_item);
            }
            */
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

                /*
                    if (this->history.add_new(temp_addr)){
                        b_found = 1;
                    }
                    */
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
    } else {
        return;
    }

}

Predictor::Predictor()
{
    int i, j;
    for (i=0; i<MAX_PREDS_TABLE; i++)
    {
        memset(&(preds[i]), 0, sizeof(prediction));
        /*
        preds[i].pc = 0;
        preds[i].last_access = 0;
        */
        for (j = 0; j < PREDICTION_NUM; j++){
            //preds[i].nextaddr[j] = (PREDICTION_NUM - j) * 32;
            preds[i].nextaddr[j] = 0;
        }
    }
    this->prev_pc = 0;
    this->prev_addr = 0;
}



int Predictor::record(u_int32_t pc, u_int32_t addr, u_int32_t cycle, queue_item_t* q_items, Request req) {
    prediction* p; //p value
    p = 0;

    /*
    if (this->prev_pc == 0) {
        //pass

    }else{
        //if (addr != this->prev_addr)
    }
    */
    p = this->update(prev_pc, addr, cycle);

    //this->prev_pc = pc;
    //this->prev_addr = addr;

    //p = 0;
    //update previous addr and pc
    int max_fetch_number = PREDICT_LENGTH;
    if (p)
    {


//#define DEBUG 1
#ifdef DEBUG
        printf("%x\n", p->pc);
        int i;
        printf("\n-------------\npc  =%x\n", p->pc);
        printf("last=%x\n", p->last_access);
        printf("nextaddr=");
        for (i = 0; i < PREDICTION_NUM; i++){
            printf("%d,", p->nextaddr[i]);
        }
        printf("\n");
        printf("count=");
        for (i = 0; i < PREDICTION_NUM; i++){
            printf("%d,", p->count[i]);
        }
        printf("\n");
#endif

        int it;
        for (it = 0; it< max_fetch_number; it++){
            if (p->nextaddr[it] != 0){
                (q_items[it]).addr = p->nextaddr[it];
            }else{
                (q_items[it]).addr = p->nextaddr[it + 1];
            }
        }
        return it + 1;
    }else{
        int stride;
        /*
        max_fetch_number = 4;
        for (stride = 1; stride < max_fetch_number; stride++){
            (q_items[stride - 1]).addr = addr + stride * 32 ;
        }
        */
        stride = 0;
        return stride;
    }


}

int sort(prediction* t){
    u_int32_t* nextaddr;
    int* count;
    nextaddr = t->nextaddr;
    count = t->count;

    int i, j;
    u_int32_t tmp;
    for (i = 0; i < (PREDICTION_NUM - 1); i++){
        for (j = i; j < PREDICTION_NUM; j++){
            if (count[i] < count[j]){
                tmp = count[i];
                count[i] = count[j];
                count[j] = tmp;
            }
        }
    }
    return 0;
}

prediction* Predictor::update(u_int32_t pc, u_int32_t next_addr , u_int32_t cycle)
{
    int i,j,appeared;
    int tokick = 0;
    int tochange = -1;
    int min_count = -30000;
    int min_index = 0;

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
        preds[i].nextaddr[0] = next_addr;
        preds[i].count[0] = 1;

        for (j = 1; j < PREDICTION_NUM; j++)
        {
            preds[i].nextaddr[j] = 0;
            preds[i].count[j] = -1;
        }

    }

    else // update on an existing node
    {
        i = tochange;
        preds[i].last_access = cycle;

        appeared = 0; // if next_addr already appears in the list
        for (j = 0; j < PREDICTION_NUM; j++)
        {
            // this entry is this address
            if (preds[i].nextaddr[j] == next_addr)
            {
                if (preds[i].count[j] < 30000000)
                {
                    preds[i].count[j]++;
                }
                appeared = 1;
            }
        }
        
        for (j = 0; j < PREDICTION_NUM; j++)
        {
           //replace the entry with the address
            if (preds[i].count[j] <= 0 && !appeared)
            {
                preds[i].nextaddr[j] = next_addr;
                preds[i].count[j] = 1;
                appeared = 1;
            }
            //decrease the count of other addresses
            else if (preds[i].nextaddr[j] != next_addr)
            {
                if (preds[i].count[j] > -3000000){
                    preds[i].count[j]--;
                }
            }
        }
    }


    /*
    int k = 1;
    for (j = 0; j < PREDICTION_NUM; j++)
    {
        if (preds[i].count[j] <= 0)
        {
            preds[i].nextaddr[j] = prev_addr + 32 * k;
            preds[i].count[j] = -k;
            k++;
        }
    }
            */

    sort(&preds[i]);
    //prediction* p;
    //p = &preds[i];

    /*
    printf("\n-------------\npc  =%x\n", p->pc);
    printf("\n-------------\nlast=%x\n", p->last_access);
    printf("nextaddr=");
    for (i = 0; i < PREDICTION_NUM; i++){
        printf("%x,", p->nextaddr[i]);
    }
    printf("\n");
    printf("count=");
    for (i = 0; i < PREDICTION_NUM; i++){
        printf("%d,", p->count[i]);
    }
    printf("\n");
    */


    return &(preds[i]);

    
}  


/*void Predictor::update() {
}
void Predictor::record() {
}*/
