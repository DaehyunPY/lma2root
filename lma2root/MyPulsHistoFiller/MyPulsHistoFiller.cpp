#include <iostream>
#include <iomanip>
#include <fstream>
#include <TH1.h>

#include "MyPulsHistoFiller.h"

#include "../MyRootManager/MyHistos.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalEventInfo.h"
#include "../MyEvent/MyOriginalEvent/MyPuls/MyPuls.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannel.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEventInfo.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.h"
#include "../MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h"


//____________________________find nbr of peaks of a given puls_______________________________________________________________________________________________________________________________
int findNbrPeaksFor(int PulsNbr, const MySignalAnalyzedChannel &sac)
{
	int cnt=0;
	for (size_t i=0;i<sac.GetNbrPeaks();++i)
		if(sac.GetPeak(i).GetParentPulsNbr() == PulsNbr)
			++cnt;
	return cnt;
}
//_____________________________write mean puls to file______________________________________________________________________________________________________________________________
void extractNormPulsAndSlope(TH1* p, TH1* ps, const char * ChannelSectionName, ofstream &file)
{
	//write slope
	file << ChannelSectionName<< "_MeanPulsSlope=" << ps->GetMean() << std::endl;

	//write mean Signal
	file << ChannelSectionName<< "_NbrMeanPulsPoints=" << p->GetNbinsX() << std::endl;
	double Maximum = p->GetMaximum();
	for (int i=1; i <= p->GetNbinsX(); ++i)
	{
		file << ChannelSectionName << "_MeanPuls" << std::setw(3) << std::setfill('0') << i << "=" << (p->GetBinContent(i) / Maximum) << "\t";
		if((i%5)==0) file << std::endl;
	}
	file << std::endl;
}
//____________________________all channelsections________________________________________________________________________________________________________
void writeMeanPulsHistos(const MyOriginalEventInfo &oei, const MySignalAnalyzedEventInfo &saei, const MyHistos &rm)
{
	int idxOffset=0;
	for (int ic=0;ic<oei.GetNbrOfChannels();++ic)
	{
		if (oei.GetUsedChannels() & (0x1<<ic))
		{
			const MySignalAnalyzedChannelInfo &saci = saei.GetChannelInfo(ic);
			for (int ics=0; ics < saci.GetNbrOfChannelSections(); ++ics)
			{
				const MyChannelSection &cs = saci.GetChannelSection(ics);
				if (!cs.IsOnlyVoltage())
				{
					//if ChannelSection is not for just storing voltage, output the meanpuls and meanpulsslope
					//create a file that the infos for this Channel Section are stored in//
					std::ofstream f(Form("MeanPulses_%s.txt",cs.GetName()));
					//Positive//
					//get the indizes of the histograms where the infos are stored in//
					const int mpidxPos	= idxOffset + 0;
					const int mpsidxPos	= idxOffset + 1;
					//add caption and write the settings//
					f<<"######Positive Polarity########"<<std::endl;
					extractNormPulsAndSlope(rm.getHist(mpidxPos),rm.getHist(mpsidxPos),cs.GetName(),f);
					f<<std::endl<<std::endl<<std::endl;

					//Negative//
					//get the indizes of the histograms where the infos are stored in//
					const int mpidxNeg	= idxOffset + 4;
					const int mpsidxNeg	= idxOffset + 5;
					//add caption and write the settings//
					f<<"######Negative Polarity########"<<std::endl;
					extractNormPulsAndSlope(rm.getHist(mpidxPos),rm.getHist(mpsidxPos),cs.GetName(),f);
				}
				//for each ChannelSection we have to increase the index offset eight times
				idxOffset += 8;
			}
		}
	}
}
//____________________________to create the meanpuls_____________________________________________________________________________________________________________
template <typename T>
double gmp(const double i, const MyOriginalEvent &e, const MyOriginalChannel &c, const MyPuls &pu, const MyPeak &p)
{
	//first extract the baseline of the channel that this peak belongs to//
	double baseline			= c.GetBaseline();
	const T  *Puls			= static_cast<const T*>(c.GetDataPointerForPuls(pu));
	const long pulsLength	= pu.GetLength();
	const double x0			= p.GetTime()/(e.GetSampleInterval()*1e9) - pu.GetIndexToFirstPointOfOriginalWaveform()- e.GetHorpos();

	//i is the pos in the mean Pulse//
	//we need to know the distance of this pos to the wanted center of the mean pulse(50)//
	const double Abstand = 50.- i;
	
	//the distance in the Pulse is the same//
	//so the wanted pos in the Puls is at x0 - distance//
	const double x = x0-Abstand;

	//truncate this number and you know the bin left to the wanted x point//
	const int binLeft = (int) x;
	const int binRight = binLeft+1;

	
	//check wether we are still in range//
	if (binLeft  < 0) return 0;
	if (binRight > pulsLength) return 0;

	//make a line through the leftbin and the right bin//
	//give back the y for the asked x//
	const double m = Puls[binRight] - Puls[binLeft];
	const double ret = m*(x-binLeft) + Puls[binLeft];

	return (ret-baseline);
}


