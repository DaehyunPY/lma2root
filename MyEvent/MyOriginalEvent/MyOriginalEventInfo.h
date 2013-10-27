#ifndef __MyOriginalEventInfo_h__
#define __MyOriginalEventInfo_h__

#include <vector>
#include <TObject.h>

#include "./MyOriginalChannel/MyOriginalChannelInfo.h"


class MyArchive;

typedef std::vector<MyOriginalChannelInfo> OrigChInfo;
class MyOriginalEventInfo : public TObject
{
public:
	MyOriginalEventInfo():
		fNbrBytes(0),fSampInter(0),fNbrSamples(0),fDelayTime(0),
		fTrigChan(0),fTrigLevel(0),fTrigSlope(0),fUsedChans(0),
		fChanCombUsedChans(0),fNbrConPerCh(0),fInfoChannels(0)						{}

public:
	bool							 ReadFileHeader(MyArchive &ar);

public:
	short							 GetNbrBytes()const								{return fNbrBytes;}
	double							 GetSampleInterval()const						{return fSampInter;}
	long							 GetNbrSamples()const							{return fNbrSamples;}
	double							 GetDelayTime()const							{return fDelayTime;}
	short							 GetTriggerChannel()const						{return fTrigChan;}
	double							 GetTriggerLevel()const							{return fTrigLevel;}
	short							 GetTriggerSlope()const							{return fTrigSlope;}
	long							 GetUsedChannels()const							{return fUsedChans;}
	long							 GetChanCombUsedChannels()const					{return fChanCombUsedChans;}
	short							 GetNbrConvPerChans()const						{return fNbrConPerCh;}

	size_t							 GetNbrOfChannels()const						{return fInfoChannels.size();}
	MyOriginalChannelInfo			&GetChannelInfo(long idx)						{return fInfoChannels[idx];}
	const MyOriginalChannelInfo		&GetChannelInfo(long idx)const					{return fInfoChannels[idx];}

private:
	short							 fNbrBytes;										//Nbr of bytes of the adc values (either 1 or 2)
	double							 fSampInter;									//the time between two consecutive points (in ns)
	long							 fNbrSamples;									//Nbr of Points (multiplied by the fSampInter it will give the timewindow in ns)
	double							 fDelayTime;									//the delay of the trigger with respect to the window
	short							 fTrigChan;										//the fTriggering Channel
	double							 fTrigLevel;									//the trigger Level from the Offset
	short							 fTrigSlope;									//which Slope was used by the fTrigger
	long							 fUsedChans;									//Bitmask discribing which Channels have been recorded 
	long							 fChanCombUsedChans;							//Bitmask discribing which Converters per Channel have been used
	short							 fNbrConPerCh;									//tells how many converts per channel have been used
	OrigChInfo						 fInfoChannels;									//Container for all Channels Informations


	ClassDef(MyOriginalEventInfo,1)													//Infos about an Event as they are stored in the lma file
};
#endif