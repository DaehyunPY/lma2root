#ifndef __MyOriginalChannelInfo_h__
#define __MyOriginalChannelInfo_h__

#include <TObject.h>
#include <TClonesArray.h>

class MyArchive;
class MyChannelSection;
class MySettings;
class MyEventInfo;

class MyOriginalChannelInfo
{
public:
	MyOriginalChannelInfo()										{}
	MyOriginalChannelInfo(int chnbr);

public:
	bool					 ReadFileHeader(MyArchive &ar);

public:
	int						 GetChannelNbr()const				{return fChNbr;}
	short					 GetBaseline()const					{return fBaseline;}
	short					 GetNoise()const					{return fNoise;}
	short					 GetFullScale()const 				{return fFullscale;}
	short					 GetOffset()const					{return fOffset;}
	double					 GetVertGain()const 				{return fGain;}
	long					 GetStepSize()const					{return fStsi;}
	long					 GetBackSize()const					{return fBs;}

private:
	short					 fFullscale;						//the fullscale for this channel (in mV)
	short					 fOffset;							//the offset for this channel (in mV)
	double					 fGain;								//the conversion factor from adc bytes to mV (adc bytes * fGain = mV)
	short					 fBaseline;							//the fBaseline for this channel (in adc bytes)
	short					 fNoise;							//the fNoiselevel for this channel (in adc bytes)
	long					 fStsi;								//the stepsize for this channel
	long					 fBs;								//the backsize for this channel
	int						 fChNbr;							//the Channel Nbr this Information is for

	ClassDef(MyOriginalChannelInfo,1)						//Contains Info about a Channel
};
#endif
