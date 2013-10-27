#ifndef __MySemaphore_h__
#define __MySemaphore_h__

#include <TMutex.h>
#include <TCondition.h>


//a semaphore implementation
class MySemaphore
{
public:
	MySemaphore(int initial=1): 
	  fMutex(), fCond(&fMutex), fCount(initial)	{}

	//this functions waits until the count is >= 1
	int Wait()
	{
		//make this function mutual exclusive
		//in the destructor of the guard the mutex is unlocked
		TLockGuard guard(&fMutex);

		//wait on the condition until the count is >= 1
		while (fCount < 1)
			fCond.Wait();

		return 0;
	}

	int Decrement()
	{
		//make this function mutual exclusive
		//in the destructor of the guard the mutex is unlocked
		TLockGuard guard(&fMutex);

		//Decrement the Count
		--fCount;

		return 0;
	}

	int Increment()
	{
		//lock this function
		fMutex.Lock();

		//increment the count
		++fCount;

		//check wether we need to signal the condition that it is met
		bool doSignal = (fCount >= 1);

		//unlock at this point
		fMutex.UnLock();
		//when the count is >= 1 then signal the condition
		if (doSignal) fCond.Signal();

		return 0;
	}

private:
	TMutex		fMutex;		//the Mutex
	TCondition	fCond;		//the Condition
	int			fCount;		//the count

};

#endif