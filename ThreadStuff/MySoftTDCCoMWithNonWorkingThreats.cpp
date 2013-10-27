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
void *com(void * arg)
{
	MySoftTDCMessage stdcm;					//a container for the events to come

	//get the queue and semaphore from the argument//
	stdcQueue *q		= static_cast<MyThreadArgument*>(arg)->fQueue;
	MySemaphore *sem	= static_cast<MyThreadArgument*>(arg)->fSemaphore;

	while(1)
	{
		//wait until a Puls is in the queue
		//if there is a Puls in the queue retrieve it
		q->wait_and_pop(stdcm);

		//tell the semaphore that we are now working
		sem->Decrement();

		//extract some non changing informations for the analysis
		const MyOriginalEvent &oe	= *stdcm.GetOriginalEvent();
		const MyOriginalChannel &oc	= *stdcm.GetOriginalChannel();
		MySignalAnalyzedChannel &sac= *stdcm.GetSignalAnalyzedChannel();

		const long baseline = oc.GetBaseline();

		//go through all Pulses in the Channel//
		for (int pu=0;pu<oc.GetNbrPulses();++pu)
		{
			const MyPuls &Puls			= oc.GetPuls(pu);
			const long idxToFiPoint		= Puls.GetIndexToFirstPoint();
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

			const T *Data			= static_cast<T*>(Puls.GetData());
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
		//tell the semaphore that we are done working
		sem->Increment();
	}
	return 0;
}



//########################## 8 Bit Version ###########################################################################
//______________________________________________________________________________________________________________________
MySoftTDCCoM8Bit::MySoftTDCCoM8Bit(int NbrOfThreads):MySoftTDC(NbrOfThreads)
{
	for (int i=0;i<fNThreads;++i)
	{
		fThreads[i] = new TThread(Form("t%d",i), com<char>, (void*) &fArg[i]);
		fThreads[i]->Run();
	}
}
//########################## 16 Bit Version ###########################################################################
//______________________________________________________________________________________________________________________
 MySoftTDCCoM16Bit::MySoftTDCCoM16Bit(int NbrOfThreads):MySoftTDC(NbrOfThreads)
{
	for (int i=0;i<fNThreads;++i)
	{
		fThreads[i] = new TThread(Form("t%d",i), com<short>, (void*) &fArg[i]);
		fThreads[i]->Run();
	}
}