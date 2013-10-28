#ifndef __MyDetektorHitSorterAchimQuad_H_
#define __MyDetektorHitSorterAchimQuad_H_

#include "MyDetektorHitSorterAchim.h"
#include "MyDetektorHitSorterQuad.h"

//______________________MyDetektorHitSorter Achim Hex______________________
class MyDetektorHitSorterAchimQuad : public MyDetektorHitSorterQuad, public MyDetektorHitSorterAchim
{
public:
	MyDetektorHitSorterAchimQuad(const MyDetektorInfo&, MyHistos&, int HiOff);

public:
	virtual void Sort(MySignalAnalyzedEvent &sae, MyDetektor &d, MyHistos &rm) {SortImpl(sae,d,rm);}
	void WriteCalibData(const MyDetektorInfo&);

protected:
	void SortImpl(MySignalAnalyzedEvent&, MyDetektor&, MyHistos&);
	void Shift(const MyDetektor&);
	void CreateDetHits(MyDetektor&, MyHistos&);
	void FillHistosAfterShift(const MyDetektor&, MyHistos&);	
	void CreateTDCArrays();
	void Calibrate();

//[2013/10/28 Matsunami]
public:
	int GetNbrRecHits() override {return fNRecHits;}
};

class MyDetektorHitSorterAchimQuadCalib : public MyDetektorHitSorterAchimQuad
{
public:
	MyDetektorHitSorterAchimQuadCalib(const MyDetektorInfo &di, MyHistos &rm, int HiOff):
	  MyDetektorHitSorterAchimQuad(di,rm,HiOff)	{}

public:
	void Sort(MySignalAnalyzedEvent &sae, MyDetektor &d, MyHistos &rm) {SortImpl(sae,d,rm);Calibrate();}
};
#endif