//_________________________________fill the mean puls histos______________________________________________
template <typename T>
void fillMeanPulsAndFwhmHistos(MyHistos &hi, const int off, const MyOriginalEvent &e, const MyOriginalChannel &c, const MyPuls &pu, const MyPeak &p)
{
	//otherwise go through the complete Puls and fill it in the histos//
	double integral = p.GetIntegral();

	//get histos with right indizes//
	const int mp1didx	= off + 4*(p.GetPolarity()-1) + 0;
	const int mpsidx	= off + 4*(p.GetPolarity()-1) + 1;
	const int mp2didx	= off + 4*(p.GetPolarity()-1) + 2;
	const int fwphidx	= off + 4*(p.GetPolarity()-1) + 3;

	//go throug all entries of the mean puls histograms and find the right y value//
	for (int i=0;i<hi.getHist(mp1didx)->GetNbinsX();++i)
	{
		double mv = gmp<T>(i,e,c,pu,p);
		hi.fill1d(mp1didx,i,mv);
		hi.fill2d(mp2didx,i,mv/integral);
	}
	//fill slope histogram//
	//hi.fill1d(mpsidx,TMath::Abs(p.GetSlope()/p.GetMaximum()));
	hi.fill1d(mpsidx, TMath::Abs(p.GetSlope()));
	//fill fwhm vs ph histogram//
	hi.fill2d(fwphidx,p.GetFWHM(),p.GetHeight());
}

//_________________________________all Signal Histos_______________________________________________________
template <typename T>
void fillPulsHistosImpl(const MyOriginalEvent &oe, const MySignalAnalyzedEvent &sae, MyHistos &rm)
{
	int idxOffset=0;
	for (int ic=0;ic<oe.GetNbrOfChannels();++ic)
	{
		if (oe.GetUsedChannels() & (0x1<<ic))
		{
			const MySignalAnalyzedChannel &sac = sae.GetChannel(ic);
			for (int ics=0; ics < sac.GetNbrOfChannelSections(); ++ics)
			{
				const MyChannelSection &cs = sac.GetChannelSection(ics);
				if (!cs.IsOnlyVoltage())
				{
					//if channelsection is not just a voltage then go through all peaks
					for (int ip=0; ip < sac.GetNbrPeaks(); ++ip)
					{
						const MyPeak			&p  = sac.GetPeak(ip);
						const MyOriginalChannel &oc = oe.GetChannel(p.GetParentChannelNbr());
						const MyPuls			&pu = oc.GetPuls(p.GetParentPulsNbr());
						//if the channelsection of the peak is the channelsection that we 
						//look at right now, then fill the mean puls histograms
						//but only if the puls has just one peak//
						if (*(sac.GetPointerToChannelSectionForIndex(pu.GetIndexToFirstPointOfOriginalWaveform())) == cs)
						if (findNbrPeaksFor(pu.GetPulsNbr(),sac) == 1)
							fillMeanPulsAndFwhmHistos<T>(rm,idxOffset,oe,oc,pu,p);
					}
				}
				else
				{	
					//otherwise get the Voltage for this channelsection from the channel
					rm.fill1d(idxOffset,sac.GetVoltageForChannelSection(cs.GetChannelSectionNbr()));
				}
				//for each ChannelSection we have to increase the index offset eight times
				idxOffset += 8;
			}
		}
	}
}
//####################################################################################################################
//_____________________________________________Base______________________________________________________________________________________________________________
MyPulsHistoFillerBase::MyPulsHistoFillerBase(const MyOriginalEventInfo &oei, const MySignalAnalyzedEventInfo &saei, MyHistos &rm)
{
	//go through all channels and create the Histograms for the mean pulses//
	int idxOffset=0;
	for (int ic=0; ic < saei.GetNbrOfChannels(); ++ic)
	{
		if (oei.GetUsedChannels() &(0x1<<ic))
		{
			const MyOriginalChannelInfo		  &oci  = oei.GetChannelInfo(ic);
			const MySignalAnalyzedChannelInfo &saci = saei.GetChannelInfo(ic);
			for (int ics=0;ics < saci.GetNbrOfChannelSections();++ics)
			{
				const MyChannelSection &cs = saci.GetChannelSection(ics);
				if (!cs.IsOnlyVoltage())
				{
					//create a dir for this channel section//
					const char * dirnamePos = Form("Channel%02d/Section%02d(%d...%d)/Positive",ic+1,ics+1,cs.GetTimeRangeLow(),cs.GetTimeRangeHigh());
					const char * dirnameNeg = Form("Channel%02d/Section%02d(%d...%d)/Negative",ic+1,ics+1,cs.GetTimeRangeLow(),cs.GetTimeRangeHigh());

					const int mpidxPos		= idxOffset + 0;
					const int mpsidxPos		= idxOffset + 1;
					const int mp2didxPos	= idxOffset + 2;
					const int hwidxPos		= idxOffset + 3;

					const int mpidxNeg		= idxOffset + 4;
					const int mpsidxNeg		= idxOffset + 5;
					const int mp2didxNeg	= idxOffset + 6;
					const int hwidxNeg		= idxOffset + 7;

					rm.create1d(mpidxPos,"MPuls","Puls",150,0,150,dirnamePos);
					rm.create1d(mpidxNeg,"MPuls","Puls",150,0,150,dirnameNeg);

					//rm.create1d(mpsidxPos,"MPulsSlope","#frac{Slope}{height}",400,0,1,dirnamePos);
					//rm.create1d(mpsidxNeg,"MPulsSlope","#frac{Slope}{height}",400,0,1,dirnameNeg);
					rm.create1d(mpsidxPos, "PulsSlope", "Slope", 400, 0, 10000, dirnamePos);
					rm.create1d(mpsidxNeg, "PulsSlope", "Slope", 400, 0, 10000, dirnameNeg);

					rm.create2d(mp2didxPos, "MPuls2D", "Puls", "", 150, 0, 150, 400, -0.2, 0.4, dirnamePos);
					rm.create2d(mp2didxNeg,"MPuls2D","Puls","",150,0,150,400,-0.4,0.2,dirnameNeg);

					rm.create2d(hwidxPos,"HeightVsFWHM","fwhm [ns]","U [mV]",300,0,30,256,0,oci.GetFullScale(),dirnamePos);
					rm.create2d(hwidxNeg,"HeightVsFWHM","fwhm [ns]","U [mV]",300,0,30,256,0,oci.GetFullScale(),dirnameNeg);
				}
				else
				{
					const char * dirname = Form("Channel%02d/Section%02d(%d...%d)",ic+1,ics+1,cs.GetTimeRangeLow(),cs.GetTimeRangeHigh());
					rm.create1d(idxOffset,"Voltage","U [mV]",200,-0.5*oci.GetFullScale(),0.5*oci.GetFullScale(),dirname);
				}
				idxOffset += 8;
			}
		}
	}
}
//_____________________________________________8 Bit______________________________________________________________________________________________________________
void MyPulsHistoFiller8Bit::WritePulsHistosToFile(const MyOriginalEventInfo &oei, const MySignalAnalyzedEventInfo &saei, const MyHistos &rm)
{
	writeMeanPulsHistos(oei,saei,rm);
}
void MyPulsHistoFiller8Bit::FillPulsHistos(const MyOriginalEvent &oe, const MySignalAnalyzedEvent &sae, MyHistos &rm)
{
	fillPulsHistosImpl<char>(oe,sae,rm);
}

