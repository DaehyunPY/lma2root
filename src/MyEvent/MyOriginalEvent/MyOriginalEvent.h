#ifndef __MyOriginalEvent_h__
#define __MyOriginalEvent_h__

#include <vector>

#include "./MyOriginalChannel/MyOriginalChannel.h"
class MyOriginalEventInfo;
class MyArchive;
#include "../../MyRootManager/MyHistos.h"

typedef std::vector<MyOriginalChannel> OrigChannelVec;
class MyOriginalEvent
{
public:
	MyOriginalEvent():fEventID(0),fHorpos(0),fChannels()				{}

public:
	void					 Clear();
	void					 ClearChannel(size_t);
	void					 Serialize(MyArchive&);
	void					 ReadFromEventInfo(const MyOriginalEventInfo&);
	void					 Differential(const double,const double);//motomura
	void					 Smoothing(const double);
	void					 RemoveNoiseLongPulse(const int maxLength);
	//void					 MakeDataHisto(MyHistos &rm);
	//void					 FillDataHisto(MyHistos &rm);


public:
	size_t					 GetNbrOfChannels()const					{return fChannels.size();}
	MyOriginalChannel		&GetChannel(long idx)						{return fChannels[idx];}
	const MyOriginalChannel	&GetChannel(long idx)const					{return fChannels[idx];}

public:
	long/*Long64_t*/		 GetEventID()const							{return fEventID;}
	double					 GetHorpos()const							{return fHorpos;}
	short					 GetNbrBytes()const							{return fNbrBytes;}
	double					 GetSampleInterval()const					{return fSampInter;}
	long					 GetNbrSamples()const						{return fNbrSamples;}
	double					 GetDelayTime()const						{return fDelayTime;}
	short					 GetTriggerChannel()const					{return fTrigChan;}
	double					 GetTriggerLevel()const						{return fTrigLevel;}
	short					 GetTriggerSlope()const						{return fTrigSlope;}
	long					 GetUsedChannels()const						{return fUsedChans;}
	long					 GetChanCombUsedChannels()const				{return fChanCombUsedChans;}
	short					 GetNbrConvPerChans()const					{return fNbrConPerCh;}

private:
	long/*Long64_t*/		 fEventID;									//an id for this event. It is just the time the event was recorded
	double					 fHorpos;									//the fHorpos value for this event
	OrigChannelVec			 fChannels;									//Container for all Channels

	//long					 fEventIDThen;								//-------------------------!!!!!!!!!!!

	//these Infos are stored in the Tree Header
	bool					 fKill;										//! a flag that telling the thread to quit
	short					 fNbrBytes;									//! Nbr of bytes of the adc values (either 1 or 2)
	double					 fSampInter;								//! the time between two consecutive points (in ns)
	long					 fNbrSamples;								//! Nbr of Points (multiplied by the fSampInter it will give the timewindow in ns)
	double					 fDelayTime;								//! the delay of the trigger with respect to the window
	short					 fTrigChan;									//! the fTriggering Channel
	double					 fTrigLevel;								//! the trigger Level from the Offset
	short					 fTrigSlope;								//! which Slope was used by the fTrigger
	long					 fUsedChans;								//! Bitmask discribing which Channels have been recorded 
	long					 fChanCombUsedChans;						//! Bitmask discribing which Converters per Channel have been used
	short					 fNbrConPerCh;								//! tells how many converts per channel have been used

	ClassDef(MyOriginalEvent,1)											//An Event as it is stored in the lma file
};

#endif