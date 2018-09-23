#ifndef __MyQueue_h__
#define __MyQueue_h__

#include <queue>
#include <TMutex.h>
#include <TCondition.h>

//a queue for communication between one producer and many consumers
template<typename Data>
class MyQueue
{
public:
	MyQueue(): fMutex(), fCond(&fMutex)	{}

	//empty the queue from all entries
	void leeren()
	{
		//make this function mutual exclusive
		//in the destructor of the guard the mutex is unlocked
		TLockGuard guard(&fMutex);
		while(!fQ.empty())
			fQ.pop();
	}

	//to put something in the queue
	void push(Data const& data)
	{
		//Lock the function
		fMutex.Lock();
		//put the data to the queue
		fQ.push(data);
		//unlock at this point
		fMutex.UnLock();
		//tell the condition that there is something in the queue now
		fCond.Signal();
	}
	
	//retrieve something from queue, but wait until something is available first
	void wait_and_pop(Data& popped_value)
	{
		//make this function mutual exclusive
		//in the destructor of the guard the mutex is unlocked
		TLockGuard guard(&fMutex);
		//wait until the condition got a signal
		//but also check wether there is really someting in the queue (to avoid spurous Signals)
		while(fQ.empty())
			fCond.Wait();

		//when there is something in the queue retrieve and pop it
		popped_value=fQ.front();
		fQ.pop();
	}

	//check wether the queue is empty
	bool empty()
    {
		//make this function mutual exclusive
		//in the destructor of the guard the mutex is unlocked
		TLockGuard guard(&fMutex);
		return fQ.empty();
    }

private:
	std::queue<Data>	fQ;			//the Queue
	TMutex				fMutex;		//the Mutex
	TCondition			fCond;		//the Condition

};

#endif
