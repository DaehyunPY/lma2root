#ifndef __MySortedEventInfo_h__
#define __MySortedEventInfo_h__

#include <vector>
#include <TObject.h>

#include "./MyDetektor/MyDetektorInfo.h"

class MySettings;
class MySignalAnalyzedEventInfo;

typedef std::vector<MyDetektorInfo> DetInfoVec;
class MySortedEventInfo : public TObject
{
public:
	MySortedEventInfo():fInfoDets()											{}

public:
	bool						 ReadSettings(MySettings&, const MySignalAnalyzedEventInfo&);

public:
	size_t						 GetNbrOfDetektorInfos()const				{return fInfoDets.size();}
	MyDetektorInfo				&GetDetektorInfo(long idx)					{return fInfoDets[idx];}
	const MyDetektorInfo		&GetDetektorInfo(long idx)const				{return fInfoDets[idx];}

private:
	DetInfoVec					 fInfoDets;									//Container for all Detektor Information


	ClassDef(MySortedEventInfo,1)											//Infos about an Event that stores the Sorted for DetektorHits events
};
#endif