//_____________________________________________16 Bit______________________________________________________________________________________________________________
void MyPulsHistoFiller16Bit::WritePulsHistosToFile(const MyOriginalEventInfo &oei, const MySignalAnalyzedEventInfo &saei, const MyHistos &rm)
{
	writeMeanPulsHistos(oei,saei,rm);
}
void MyPulsHistoFiller16Bit::FillPulsHistos(const MyOriginalEvent &oe, const MySignalAnalyzedEvent &sae, MyHistos &rm)
{
	fillPulsHistosImpl<short>(oe,sae,rm);
}


//_____________________________________________init the actual worker______________________________________________________________________________________________________________
MyPulsHistoFiller::~MyPulsHistoFiller()
{
	//std::cout << "deleting pulshistofiller"<<std::endl;
	delete fPhf;
	//std::cout << "done"<<std::endl;
}
void MyPulsHistoFiller::Init(bool fill, const MyOriginalEventInfo &oei, const MySignalAnalyzedEventInfo &saei, MyHistos &rm)
{
	//do we have to fill something//
	if(!fill)
	{
		//if fFill was true before (initial state) delete the old filler and create a "non filling" filler//
		if(fFill != fill)
		{
			delete fPhf;
			fPhf = new MyPulsHistoFillerDoNothing();
		}
		fFill = fill;
	}
	else
	{
		//check wether the nbr of bytes have changed//
		if(oei.GetNbrBytes() != fNBytes)
		{
			delete fPhf;
			fNBytes = oei.GetNbrBytes();

			//if we want to fill create filler according to the bytes we have//
			if (fNBytes == 1)
				fPhf = new MyPulsHistoFiller8Bit(oei,saei,rm);
			else if (fNBytes == 2)
				fPhf = new MyPulsHistoFiller16Bit(oei,saei,rm);
			else
			{
				std::cout <<"No PulsHistofiller for "<<fNBytes*8<<" Bits"<<std::endl;
				exit(1);
			}
		}
		fFill=true;
	}
}