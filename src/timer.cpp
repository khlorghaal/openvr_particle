/* timer.cpp
 *
 *  Created on: Oct 14, 2015
 *  Author: Khlorghaal
 *	All Rights Reserved.
 *	Do not distribute.
 */

#include "timer.hpp"
#include <glfw3.h>
#include <thread>
using namespace std::this_thread;

int64_t current_time_microsec(){
	return (int64_t)(glfwGetTime()*1000000);
}

Timer::Timer(float fps){
	setMaxUpdatesPerSecond(fps);
	tRelease= current_time_microsec();
}

void Timer::setMaxUpdatesPerSecond(float cap){
	if(cap==-1 || cap==0)
		targetTimeus= 0;
	targetTimeus= int(1e6f/cap);
}

void Timer::invoke(){
	//(release-invoke)+wait=target
	//wait= target-(release-invoke)
	tick++;

	tInvokep= tInvoke;
	tInvoke= current_time_microsec();

	spf= float(tInvoke-tInvokep+1)/1e6f;
	fps= 1.f/spf;
	mut.lock();
	prevspf<<spf;
	mut.unlock();

	int dt= int(tInvoke-tRelease);
	if(dt<0)
		dt= 0;

	int w;
	if(halttime==0)
		w= targetTimeus-dt;
	else{
		w= halttime-dt;
		halttime= 0;
	}

	if(!unyeilding && w>2000){
		waiting= true;
		sleep_for(std::chrono::microseconds(w));
		waiting= false;
	}
	else{
		waiting= true;
		if(!unyeilding && w>500)
			yield();
		auto end= tInvoke + w;
		while( current_time_microsec()< end );
		waiting= false;
	}

	tRelease= current_time_microsec();
}

void Timer::halt(){
	halttime= 0x7fffffff;
}
void Timer::halt(int timems){
	halttime= timems;
}
void Timer::dehalt(){
	halttime= 0;
}

float Timer::getAverageSpf(){
	float a=0;
	for(auto& t : prevspf)
		a+=t;
	a/= prevspf.capacity;
	return a;
}
int Timer::getAverageFps(){
	return int(1.f/getAverageSpf());
}

/**thisrate * k = thatrate*/
float Timer::getRelativeRate(Timer& that){
	return that.spf/spf;
}
