/* execq.cpp
 *
 *  Created on: Jul 28, 2015
 *  Author: Khlorghaal
 *	All Rights Reserved
 *	Do not distribute.
 */

#include "execq.hpp"

void execq::push(exec* e){
	mut.lock();
	q<<e;
	mut.unlock();
}
void execq::flush(){
	vector<exec*> qc;
	mut.lock();
	if(!q.empty())
		swap(q, qc);//create a copy to minimize blocking
	mut.unlock();

	for(auto i:qc){
		i->invoke();
		delete i;
	}
}
