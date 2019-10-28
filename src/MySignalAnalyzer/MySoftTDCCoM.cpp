#include <iostream>

#include "MySoftTDCCoM.h"
#include "helperfunctionsforSTDC.h"

#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannel.h"
#include "../MyEvent/MyOriginalEvent/MyPuls/MyPuls.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannel.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.h"
#include "../MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h"

//______________________Implimentation of simple Version__________________________________________________________
template <typename T>
void com(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae)
{
	//go through all channels of the event//
	for (size_t iChan=0;iChan<oe.GetNbrOfChannels();++iChan)
	{
		const MyOriginalChannel &oc	= oe.GetChannel(iChan);
		MySignalAnalyzedChannel &sac= sae.GetChannel(iChan);

		const long baseline = oc.GetBaseline();

		//go through all Pulses in the Channel//
		for (int pu=0;pu<oc.GetNbrPulses();++pu)
		{
			const MyPuls &Puls			= oc.GetPuls(pu);
			const long idxToFiPoint		= Puls.GetIndexToFirstPointOfOriginalWaveform();
			const MyChannelSection *cs  = sac.GetPointerToChannelSectionForIndex(idxToFiPoint);
			if (!cs) continue;

			//check wether this puls only records a voltage//
			if(cs->IsOnlyVoltage())
			{
				//extract the  voltage and end here//
				extractVoltage<T>(oc,Puls,*cs,sac);
				continue;
			}

			//get the threshold from the settings
			const double threshold  = cs->GetThreshold() / oc.GetVertGain();	//mV -> ADC Bytes

			const T *Data			= static_cast<const T*>(oc.GetDataPointerForPuls(Puls));
			const size_t pLength	= Puls.GetLength();
			bool risingEdge			= false;
			bool firsttime			= true;
			long startpos			= -1;

			//--go through the puls--//
			for (int i=3; i<pLength;++i)
			{
				if (   (abs(Data[i] - baseline) <= threshold)					//checking for inside noise
					|| ( i == pLength-1)										//check if we are at the second to last index
					|| ( ( (Data[i]-baseline)*(Data[i-1]-baseline) ) < 0.))		//check wether we go through the zero
				{
					if (risingEdge)			//if we had a rising edge before we know that it was a real peak
					{
						//--add a new peak to the puls--//
						MyPeak &Peak = sac.AddPeak(pu);
						//--set all known settings--//
						Peak.SetStartPos(startpos);
						Peak.SetStopPos(i-1);

						//--height stuff--//
						maximum<T>(oe,oc,Peak);
						
						//--fwhm stuff--//
						fwhm<T>(oe,oc,Peak);

						//--center of mass stuff--//
						CoM<T>(oe,oc,Peak);
						
						//--Time is the Center of Mass--//
						Peak.SetTime(Peak.GetCoM());

						//--check the polarity--//
						if (Data[Peak.GetMaxPos()]-baseline == Peak.GetMaximum())			//positive
							Peak.SetPolarity(kPositive);
						else if (Data[Peak.GetMaxPos()]-baseline == -Peak.GetMaximum())	//negative
							Peak.SetPolarity(kNegative);
						else																//error: polarity found
							Peak.SetPolarity(kBad);				
						

					}
					risingEdge = false;
					firsttime=true;
				}
				else		//if the above is not true then we are outside the noise
				{
					if(firsttime)	//if it is the firsttime we are outside the noise
					{
						firsttime = false;
						startpos= i;			//remember the position
					}

					//--if haven't found a risingEdge before check if we have--//
					if (!risingEdge)
					{
						if ((abs(Data[i-3]-baseline) < abs(Data[i-2]-baseline)) && 
							(abs(Data[i-2]-baseline) < abs(Data[i-1]-baseline)) && 
							(abs(Data[i-1]-baseline) < abs(Data[i  ]-baseline)) )
						{
							risingEdge=true;
						}
					}
				}
			}
		}
	}
}



//########################## 8 Bit Version ###########################################################################
//______________________________________________________________________________________________________________________
void MySoftTDCCoM8Bit::FindPeaksIn(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae, bool blcorr)
{
	com<char>(oe,sae);
}
//########################## 16 Bit Version ###########################################################################
//______________________________________________________________________________________________________________________
void MySoftTDCCoM16Bit::FindPeaksIn(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae, bool blcorr)
{
	com<short>(oe,sae);
}