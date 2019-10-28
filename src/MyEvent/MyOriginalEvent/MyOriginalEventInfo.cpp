#include "MyOriginalEventInfo.h"

#include "../../MyArchive/MyArchive.h"


//______________________________________________________________________________________________________________________
bool MyOriginalEventInfo::ReadFileHeader(MyArchive &ar)
{
	//flag that will show wether something has changed
	bool changed=false;

	//--read all the values in the header for the Event--//
	short  tempS;
	long   tempL;
	double tempD;
	bool   tempB;
	
	//the headersize in bytes//
	long headersize=0;
	ar >> headersize;
	//nbr of Channels in Instrument, check wether it changed is below//
	short nbrChannels=0;
	ar >> nbrChannels;
	//the nbr of bytes (in file is the nbr of bits)
	ar >> tempS; tempS /= 8;
	changed = (tempS != fNbrBytes) || changed;
	fNbrBytes = tempS;
	//the sample interval//
	ar >> tempD;
	changed = (TMath::Abs(tempD - fSampInter) > 1.e-4) || changed;
	fSampInter = tempD;
	//the nbr of samples in the window
	ar >> tempL;
	changed = (tempL != fNbrSamples) || changed;
	fNbrSamples = tempL;
	//the delaytime	
	ar >> tempD;
	changed = (TMath::Abs(tempD - fDelayTime) > 1.e-4) || changed;
	fDelayTime=tempD;
	//the trigger Channel
	ar >> tempS;
	changed = (tempS != fTrigChan) || changed;
	fTrigChan = tempS;
	//the trigger Level
	ar >> tempD;
	changed = (TMath::Abs(tempD - fTrigLevel) > 1.e-4) || changed;
	fTrigLevel=tempD;
	//the trigger Slope
	ar >> tempS;
	changed = (tempS != fTrigSlope) || changed;
	fTrigSlope = tempS;
	//the used Channels Bitmask
	ar >> tempL;
	changed = (tempL != fUsedChans) || changed;
	fUsedChans = tempL;
	//the Channel Combination Bitmask
	ar >> tempL;
	changed = (tempL != fChanCombUsedChans) || changed;
	fChanCombUsedChans = tempL;
	//the Nbr of Converters per Channel
	ar >> tempS;
	changed = (tempS != fNbrConPerCh) || changed;
	fNbrConPerCh = tempS;
	
	//if the nbrOfChannels has changed, clear and create new Channel infos//
	if (nbrChannels != GetNbrOfChannels())
	{
		changed = true;
		fInfoChannels.clear();
		for (size_t i=0;i<nbrChannels;++i)
		{
			fInfoChannels.push_back(MyOriginalChannelInfo(i));
				if (fUsedChans & (0x1<<i))
					fInfoChannels.back().ReadFileHeader(ar);

		}
	}
	//otherwise just reread the channel infos
	else
	{
		for (int i=0;i<GetNbrOfChannels();++i)
			if (fUsedChans & (0x1<<i))
				changed = GetChannelInfo(i).ReadFileHeader(ar) || changed;
	}

	return changed;
}
