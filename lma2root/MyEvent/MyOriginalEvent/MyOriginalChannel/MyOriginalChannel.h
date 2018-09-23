#ifndef __MyOriginalChannel_h__
#define __MyOriginalChannel_h__
#include <iostream>
#include <TObject.h>
#include <vector>

#include "../MyPuls/MyPuls.h"

class MyOriginalChannelInfo;
class MyOriginalEvent;
class MyOriginalEventInfo;
class MyArchive;
class MyChannelSection;

typedef std::vector<MyPuls> PulsVec;
class MyOriginalChannel
{
public:
	MyOriginalChannel():fChNbr(-1)										{}
	MyOriginalChannel(int chNbr, const MyOriginalEventInfo&);

public:
	MyPuls					&AddPuls();
	void					 Serialize(MyArchive&, MyOriginalEvent&);
	void					 ReadFromChannelInfo( const MyOriginalEventInfo&);
	void					 PrepareCompressedWaveform(long DataSize, char *&DataPointer, long &IndexToFirstPointInCompressedWaveform);
	void					 Differential(MyOriginalEvent&,double,double);
	void					 Smoothing(MyOriginalEvent&, const double);
	void					 RemoveNoiseLongPulse(MyOriginalEvent&, const int);
	int						 DataSize(MyOriginalEvent& );


public:
	void					 Clear()									{fPulses.clear();fZeroSuppressedWaveform.clear();}

	size_t					 GetNbrPulses()const						{return fPulses.size();}
	MyPuls					&GetPuls(long idx)							{return fPulses[idx];}
	const MyPuls			&GetPuls(long idx)const						{return fPulses[idx];}

	int						 GetChannelNbr()const						{return fChNbr;}
	short					 GetBaseline()const							{return fBaseline;}
	short					 GetNoise()const							{return fNoise;}
	short					 GetFullScale()const 						{return fFullscale;}
	short					 GetOffset()const							{return fOffset;}
	double					 GetVertGain()const 						{return fGain;}
	long					 GetStepSize()const							{return fStsi;}
	long					 GetBackSize()const							{return fBs;}
	const void				*GetDataPointerForPuls(const MyPuls&pu)const{return &fZeroSuppressedWaveform[pu.GetIndexToFirstPointOfCompressedWaveform()];}
	void					*GetDataPointerForPuls(const MyPuls&pu)		{return &fZeroSuppressedWaveform[pu.GetIndexToFirstPointOfCompressedWaveform()];}


private:
	//these need to be written into the tree for each event//
	int						 fChNbr;									//This Channels Number
	PulsVec					 fPulses;									//Container storing the pulses
	std::vector<char>		 fZeroSuppressedWaveform;					//a buffer to contain the zero suppressed traces of the pulses

	//these informations are stored in the header
	short					 fFullscale;								//! the fullscale for this channel (in mV)
	short					 fOffset;									//! the offset for this channel (in mV)
	double					 fGain;										//! the conversion factor from adc bytes to mV (adc bytes * fGain = mV)
	short					 fBaseline;									//! the fBaseline for this channel (in adc bytes)
	short					 fNoise;									//! the fNoiselevel for this channel (in adc bytes)
	long					 fStsi;										//! the stepsize for this channel
	long					 fBs;										//! the backsize for this channel

	ClassDef(MyOriginalChannel,1)										//The Channel as it is in the lma file
};
#endif
