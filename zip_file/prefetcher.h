/*
 *
 * File: prefetcher.h
 * Author: Sat Garcia (sat@cs)
 * Description: Header file for prefetcher implementation
 *
 */


#ifndef PREFETCHER_H
#define PREFETCHER_H

#define MAX_QUEUE_SIZE 2000

#include <sys/types.h>
#include "mem-sim.h"

typedef struct p_item {
    u_int32_t pc_addr;
    u_int32_t offset;
    int count;

    int hit;
    int total;

}p_item_t;
class Prefetcher {
  private:
	bool _ready;
	Request _nextReq;
    //
    //
    //PC based prefetcher
    //PC_predicter["PC"] = [next addr's offset]
    //
    //Try to fill it with
    //  previous_pc   = ?
    //  previous_addr = ?
    u_int32_t previous_pc;
    u_int32_t previous_addr;

    //Fill the map?
    p_item_t p_queue[MAX_QUEUE_SIZE];

  public:
	Prefetcher();

    int find_pc(u_int32_t pc);
    void decrease_count();

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
