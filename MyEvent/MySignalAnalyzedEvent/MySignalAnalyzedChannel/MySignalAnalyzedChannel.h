#ifndef __MySignalAnalyzedChannel_h__
#define __MySignalAnalyzedChannel_h__

#include <TObject.h>
#include <vector>

#include "MyChannelSection.h"

#include "../MyPeak/MyPeak.h"
#include "../MyVoltage/MyVoltage.h"

class MySignalAnalyzedChannelInfo;

typedef std::vector<MyChannelSection> ChanSectVec;
typedef std::vector<MyPeak> PeakVec;
typedef std::vector<MyVoltage> VoltVec;

class MySignalAnalyzedChannel
{
public:
	MySignalAnalyzedChannel()									{}
	MySignalAnalyzedChannel(long chNbr,const MySignalAnalyzedChannelInfo&);

public:
	MyPeak					&AddPeak(int PulsNbr);
	void					 ReadFromChannelInfo(const MySignalAnalyzedChannelInfo&);
	const MyChannelSection	*GetPointerToChannelSectionForIndex(long idx)const;
	double					 GetVoltageForChannelSection(int ChannelSectionNbr)const;
	void					 DelPeak();
public:
	void					 Clear()							{fPeaks.clear();fVoltages.clear();}
	void					 AddVoltage(double v, int csn)		{fVoltages.push_back(MyVoltage(v,csn));}
	long					 GetChannelNbr()const				{return fChNbr;}

	size_t					 GetNbrPeaks()const					{return fPeaks.size();}
	MyPeak					&GetPeak(long idx)					{return fPeaks[idx];}
	const MyPeak			&GetPeak(long idx)const				{return fPeaks[idx];}

	size_t					 GetNbrOfChannelSections() const	{return fChannelSections.size();}
	MyChannelSection		&GetChannelSection(int idx)			{return fChannelSections[idx];}
	const MyChannelSection	&GetChannelSection(int idx) const	{return fChannelSections[idx];}

private:
	//these need to be written into the tree for each event//
	long					 fChNbr;							//This Channels Number
	PeakVec					 fPeaks;							//Container storing the pulses
	VoltVec					 fVoltages;							//Container storing the voltages

	//these values are in the Info
	ChanSectVec				 fChannelSections;					//!contains the Channel Sections

	ClassDef(MySignalAnalyzedChannel,1)							//The Channel as it is in the lma file
};
#endif
