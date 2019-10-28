#ifndef __MyDetektorHitSorterHex_H_
#define __MyDetektorHitSorterHex_H_

#include "MyDetektorHitSorter.h"

class MyDetektorInfo;

//___________________________MyDetektorHitSorter______________________________
class MyDetektorHitSorterHex : public MyDetektorHitSorterBase
{
public:
	MyDetektorHitSorterHex(const MyDetektorInfo&, MyHistos&, int HiOff);
	virtual ~MyDetektorHitSorterHex()	{}

public:
	virtual void Sort(MySignalAnalyzedEvent&, MyDetektor&, MyHistos&)=0;
	virtual void WriteCalibData(const MyDetektorInfo&)=0;

protected:
	void FillHistosBeforeShift(const MyDetektor&, MyHistos&);
	void FillDeadTimeHistos(MyHistos &);
	void ExtractTimes(MySignalAnalyzedEvent&, const MyDetektor&);
	void FillRatioHistos(const MyDetektor&, MyHistos&);
};
#endif