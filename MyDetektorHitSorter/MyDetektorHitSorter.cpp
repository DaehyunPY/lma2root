#include <iostream>

#include "MyDetektorHitSorter.h"

#include "MyDetektorHitSorterSimple.h"
#include "MyDetektorHitSorterAchimQuad.h"
#include "MyDetektorHitSorterAchimHex.h"

#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MySortedEvent/MySortedEvent.h"
#include "../MyEvent/MySortedEvent/MySortedEventInfo.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektor.h"
#include "../MyRootManager/MyHistos.h"


//______________________________________________the acutal worker_____________________________________________________________________________________________________________
MyDetektorHitSorter::~MyDetektorHitSorter()
{
	//std::cout <<"delete dethitsorter"<<std::endl;
	fSM.clear();
	for (size_t i=0;i<fDhs.size();++i)
		delete fDhs[i];
	fDhs.clear();
	//std::cout <<"done"<<std::endl;
}

//______________________________________________initialization_____________________________________________________________________________________________________________
void MyDetektorHitSorter::Sort(MySignalAnalyzedEvent &sae, MySortedEvent &se, MyHistos &rm)
{
	for (size_t i=0; i<fDhs.size();++i)
		fDhs[i]->Sort(sae,se.GetDetektor(i),rm);
}

//______________________________________________initialization_____________________________________________________________________________________________________________
void MyDetektorHitSorter::WriteCalibData(const MySortedEventInfo &sei)
{
	for (size_t i=0; i<fDhs.size();++i)
		if (sei.GetDetektorInfo(i).DoCalibration())
			fDhs[i]->WriteCalibData(sei.GetDetektorInfo(i));
}

//______________________________________________initialization_____________________________________________________________________________________________________________
void MyDetektorHitSorter::Init(const MySortedEventInfo &sei, MyHistos &rm)
{
	//if we have not the right amount of detektorhit sorters//
	bool createNew = false;
	//if we have the right amount of detektorhit sorters//
	if (sei.GetNbrOfDetektorInfos() == fDhs.size())
	{
		//check if one of the methods have changed//
		for (size_t i=0;i<sei.GetNbrOfDetektorInfos();++i)
			createNew = createNew || (sei.GetDetektorInfo(i).GetSorterMethod() != fSM[i]);
	}
	//otherwise we have to create the sorter new//
	else
		createNew = true;

	//if we have to create a new sorter, do it for each detektor
	if (createNew)
	{
		//first delete the old settings//
		fSM.clear();
		for (size_t i=0;i<fDhs.size();++i)
			delete fDhs[i];
		fDhs.clear();

		//create a sorter for each Detektor
		for (size_t i=0;i<sei.GetNbrOfDetektorInfos();++i)
		{
			//remember the sorter method for detektor//
			fSM.push_back(sei.GetDetektorInfo(i).GetSorterMethod());
			if (sei.GetDetektorInfo(i).GetSorterMethod() == kSimple)
				fDhs.push_back(new MyDetektorHitSorterSimple(sei.GetDetektorInfo(i),rm,(i+1)*1000));
			else if (sei.GetDetektorInfo(i).GetSorterMethod() == kAchim)
				if (sei.GetDetektorInfo(i).IsHexAnode())
					if (sei.GetDetektorInfo(i).DoCalibration())
						fDhs.push_back(new MyDetektorHitSorterAchimHexCalib(sei.GetDetektorInfo(i),rm,(i+1)*1000));
					else
						fDhs.push_back(new MyDetektorHitSorterAchimHex(sei.GetDetektorInfo(i),rm,(i+1)*1000));
				else 
				{
					if (sei.GetDetektorInfo(i).DoCalibration())
						fDhs.push_back(new MyDetektorHitSorterAchimQuadCalib(sei.GetDetektorInfo(i),rm,(i+1)*1000));
					else
						fDhs.push_back(new MyDetektorHitSorterAchimQuad(sei.GetDetektorInfo(i),rm,(i+1)*1000));
				}
			else{
				std::cout <<sei.GetDetektorInfo(i).GetName()<< ": requested SorterMethod("<<sei.GetDetektorInfo(i).GetSorterMethod()<<") does not exist "<<std::endl; exit(0);}
		}
	}
}
