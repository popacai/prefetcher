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

#define MAX_PREDS_TABLE 100
#define PREDICTION_NUM 4
#define MAX_HEALTH_COUNT 30

#define PREDICT_LENGTH 3
typedef struct queue_item {
    u_int32_t addr;
    u_int32_t cycle;
    bool HitL1;
    bool HitL2;
}queue_item_t;

typedef struct prediction_t {
	u_int32_t nextaddr[PREDICTION_NUM];
	int count[PREDICTION_NUM];
    u_int32_t pc;
    u_int32_t last_access;
}prediction;


	//size of (prediction) = (2 + 2) * PREDICTION_NUM + 4 + 4 
//4 * PREDICTION_NUM + 4 + 4 = 32 Bytes

//39KB / 32


class Predictor{
    private:
        u_int32_t prev_pc;
        u_int32_t prev_addr;

        prediction preds[MAX_PREDS_TABLE];

    public:
    	Predictor();
    	//Every time accessing an address
        void not_hit(u_int32_t pc, u_int32_t addr);

    	int record(u_int32_t pc, u_int32_t addr, u_int32_t cycle, queue_item_t* q_items, Request req);

    	//update the states inside predictor
        prediction*  update(u_int32_t pc, u_int32_t next_addr, u_int32_t cycle);
};


#define MAX_HISTORY_SIZE 40
class HistoryLog{
    private:
        int pos;
        u_int32_t addrs[MAX_HISTORY_SIZE];
    public:
        HistoryLog();
        int add_new(u_int32_t addr);
            
};




//4 * MAX_QUEUE_SIZE = 40 * 2 = 80 Bytes

class Queue{
    private:
        queue<queue_item_t> data; // MAX_QUEUE_SIZE
        queue<queue_item_t> temp; // MAX_QUEUE_SIZE, only for swap
    public:
        Queue();
        int push(queue_item);
        queue_item_t pop(int index = 0);
        int has_request();
        int destroy();
        int recovery();
};


class Prefetcher {
  private:

	bool _ready;
	Request _nextReq;
    //Fill the map?
    //
    Queue prefetch_queue;
    Queue mem_queue;
    Predictor predictor;

    //u_int32_t issue_cycle;
    u_int32_t last_issue;
    u_int32_t last_complete_issue;

    int mem_start_pos;
    int mem_end_pos;
    u_int32_t mem_start_time[MAX_HEALTH_COUNT];
    u_int32_t mem_end_time[MAX_HEALTH_COUNT];
    u_int32_t avg_prefetch_time;

    HistoryLog history;
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
