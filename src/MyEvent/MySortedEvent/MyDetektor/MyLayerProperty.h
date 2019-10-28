#ifndef __MyLayerProperty_h_
#define __MyLayerProperty_h_

#include <TString.h>
#include <vector>

#include "MyTimeRange.h"

class MySettings;
class MyChannelSection;

typedef std::vector<MyTimeRange> trVec;
class MyLayerProperty
{
public:
	MyLayerProperty()										{}
	
public:
	void			operator=(const MyChannelSection &);
	bool			ReadSettings(MySettings&);

	const long		GetChannelNbr()const					{return fChNbr;}
	const long		GetChannelSectionNbr()const				{return fSecNbr;}

	const long		GetPolarity()const						{return fPolarity;}

	const long		GetNbrOfTimeRanges()const				{return fTimeRanges.size();}
	const double	GetTimeRangeLow(size_t idx)const		{return fTimeRanges[idx].Low();}
	const double	GetTimeRangeHigh(size_t idx)const		{return fTimeRanges[idx].High();}

	const char	   *GetName()const							{return fName.Data();}
	void			SetName(const char *name)				{fName = name;}

private:
	long			fChNbr;									//the ChannelNbr this Section belongs to
	long			fSecNbr;								//the number of the channelsection that this property belongs to
	long			fPolarity;								//the Polarity the Signal have
	trVec			fTimeRanges;							//the time ranges of this layerproperty
	TString			fName;									//the name of the layer this channelsection belong to

	ClassDef(MyLayerProperty,1)								//Defines a Section of a Channel and its Properties
};


#endif