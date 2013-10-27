#ifndef __MySignalAnalyzedEventInfo_h__
#define __MySignalAnalyzedEventInfo_h__

#include <vector>
#include <TObject.h>

#include "./MySignalAnalyzedChannel/MySignalAnalyzedChannelInfo.h"

class MySettings;
class MyHistos;
class MyOriginalEventInfo;

typedef std::vector<MySignalAnalyzedChannelInfo> SAChannelInfoVec;
class MySignalAnalyzedEventInfo : public TObject
{
public:
	MySignalAnalyzedEventInfo()													{}

public:
	bool								 ReadSettings(MySettings&,const MyOriginalEventInfo&);

public:
	short								 GetUsedMethod()const					{return fUsedMethod;}
	size_t								 GetNbrOfChannels()const				{return fInfoChannels.size();}
	MySignalAnalyzedChannelInfo			&GetChannelInfo(long idx)				{return fInfoChannels[idx];}
	const MySignalAnalyzedChannelInfo	&GetChannelInfo(long idx)const			{return fInfoChannels[idx];}

private:
	SAChannelInfoVec					 fInfoChannels;							//Container for all Channels Information
	short								 fUsedMethod;							//the Method that is used to extract the peaks from the pulses

	ClassDef(MySignalAnalyzedEventInfo,1)										//Infos about an Event where the Signals are analyzed
};
#endif