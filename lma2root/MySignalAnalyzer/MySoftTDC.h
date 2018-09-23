#ifndef __MySoftTDC_h__
#define __MySoftTDC_h__

#include <vector>

class MyOriginalEvent;
class MySignalAnalyzedEvent;


//this class is placeholder for two other classes wich will be called 
//according to how many bits the instrument has
class MySoftTDC
{
public:
	virtual void FindPeaksIn(const MyOriginalEvent&, MySignalAnalyzedEvent&, bool)=0;
protected:
	std::vector<double> cData;
};

//this class does nothing 
class MySoftTDCDoNothing : public MySoftTDC
{
public:
	void FindPeaksIn(const MyOriginalEvent&, MySignalAnalyzedEvent&, bool){}
};

#endif