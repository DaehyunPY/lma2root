#include "MySignalAnalyzedChannel.h"
#include "MySignalAnalyzedChannelInfo.h"


//______________________________________________________________________________________________________________________
MySignalAnalyzedChannel::MySignalAnalyzedChannel(long chNbr, const MySignalAnalyzedChannelInfo &saci):
	fChNbr(chNbr)
{
	ReadFromChannelInfo(saci);
}

//______________________________________________________________________________________________________________________
double MySignalAnalyzedChannel::GetVoltageForChannelSection(int csn)const
{
	double v=0;
	for(size_t i=0;i<fVoltages.size();++i)
	{
		if(fVoltages[i].GetChannelSectionNbr() == csn)
			v += fVoltages[i].GetVolt();
	}
	return v;
}

//______________________________________________________________________________________________________________________
const MyChannelSection* MySignalAnalyzedChannel::GetPointerToChannelSectionForIndex(long idx)const
{
	//go through all channelsection and chekc wether the index is in theier range
	for (size_t i=0;i<fChannelSections.size();++i)
		if((fChannelSections[i].GetTimeRangeLow() <= idx) && (idx < fChannelSections[i].GetTimeRangeHigh()))
			return &fChannelSections[i];
	return 0;
}

//______________________________________________________________________________________________________________________
MyPeak& MySignalAnalyzedChannel::AddPeak(int PulsNbr)
{
	//add a peak to the container
	fPeaks.push_back(MyPeak(fChNbr,PulsNbr,fPeaks.size()));
	return fPeaks.back();
}


//______________________________________________________________________________________________________________________
void MySignalAnalyzedChannel::ReadFromChannelInfo(const MySignalAnalyzedChannelInfo &saci)
{
	//clear vector and reinitialize it from the info
	fChannelSections.clear();
	for (int i=0;i<saci.GetNbrOfChannelSections();++i)
		fChannelSections.push_back(MyChannelSection(saci.GetChannelSection(i)));
}

//_________________________________________________________________________________________________________________moto
void MySignalAnalyzedChannel::DelPeak()
{
	//delete a peak from container
	fPeaks.pop_back();
}