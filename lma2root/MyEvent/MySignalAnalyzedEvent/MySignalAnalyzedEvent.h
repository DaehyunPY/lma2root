#ifndef __MySignalAnalyzedEvent_h__
#define __MySignalAnalyzedEvent_h__

#include <vector>

#include "./MySignalAnalyzedChannel/MySignalAnalyzedChannel.h"
#include "../MyOriginalEvent/MyOriginalEvent.h"

class MyOriginalEvent;
class MySignalAnalyzedEventInfo;
class MyArchive;


typedef std::vector<MySignalAnalyzedChannel> SAChannelVec;
class MySignalAnalyzedEvent
{
public:
	MySignalAnalyzedEvent()														{}

public:
	void							 ReadFromEventInfo(const MySignalAnalyzedEventInfo&);
	void							 Clear()									{for (size_t i=0;i<fChannels.size();fChannels[i++].Clear());}
	void							 ClearChannel(size_t);
	void							 CopyEventIDFrom(const MyOriginalEvent &oe)		{fEventID = oe.GetEventID();}

public:
	size_t							 GetNbrOfChannels() const					{return fChannels.size();}
	MySignalAnalyzedChannel			&GetChannel(long idx)						{return fChannels[idx];}
	const MySignalAnalyzedChannel	&GetChannel(long idx)const					{return fChannels[idx];}

	short							 GetUsedMethod()const						{return fUsedMethod;}
	long/*Long64_t*/				 GetEventID()const							{return fEventID;}

private:
	long/*Long64_t*/				 fEventID;									//an id for this event. It is just the time the event was recorded
	SAChannelVec					 fChannels;									//Container for all Channels

	//these Infos are stored in the Tree Header
	short							 fUsedMethod;								//!the Method that is used to extract the peaks from the pulses

	ClassDef(MySignalAnalyzedEvent,1)											//An Event where the Pulses are analyzed
};

#endif