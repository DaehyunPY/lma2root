#include <TMath.h>

#include "MyOriginalChannelInfo.h"

#include "../../../MyArchive/MyArchive.h"

//______________________________________________________________________________________________________________________
MyOriginalChannelInfo::MyOriginalChannelInfo(int chnbr):
	fFullscale(0), fOffset(0), fGain(0), fBaseline(0),
	fNoise(0), fStsi(0), fBs(0),fChNbr(chnbr)
{
}

//______________________________________________________________________________________________________________________
bool MyOriginalChannelInfo::ReadFileHeader(MyArchive &ar)
{
	bool changed=false;
	//read all the values in the header from the archive
	//check wether something has changed
	short  tempS;
	long   tempL;
	double tempD;
	//the fullscale in mV
	ar >> tempS;
	changed = (tempS != fFullscale) || changed;
	fFullscale = tempS;
	//the offset in mV
	ar >> tempS;
	changed = (tempS != fOffset) || changed;
	fOffset = tempS;
	//the vertical Gain (conversion factor to convert the bits to mV
	ar >> tempD;
	changed = (TMath::Abs(tempD - fGain) > 1.e-4) || changed;
	fGain = tempD;
	//the Baseline for zero substraction
	ar >> tempS;
	changed = (tempS != fBaseline) || changed;
	fBaseline = tempS;
	//the Noise Level for zero substraction
	ar >> tempS;
	changed = (tempS != fNoise) || changed;
	fNoise = tempS;
	//the stepsize of the zero substraction
	ar >> tempL;
	changed = (tempL != fStsi) || changed;
	fStsi = tempL;
	//the BackSize of the zero substraction
	ar >> tempL;
	changed = (tempL != fBs) || changed;
	fBs = tempL;

	return changed;
}
