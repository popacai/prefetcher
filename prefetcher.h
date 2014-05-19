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


#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE_SIZE 10
#include <queue>
using namespace std;

typedef struct prediction_t {
    int aa;
}prediction;


class Predicter{
    private:
        int a;//??
    public:
        void update();
        void record();
        void aaaa();
};



typedef struct queue_item {
    u_int32_t addr;
    u_int32_t cycle;
}queue_item_t;


class Queue{
    private:
        queue<queue_item_t> data;
    public:
        Queue();
        int push(queue_item);
        queue_item_t pop(int index = 0);
        int has_request();
        int destroy();
};

#endif

class Prefetcher {
  private:

	bool _ready;
	Request _nextReq;
    //Fill the map?
    //
    Queue prefetch_queue;

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
