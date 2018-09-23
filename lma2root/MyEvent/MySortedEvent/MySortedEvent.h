#ifndef __MySortedEvent_h__
#define __MySortedEvent_h__

#include <vector>

#include "./MyDetektor/MyDetektor.h"
#include "../MyOriginalEvent/MyOriginalEvent.h"

class MySortedEventInfo;

typedef std::vector<MyDetektor>  DetVec;
class MySortedEvent
{
public:
	MySortedEvent():fDetektors(),fEventID(0)				{}

public:
	void				 ReadFromEventInfo(const MySortedEventInfo&);
	void				 Clear()									{for (size_t i=0;i<fDetektors.size();fDetektors[i++].Clear());}
	void				 CopyEventIDFrom(const MyOriginalEvent &oe)	{fEventID = oe.GetEventID();}

public:
	size_t				 GetNbrOfDetektors()const					{return fDetektors.size();}
	MyDetektor			&GetDetektor(long idx)						{return fDetektors[idx];}
	const MyDetektor	&GetDetektor(long idx)const					{return fDetektors[idx];}

public:
	long/*Long64_t*/	 GetEventID()const							{return fEventID;}

private:
	long/*Long64_t*/	 fEventID;									//an id for this event. It is just the time the event was recorded
	DetVec				 fDetektors;								//Container for all Detektors

	ClassDef(MySortedEvent,1)										//An Event that is sorted for DetektorHits
};

#endif