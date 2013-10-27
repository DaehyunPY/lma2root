#include <iostream>
#include <TString.h>

#include "MyOriginalEvent.h"
#include "MyOriginalEventInfo.h"

#include "./MyOriginalChannel/MyOriginalChannel.h"
#include "../../MyArchive/MyArchive.h"
#include "../../MyRootManager/MyHistos.h"

//______________________________________________________________________________________________________________________
void MyOriginalEvent::Serialize(MyArchive &ar)
{
	//--read the event values from the archive--//
	ar >> (long)fEventID;				//event time stamp
	//std::cerr<<"EventID:"<<EventID<<std::endl;
	ar >> (double)fHorpos;				//horpos from acqiris
	//std::cerr<<"horpos:"<<horpos<<std::endl;

	//--if the channel was recorded serialize it
	for (size_t i=0; i<fChannels.size();++i)
		if (fUsedChans & (0x1<<i))
			GetChannel(i).Serialize(ar,*this);
}

//______________________________________________________________________________________________________________________
void MyOriginalEvent::ReadFromEventInfo(const MyOriginalEventInfo &ei)
{
	//copy the values from the Event Header//
	fNbrBytes			= ei.GetNbrBytes();
	fSampInter			= ei.GetSampleInterval();
	fNbrSamples			= ei.GetNbrSamples();
	fDelayTime			= ei.GetDelayTime();
	fTrigChan			= ei.GetTriggerChannel();
	fTrigLevel			= ei.GetTriggerLevel();
	fTrigSlope			= ei.GetTriggerSlope();
	fUsedChans			= ei.GetUsedChannels();
	fChanCombUsedChans	= ei.GetChanCombUsedChannels();
	fNbrConPerCh		= ei.GetNbrConvPerChans();

	//channels
	fChannels.clear();
	for (size_t i=0;i<ei.GetNbrOfChannels();++i)
			fChannels.push_back(MyOriginalChannel(i,ei));
}

//______________________________________________________________________________________________________________________
void MyOriginalEvent::Clear()
{
	//--Clear the Channels, don't empty the Container--//
	for (size_t i = 0; i < fChannels.size(); ++i)
		fChannels[i].Clear();
}

//______________________________________________________________________________________________________________________
void MyOriginalEvent::ClearChannel(size_t ch)//motomura
{
	fChannels[ch].Clear();
}

//______________________________________________________________________________________________________________________
void MyOriginalEvent::Differential(const double dt_nsec, const double multi)
{
	for (size_t i=0; i<7;++i)
		if (fUsedChans & (0x1<<i))
			GetChannel(i).Differential(*this,dt_nsec,multi);
}

void MyOriginalEvent::Smoothing(const double SmoothingTime_ns)
{
	for (size_t i=0; i<7;++i)
		if (fUsedChans & (0x1<<i))
			GetChannel(i).Smoothing(*this,SmoothingTime_ns);
}

void MyOriginalEvent::RemoveNoiseLongPulse(const int maxLength)
{
		for (size_t i=0; i<7;++i)
		if (fUsedChans & (0x1<<i))
			GetChannel(i).RemoveNoiseLongPulse(*this,maxLength);
}

//------------------------------------------------------------------------------------------------------------------------
//void MyOriginalEvent::MakeDataHisto(MyHistos &rm)
//{
//	const int IDOffset = 10000;
//	for (size_t i=0; i<8; i++)
//	{
//		char* HistoName = Form("Ch%i_DataSize",i+1);
//		rm.create1d(IDOffset+i,HistoName,"DataSize(Kbytes)",100,0,100,"Data");
//	}
//	rm.create1d(IDOffset+8,"AllCh_DataSize","DataSize(Kbytes)",1000,0,1000,"Data");
//	rm.create1d(IDOffset+9,"DiffEventID","DiffEventID(ms)",1000,0,1000,"Data");
//	rm.create2d(IDOffset+10,"DataSizeVsEventID","DataSize(Kbytes)","DiffEventID(ms)",1000,0,1000,1000,0,1000,"Data");
//
//}

//void MyOriginalEvent::FillDataHisto(MyHistos &rm)
//{
//	const int IDOffset = 10000;
//	double AllChDataSize = 0;
//
//	for (size_t i=0; i<8; i++)
//	{
//		 double dataSize = GetChannel(i).DataSize(*this)/1000.0;
//		 AllChDataSize += dataSize;
//		 rm.fill1d(IDOffset+i,dataSize);
//	}
//	rm.fill1d(IDOffset+8,AllChDataSize);
//	rm.fill1d(IDOffset+9,(fEventID-fEventIDThen));
//	rm.fill2d(IDOffset+10,AllChDataSize,(fEventID-fEventIDThen));
//
//	fEventIDThen = fEventID;
//
//}

