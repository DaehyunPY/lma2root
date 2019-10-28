#ifndef __MySignalAnalyzedChannelInfo_h__
#define __MySignalAnalyzedChannelInfo_h__

#include <TObject.h>

#include "MyChannelSection.h"

class MySettings;
class MyOriginalEventInfo;

typedef std::vector<MyChannelSection> ChanSectVec;
class MySignalAnalyzedChannelInfo
{
public:
	MySignalAnalyzedChannelInfo()								{}
	MySignalAnalyzedChannelInfo(int chnbr, MySettings&, const MyOriginalEventInfo&);

public:
	bool					 ReadSettings(MySettings&, const MyOriginalEventInfo&);

public:
	const int				 GetChannelNbr()const				{return fChNbr;}
	size_t					 GetNbrOfChannelSections()const		{return fChannelSections.size();}
	MyChannelSection		&GetChannelSection(int idx)			{return fChannelSections[idx];}
	const MyChannelSection	&GetChannelSection(int idx)const	{return fChannelSections[idx];}

private:
	int						 fChNbr;							//the Channel Nbr this Information is for
	ChanSectVec				 fChannelSections;					//contains the Channel Sections

	ClassDef(MySignalAnalyzedChannelInfo,1)						//Contains Info about a Channel and the channelsections
};
#endif
