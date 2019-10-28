#include "MyLayerProperty.h"

#include "../../../MySettings/MySettings.h"
#include "../../MyOriginalEvent/MyOriginalEventInfo.h"
#include "../../MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.h"


//______________________________________________________________________________________________________________________
void MyLayerProperty::operator=(const MyChannelSection &ics)
{
	//copy the informations from the channelsection//
	fChNbr		= ics.GetChannelNbr();
	fSecNbr		= ics.GetChannelSectionNbr();

	//initialize the timerange vector with the information from the channel section//
	fTimeRanges.push_back(MyTimeRange(ics.GetTimeRangeLow(),ics.GetTimeRangeHigh()));
}

//______________________________________________________________________________________________________________________
bool MyLayerProperty::ReadSettings(MySettings &set)
{
	bool changed=false;
	//read all the values from the settings
	//check wether something has changed
	int		tempI;

	//Polarity//
	tempI = (int)(set.GetValue(Form("%s_Polarity",fName.Data()),-1)+0.1);
	changed = (tempI != fPolarity) || changed;
	fPolarity= tempI;

	//nbrOfTimeRanges (if 0, leave the time range set by the channelsection, otherwise overwrite with the new settings//
	tempI = (int)(set.GetValue(Form("%s_NbrOfTimeRanges",GetName()),0)+0.1);
	if (tempI)
	{
		changed = true;
		//reset the timeranges//
		fTimeRanges.clear();
		for (size_t i=0;i<tempI;++i)
		{
			//time range lower edge//
			double low = set.GetValue(Form("%s_TimeRange%02d_From",GetName(),i+1),0);
			//time range upper edge//
			double high = set.GetValue(Form("%s_TimeRange%02d_To",GetName(),i+1),0);
			//add to timeranges//
			fTimeRanges.push_back(MyTimeRange(low,high));
		}
	}
	return changed;
}
