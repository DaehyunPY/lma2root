#ifndef __MyDetektorHitSorterAchim_H_
#define __MyDetektorHitSorterAchim_H_

#include "resort64c.h"

class MyDetektorInfo;

//______________________MyDetektorHitSorter Achim Base______________________
class MyDetektorHitSorterAchim
{
public:
	MyDetektorHitSorterAchim(const MyDetektorInfo&);
	~MyDetektorHitSorterAchim();

protected:
	void SortWithAchimSorter();

private:
	void InitAchimSorter(const MyDetektorInfo&);

protected:
	sort_class						*fAs;						//pointer to achims sorter class
	sum_walk_calibration_class		*fSwc;						//pointer to tsum calibrator
	scalefactors_calibration_class	*fSfc;						//pointer to scalfactor calibrator
	int								 fCnt[7];					//counter array for achims routine
	int								 fNRecHits;					//how many hits have been found by the sorter

private:
	bool							 fAlreadyInitialized;		//flag that shows if as->init() was already called

//[2013/10/28 Matsunami]
public:
	int								 GetNbrRecHits()		 {return fNRecHits;}
};
#endif