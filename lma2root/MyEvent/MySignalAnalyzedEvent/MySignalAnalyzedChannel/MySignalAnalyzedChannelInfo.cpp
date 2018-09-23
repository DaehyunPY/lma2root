#include <TMath.h>

#include "MySignalAnalyzedChannelInfo.h"

#include "../../../MySettings/MySettings.h"

//______________________________________________________________________________________________________________________
MySignalAnalyzedChannelInfo::MySignalAnalyzedChannelInfo(int chnbr, MySettings &set, const MyOriginalEventInfo &oei):
	fChNbr(chnbr)
{
	ReadSettings(set,oei);
}

//______________________________________________________________________________________________________________________
bool MySignalAnalyzedChannelInfo::ReadSettings(MySettings &set, const MyOriginalEventInfo &oei)
{
	bool changed=false;
	
	//read all the values in the header from the archive
	//check wether something has changed
	short  tempS;
	long   tempL;
	double tempD;

	//now read the channel sections properties from the settings
	//std::cout << Form("Chan%02d_NbrSections",fChNbr+1) << std::endl;
	int nbrSec = int(set.GetValue(Form("Chan%02d_NbrSections",fChNbr+1),1)+0.1);
	if (nbrSec != GetNbrOfChannelSections())
	{
		changed = true;
		fChannelSections.clear();
		for (int i=0;i<nbrSec;++i)
			fChannelSections.push_back(MyChannelSection(fChNbr,i,set,oei));
	}
	else
	{
		for (size_t i=0;i<GetNbrOfChannelSections();++i)
			changed = GetChannelSection(i).ReadSettings(set,oei) || changed;
	}

	return changed;
}
