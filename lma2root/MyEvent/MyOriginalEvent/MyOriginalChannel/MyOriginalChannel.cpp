#include "MyOriginalChannel.h"
#include "MyOriginalChannelInfo.h"

#include "../MyOriginalEvent.h"
#include "../MyOriginalEventInfo.h"
#include "../../../MyArchive/MyArchive.h"

//______________________________________________________________________________________________________________________
MyOriginalChannel::MyOriginalChannel(int chNbr, const MyOriginalEventInfo &oei):
	fChNbr(chNbr),fPulses(),
	fFullscale(0),fOffset(0),fGain(0),fBaseline(0),fNoise(0),
	fStsi(0),fBs(0)
{
	ReadFromChannelInfo(oei);
}

//______________________________________________________________________________________________________________________
MyPuls& MyOriginalChannel::AddPuls()
{
	//add a puls to the container
	fPulses.push_back(MyPuls(fChNbr,fPulses.size()));
	return fPulses.back();
}

//______________________________________________________________________________________________________________________
void MyOriginalChannel::ReadFromChannelInfo(const MyOriginalEventInfo &oei)
{
	const MyOriginalChannelInfo &oci = oei.GetChannelInfo(fChNbr);

	fFullscale	= oci.GetFullScale();
	fOffset		= oci.GetOffset();
	fGain		= oci.GetVertGain();
	fBaseline	= oci.GetBaseline();
	fNoise		= oci.GetNoise();
	fStsi		= oci.GetStepSize();
	fBs			= oci.GetBackSize();
}

//______________________________________________________________________________________________________________________
void MyOriginalChannel::Serialize(MyArchive &ar, MyOriginalEvent &ev)
{
	//read the number of pulses in this channel from the archive
	short tmpNbrPulses;
	ar >> tmpNbrPulses;

	//add the pulses and read them from the archive
	for (int i=0; i<tmpNbrPulses; ++i)	
		AddPuls().Serialize(ar,ev);
}

//______________________________________________________________________________________________________________________
void MyOriginalChannel::PrepareCompressedWaveform(long DataSize, char *&DataPointer, long &idx)
{
	//first get the index to where the new part of the compressed waveform will be//
	idx = fZeroSuppressedWaveform.size();
	//resize the waveform, so it will fit the part of the waveform, that will be copied to it//
	fZeroSuppressedWaveform.resize(fZeroSuppressedWaveform.size()+DataSize);
	//get a pointer to the index we just saved//
	DataPointer = &fZeroSuppressedWaveform[idx];
}
//------------------------------------------------------------------------------------------------------------//
//-------------------------------Differential mode Now testing-----original code by Iwayama-------------------//
//------------------------------------------------------------------------------------------------------------//

void MyOriginalChannel::Differential(MyOriginalEvent &ev , const double dt_nsec, const double multi)
{
	for (size_t pulse=0; pulse<GetNbrPulses(); pulse++) 
	{
		//Get pointer of original buffer
		short *Buff = static_cast<short*>(GetDataPointerForPuls(GetPuls(pulse)));
		//Get length of buffer
		long Length = GetPuls(pulse).GetLength();

		//Create a temporary buff for differential calculation
		short *DiffBuff = new short[Length];
		//Init the differential buff by baseline (for the zeropoint)
		for (long p=0; p<Length;p++) {DiffBuff[p] = fBaseline;}
		//Calc differential by data[i+1] - data[i-1]

		int time_diff = static_cast<int>(dt_nsec / (ev.GetSampleInterval()*1.e9));

		for (int p=time_diff;p<Length-time_diff;p++) {
			DiffBuff[p] = static_cast<short>(((Buff[p+time_diff] - Buff[p-time_diff])*multi) / (2*time_diff) + fBaseline);
		}
		//Copy the temporary data to original buff

		for (long p=0;p<Length;p++) {
			Buff[p] = DiffBuff[p];
		}
		//Delete temporary buff. bye bye.
		if (DiffBuff != NULL) {
			delete DiffBuff;
		}

	}
	
	return;
}

void MyOriginalChannel::Smoothing(MyOriginalEvent &ev, const double SmoothingTime_ns)
{
	const int SmoothingFactor = static_cast<int>(SmoothingTime_ns / (ev.GetSampleInterval()*1.e9));

	for (size_t pulse=0; pulse<GetNbrPulses(); pulse++)
	{
		short *buff = static_cast<short*>(GetDataPointerForPuls(GetPuls(pulse)));
		long buffSize = GetPuls(pulse).GetLength();

		//perform simple smoothing (calc average)
		double *average = new double[buffSize];
		memset(average,0,sizeof(double)*buffSize);

		//calc average
		for (int i = SmoothingFactor/2; i<buffSize-SmoothingFactor/2;i++) {
			average[i] = 0.;
			for (int j=0;j<SmoothingFactor;j++) {
				average[i] += buff[i-SmoothingFactor/2 + j];
			}
		}

		for (int i=0;i<buffSize;i++) {
			buff[i] = fBaseline;
		}
		for (int i=SmoothingFactor/2;i<buffSize-SmoothingFactor/2;i++) {
			buff[i] = static_cast<short>(average[i] / SmoothingFactor);
		}

		delete average;
	}
	
}

void MyOriginalChannel::RemoveNoiseLongPulse(MyOriginalEvent &ev, const int maxLength)
{	
	for (size_t pulse=0; pulse<GetNbrPulses(); pulse++)
	{
		//Get pointer of original buffer
		short *Buff = static_cast<short*>(GetDataPointerForPuls(GetPuls(pulse)));
		//Get length of buffer
		long Length = GetPuls(pulse).GetLength();
		//std::cout<<maxLength<<std::endl;
		if (Length * ev.GetSampleInterval()*1.e+9 > maxLength) {
			for (int p=0;p<Length;p++) {
				Buff[p] = fBaseline;
			}
		}
	}
}

int MyOriginalChannel::DataSize(MyOriginalEvent &ev)
{
	int dataSize = 0;

	for (size_t pulse=0; pulse<GetNbrPulses(); pulse++)
	{
		dataSize += GetPuls(pulse).GetLength();
	}
	return dataSize*2;
}
