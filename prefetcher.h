/*
 *
 * File: prefetcher.h
 * Author: Sat Garcia (sat@cs)
 * Description: Header file for prefetcher implementation
 *
 */


#ifndef PREFETCHER_H
#define PREFETCHER_H

#include <sys/types.h>
#include "mem-sim.h"



#define MAX_QUEUE_SIZE 10
#include <queue>
using namespace std;

/* 4Bytes */
typedef struct queue_item {
    u_int32_t addr;                     
    //u_int32_t cycle;
    //bool HitL1;
    //bool HitL2;
}queue_item_t;

//48 best
#define MAX_HISTORY_SIZE 48 

class HistoryLog{
    private:
        int pos;                           //4 bytes
        u_int32_t addrs[MAX_HISTORY_SIZE]; //48 * 4 = 192 Bytes
    public:
        HistoryLog();
        int add_new(u_int32_t addr);
        int check_addr(u_int32_t addr);
        int remove_addr(u_int32_t addr);
};


class Queue{ // Size = MAX_QUEUE_SIZE * queue_size = 2 * 10 * 4 = 40 Bytes
    private:
        queue<queue_item_t> data; // MAX_QUEUE_SIZE
        queue<queue_item_t> temp; // MAX_QUEUE_SIZE, only for swap
        
    public:
        Queue();
        int push(queue_item);
        queue_item_t pop(int index = 0);
        int has_request();
        int destroy();
        queue<queue_item_t> recovery();
};


class Prefetcher { // total => 282 Bytes
  private:

	bool _ready;
    //
    Queue prefetch_queue;   //80 Bytes

    HistoryLog history; // 192 Bytes
  public:
	Prefetcher();


	// should return true if a request is ready for this cycle
	bool hasRequest(u_int32_t cycle);

	// request a desired address be brought in
	Request getRequest(u_int32_t cycle);

	// this function is called whenever the last prefetcher request was successfully sent to the L2
	void completeRequest(u_int32_t cycle);

	/*
	 * This function is called whenever the CPU references memory.
	 * Note that only the addr, pc, load, issuedAt, and HitL1 should be considered valid data
	 */
	void cpuRequest(Request req); 
};

#endif
