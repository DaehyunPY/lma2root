#ifndef __MyPuls_h__
#define __MyPuls_h__

#include <vector>
#include <TObject.h>

class MyArchive;
class MyOriginalEvent;

class MyPuls
{
public:
	MyPuls():
		fPCN(-1),
		fPulsNbr(-1),
		fDataLength(0),
		fIdxFiPOrig(-1),
		fIdxFiPComp(-1)
																	{}

	MyPuls(int ParentChannelNbr, int PulsNbr);

public:
	void			 Serialize(MyArchive&, MyOriginalEvent&);

public:
	long			 GetLength()const								{return fDataLength;}
	long			 GetIndexToFirstPointOfOriginalWaveform()const	{return fIdxFiPOrig;}
	long			 GetIndexToFirstPointOfCompressedWaveform()const{return fIdxFiPComp;}
	int				 GetParentChannelNbr()const						{return fPCN;}
	int				 GetPulsNbr()const								{return fPulsNbr;}

private:
	long			 fDataLength;									//length of Trace in units of points of the trace
	long			 fIdxFiPOrig;									//index to first point in the whole trace in units of the original data type
	long			 fIdxFiPComp;									//index to first point in the zero supressd trace in units of the char waveform
	int				 fPCN;											//the number of the channel that this puls belongs to
	int				 fPulsNbr;										//the number of this puls in the parent channel

	ClassDef(MyPuls,1)												//Subset of Channels' Trace
};
#endif