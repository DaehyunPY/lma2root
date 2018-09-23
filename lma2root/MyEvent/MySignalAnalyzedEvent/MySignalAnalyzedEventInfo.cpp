#include "MySignalAnalyzedEventInfo.h"

#include "../../MySettings/MySettings.h"
#include "../MyOriginalEvent/MyOriginalEventInfo.h"

//______________________________________________________________________________________________________________________
bool MySignalAnalyzedEventInfo::ReadSettings(MySettings &set, const MyOriginalEventInfo &oei)
{
	//flag that will show wether something has changed
	bool changed=false;

	//if the nbr of channels is not the same, clear and create the amount requested channels//
	if (oei.GetNbrOfChannels() != GetNbrOfChannels())
	{
		changed = true;
		//fInfoChannels.clear();
		for (size_t i=0;i<oei.GetNbrOfChannels();++i)
			fInfoChannels.push_back(MySignalAnalyzedChannelInfo(i,set,oei));
	}
	//otherwise just read the settings for the channel
	else
	{
		for (size_t i=0;i<GetNbrOfChannels();++i)
			changed = GetChannelInfo(i).ReadSettings(set,oei) || changed;
	}

	//the Method used to analyse the pulses
	short tempS = int(set.GetValue("UsedMethodForPeakFinding",0)+0.1);
	changed = (tempS != fUsedMethod) || changed;
	fUsedMethod = tempS;

	return changed;
}