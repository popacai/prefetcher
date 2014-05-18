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

typedef struct queue_item {
    u_int32_t addrs;
    u_int32_t cycle;
}queue_item_t;


class Queue{
    private:
        queue_item_t data[MAX_QUEUE_SIZE];
    public:
        Queue();
        int push(queue_item);
        int pop(queue_item);
};

class Prefetcher {
  private:

	bool _ready;
	Request _nextReq;
    //Fill the map?
    //

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
