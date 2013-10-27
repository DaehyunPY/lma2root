#include "MyPuls.h"

#include "../../../MyArchive/MyArchive.h"
#include "../MyOriginalChannel/MyOriginalChannel.h"
#include "../MyOriginalEvent.h"

//______________________________________________________________________________________________________________________
MyPuls::MyPuls(int pcn, int pn):
	fPCN(pcn),fPulsNbr(pn),fDataLength(0),fIdxFiPOrig(-1),fIdxFiPComp(-1)
{
}

//______________________________________________________________________________________________________________________
void MyPuls::Serialize(MyArchive &ar, MyOriginalEvent &oe) 
{
	//--read the puls properties from archive--//
	ar >> fIdxFiPOrig;
	ar >> fDataLength;

	//--calculate the size we need for the given length of the puls--//
	size_t DataSize = fDataLength * oe.GetNbrBytes();

	//--tell the parent Channel to prepare the compressed waveform with the size--//
	//--get the pointer to where we can write the data from the file now--//
	//--get index of where we are in the in the compressed char wavefrom--//
	char * Data=0;
	oe.GetChannel(fPCN).PrepareCompressedWaveform(DataSize,Data,fIdxFiPComp);
	
	//--read the waveform into the buffer given by data--//
	ar.readArray(Data, DataSize);
}