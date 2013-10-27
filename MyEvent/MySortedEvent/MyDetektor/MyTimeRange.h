#ifndef _MyTimeRange_h_
#define _MyTimeRange_h_

#include <TObject.h>

class MyTimeRange
{
public: 
	MyTimeRange():fLow(0),fHigh(0)	{}
	MyTimeRange(double low,double high):
	  fLow(low),fHigh(high)			{}
public:
	double		 Low()const			{return fLow;}
	double		 High()const		{return fHigh;}

private:
	double fLow; 
	double fHigh;

	ClassDef(MyTimeRange,1)			//a timerange
};

#endif