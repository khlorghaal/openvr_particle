/* timer.hpp
 *
 *  Created on: Oct 14, 2015
 *  Author: Khlorghaal
 *	All Rights Reserved.
 *	Do not distribute.
 */

#pragma once

#include <mutex>
using std::mutex;
#include <atomic>
using std::atomic_bool;
#include <condition_variable>
using std::condition_variable;
#include "kh/ringbuffer.hpp"

int64_t current_time_microsec();

class Timer{
	int64_t tRelease, tInvoke, tInvokep;

	int targetTimeus;

	mutex mut;
	condition_variable cv;
	atomic_bool waiting;
	int halttime=0;//sleep upon next invoke


	/**Always has size==samplesize*/
	ringbuffer<float, 100> prevspf;

public:
	uint64_t tick=0;
	float fps;
	float spf;//seconds per frame

	bool unyeilding= false;//spin wait

	Timer(float fps);

	/**@param cap -1 if infinite*/
	void setMaxUpdatesPerSecond(float cap);

	void invoke();

	void halt();
	void halt(int timems);
	void dehalt();

	float getAverageSpf();
	int getAverageFps();
	/**thisrate * k = thatrate*/
	float getRelativeRate(Timer& that);
};