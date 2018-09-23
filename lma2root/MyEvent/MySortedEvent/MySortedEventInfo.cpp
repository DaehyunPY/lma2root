#include "MySortedEventInfo.h"

#include "./MyDetektor/MyDetektorInfo.h"
#include "../../MySettings/MySettings.h"

//______________________________________________________________________________________________________________________
bool MySortedEventInfo::ReadSettings(MySettings &set, const MySignalAnalyzedEventInfo &saei)
{
	//flag that will show wether something has changed//
	bool changed=false;

	//read the infos about the detektors from the settings//
	short tempS = int(set.GetValue("NbrDetektors",0)+0.1);
	changed = (tempS != GetNbrOfDetektorInfos()) || changed;
	//if the number of detektors has changed, clear and create the right amount//
	if (tempS != GetNbrOfDetektorInfos())
	{
		changed = true;
		fInfoDets.clear();
		for (size_t i=0;i<tempS;++i)
			fInfoDets.push_back(MyDetektorInfo(i,set,saei));
	}
	//otherwise just reread the settings for the detektors//
	else
	{
		for (size_t i=0;i<GetNbrOfDetektorInfos();++i)
			GetDetektorInfo(i).ReadSettings(set,saei);
	}
	
	return changed;
}
