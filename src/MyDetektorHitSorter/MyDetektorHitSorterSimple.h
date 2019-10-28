#ifndef __MyDetektorHitSorterSimple_H_
#define __MyDetektorHitSorterSimple_H_

#include "MyDetektorHitSorterQuad.h"

//______________________MyDetektorHitSorter Simple Version______________________
class MyDetektorHitSorterSimple : public MyDetektorHitSorterQuad
{
public:
	MyDetektorHitSorterSimple(const MyDetektorInfo&, MyHistos&, int HiOff);

public:
	void Sort(MySignalAnalyzedEvent&, MyDetektor&, MyHistos&);
	void WriteCalibData(const MyDetektorInfo&)	{}

private:
	void SortForTimesum(MyDetektor&, MyHistos&);
};
#endif