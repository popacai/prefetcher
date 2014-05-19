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

#define MAX_PREDS_TABLE 1000
#define PREDICTION_NUM 3
typedef struct prediction_t {
	short nextaddr[PREDICTION_NUM];
	short count[PREDICTION_NUM];
    u_int32_t pc;
    u_int32_t last_access;
}prediction;


class Predictor{
    private:
        u_int32_t prev_pc;
        u_int32_t prev_addr;

        prediction preds[MAX_PREDS_TABLE];

    public:
    	//Every time accessing an address
    	prediction record(u_int32_t pc, u_int32_t addr, u_int32_t cycle);

    	//update the states inside predictor
        void update(u_int32_t pc, short diff);
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
    Predicter predicter;

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
