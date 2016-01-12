#include "MySoftTDCCFD.h"
#include "helperfunctionsforSTDC.h"
#include "baselineCorrection.h"

#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannel.h"
#include "../MyEvent/MyOriginalEvent/MyPuls/MyPuls.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannel.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.h"
#include "../MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h"

//________________________________Implematation of Constant Fraction Method______________________________________________________
//________________this will be a thread that is waiting for Pulses to be added to a queue________________________________________
template <typename T>
void cfd(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae, std::vector<double> &cData)
{
	//go through all channels of this event//
	for(size_t iChan=0;iChan<oe.GetNbrOfChannels();++iChan)
	{
		//now extract information from the Channel and the Event
		const MyOriginalChannel &oc	= oe.GetChannel(iChan);
		MySignalAnalyzedChannel &sac= sae.GetChannel(iChan);

		const long baseline			= oc.GetBaseline();
		const double horpos			= oe.GetHorpos()*1.e9;
		const double vertGain		= oc.GetVertGain();
		const double sampleInterval	= oe.GetSampleInterval()*1e9;	//convert the s to ns

		//go through all pulses of channel//
		for (int pu=0;pu<oc.GetNbrPulses();++pu)
		{
			const MyPuls &Puls			= oc.GetPuls(pu);
			const long idxToFiPoint		= Puls.GetIndexToFirstPointOfOriginalWaveform();
			const T *Data				= static_cast<const T*>(oc.GetDataPointerForPuls(Puls));
			const size_t pLength		= Puls.GetLength();
			const MyChannelSection *cs	= sac.GetPointerToChannelSectionForIndex(idxToFiPoint);
			if (!cs) continue;

			//check wether this puls only records a voltage//
			if(cs->IsOnlyVoltage())
			{
				//extract the  voltage and end here//
				extractVoltage<T>(oc,Puls,*cs,sac);
				continue;
			}

			//get the meanpuls stuff//
			const double MPulsSlope	= cs->GetMPulsSlope();

			//--get the right cfd settings--//
			const long delay		= static_cast<long>(cs->GetDelay() / sampleInterval);	//ns -> sampleinterval units
			const double walk		= cs->GetWalk() / vertGain;								//mV -> ADC Bytes
			const double threshold	= cs->GetThreshold() / vertGain;						//mV -> ADC Bytes
			const double fraction	= cs->GetFraction();

			//Get limit of slope
			const double limSlope = cs->GetLimSlope();

			//copy the waveform to the new array//
			//if the copy puls is not big enough to hold the original puls then the container will be resized//
			cData.resize(pLength);
			double  *tmp1 = &cData[0];	//we can't use memcopy here, because we need the typecast
			const T *tmp2 = Data;
			for (int i=0;i<pLength;++i)
				*tmp1++ = *tmp2++;

			//--go through the puls--//
			for (int i=delay+1; i<pLength-2;++i)
			{
				const double fx  = cData[i] - static_cast<double>(baseline);		//the original Point at i
				const double fxd = cData[i-delay] - static_cast<double>(baseline);	//the delayed Point	at i 
				const double fsx = -fx*fraction + fxd;								//the calculated CFPoint at i

				const double fx_1  = cData[i+1] - static_cast<double>(baseline);		//original Point at i+1
				const double fxd_1 = cData[i+1-delay] - static_cast<double>(baseline);	//delayed Point at i+1
				const double fsx_1 = -fx_1*fraction + fxd_1;							//calculated CFPoint at i+1

				//check wether the criteria for a Peak are fullfilled
				if (((fsx-walk) * (fsx_1-walk)) <= 0 )	//one point above one below the walk
				if (TMath::Abs(fx) > threshold)				//original point above the threshold
				{
					//--it could be that the first criteria is 0 because	--//
					//--one of the Constant Fraction Signal Points or both	--//
					//--are exactly where the walk is						--//
					if (TMath::Abs(fsx-fsx_1) < 1e-8)	//both points are on the walk
					{
						//--go to next loop until at least one is over or under the walk--//
						continue;
					}
					else if ((fsx-walk) == 0)		//only first is on walk
					{
						//--Only the fist is on the walk, this is what we want--//
						//--so:do nothing--//
					}
					else if ((fsx_1-walk) == 0)		//only second is on walk
					{
						//--we want that the first point will be on the walk,--//
						//--so in the next loop this point will be the first.--//
						continue;
					}
					//does the peak have the right polarity?//
					//if two pulses are close together then the cfsignal goes through the walk//
					//three times, where only two crossings are good. So we need to check for//
					//the one where it is not good//
					if (fsx      > fsx_1)		//neg polarity
					if (cData[i] > baseline)	//but pos Puls .. skip
						continue;
					if (fsx      < fsx_1)		//pos polarity
					if (cData[i] < baseline)	//but neg Puls .. skip
						continue;
					

					//--later we need two more points, create them here--//
					const double fx_m1 = cData[i-1] - static_cast<double>(baseline);		//the original Point at i-1
					const double fxd_m1 = cData[i-1-delay] - static_cast<double>(baseline);	//the delayed Point	at i-1 
					const double fsx_m1 = -fx_m1*fraction + fxd_m1;							//the calculated CFPoint at i-1

					const double fx_2 = cData[i+2] - static_cast<double>(baseline);			//original Point at i+2
					const double fxd_2 = cData[i+2-delay] - static_cast<double>(baseline);	//delayed Point at i+2
					const double fsx_2 = -fx_2*fraction + fxd_2;							//calculated CFPoint at i+2

					
					//--find x with a linear interpolation between the two points--//
					const double m = fsx_1-fsx;					//(fsx-fsx_1)/(i-(i+1));
					const double xLin = i + (walk - fsx)/m;		//PSF fx = (x - i)*m + cfs[i]

					//--make a linear regression to find the slope of the leading edge--//
					double mslope,cslope;
					const double xslope[3] = {i-delay,i+1-delay,i+2-delay};
					const double yslope[3] = {fxd,fxd_1,fxd_2};
					linearRegression(3,xslope,yslope,mslope,cslope);

					//--find x with a cubic polynomial interpolation between four points--//
					//--do this with the Newtons interpolation Polynomial--//
					const double x[4] = {i-1,i,i+1,i+2};				//x vector
					const double y[4] = {fsx_m1,fsx,fsx_1,fsx_2};		//y vector
					double coeff[4] = {0,0,0,0};				//Newton coeff vector
					createNewtonPolynomial(x,y,coeff);

					//--numericaly solve the Newton Polynomial--//
					//--give the lineare approach for x as Start Value--//
					const double xPoly = findXForGivenY(x,coeff,walk,xLin);
					const double pos = xPoly + static_cast<double>(idxToFiPoint) + horpos;

					//--add a new peak--//
					MyPeak &Peak = sac.AddPeak(pu);

					//add the info//
					Peak.SetCFD(pos*sampleInterval);
					Peak.SetTime(pos*sampleInterval);
					if (fsx > fsx_1) Peak.SetPolarity(kNegative);		//Peak has Neg Pol
					if (fsx < fsx_1) Peak.SetPolarity(kPositive);		//Peak has Pos Pol
					if (TMath::Abs(fsx-fsx_1) < 1e-8) Peak.SetPolarity(kBad);//Peak has Bad Pol
					
					//slope of peak//
					Peak.SetSlope(mslope);

					//--start and stop of the puls--//
					startstop<T>(oe,oc,Peak);

					//--height of peak--//
					maximum<T>(oe,oc,Peak);

					//--width & fwhm of peak--//
					fwhm<T>(oe,oc,Peak);

					//delete bad peak
					if ((mslope < limSlope) && (iChan < 8 - 1)) sac.DelPeak();

					//--the com and integral--//
					CoM<T>(oe,oc,Peak);

					//--Peak height limit--//moto
					//if ((Peak.GetHeight()>cs->GetMaxHeight())) sac.DelPeak();


					//if there is a MeanPuls given, substract it from this puls//
					if (cs->GetNbrOfMPulsPoints())
					{
						//subtract this peak from the puls
						for (int j=0; j<pLength; ++j)
							cData[j] -= (gmz(static_cast<double>(j),xPoly,cs->GetMPuls())*(mslope/MPulsSlope));
						//start from begining
						//i=delay-1;
					}
				}
			}
		}
	}
}


//###################################################################################################

template <typename T>
void cfdBLCorr(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae, std::vector<double> &cData)
{
	std::vector<double> BLData;
	//go through all channels of this event//
	for(size_t iChan=0;iChan<oe.GetNbrOfChannels();++iChan)
	{
		//now extract information from the Channel and the Event
		const MyOriginalChannel &oc	= oe.GetChannel(iChan);
		MySignalAnalyzedChannel &sac= sae.GetChannel(iChan);

		const long baseline			= oc.GetBaseline();
		const double horpos			= oe.GetHorpos()*1.e9;
		const double vertGain		= oc.GetVertGain();
		const double sampleInterval	= oe.GetSampleInterval()*1e9;	//convert the s to ns
		const double offset			= static_cast<double>(baseline);

		//go through all pulses of channel//
		for (int pu=0;pu<oc.GetNbrPulses();++pu)
		{
			const MyPuls &Puls			= oc.GetPuls(pu);
			const long idxToFiPoint		= Puls.GetIndexToFirstPointOfOriginalWaveform();
			const T *Data				= static_cast<const T*>(oc.GetDataPointerForPuls(Puls));
			const size_t pLength		= Puls.GetLength();
			const MyChannelSection *cs	= sac.GetPointerToChannelSectionForIndex(idxToFiPoint);
			if (!cs) continue;

			//check wether this puls only records a voltage//
			if(cs->IsOnlyVoltage())
			{
				//extract the  voltage and end here//
				extractVoltage<T>(oc,Puls,*cs,sac);
				continue;
			}

			//get the meanpuls stuff//
			const double MPulsSlope	= cs->GetMPulsSlope();

			//--get the right cfd settings--//
			const long delay		= static_cast<long>(cs->GetDelay() / sampleInterval);	//ns -> sampleinterval units
			const double walk		= cs->GetWalk() / vertGain;								//mV -> ADC Bytes
			const double threshold	= cs->GetThreshold() / vertGain;						//mV -> ADC Bytes
			const double fraction	= cs->GetFraction();

			//Get limit of slope
			const double limSlope = cs->GetLimSlope();

			//copy the waveform to the new array//
			//if the copy puls is not big enough to hold the original puls then the container will be resized//
			cData.resize(pLength);
			BLData.resize(pLength);
			double  *tmp1 = &cData[0];	//we can't use memcopy here, because we need the typecast
			const T *tmp2 = Data;
			for (int i=0;i<pLength;++i)
				*tmp1++ = *tmp2++;

			//----------------BaseLine Correction----------------//
			if ((pLength>100)&&(iChan!=(8-1)))
			{
				BLCorr(&cData[0], &BLData[0],offset,pLength,50, 1);
				for (size_t i=0; i<pLength; i++) cData[i]=cData[i]-BLData[i]+offset;
			}

			//--go through the puls--//
			for (int i=delay+1; i<pLength-2;++i)
			{
				const double fx  = cData[i] - offset;		//the original Point at i
				const double fxd = cData[i-delay] - offset;	//the delayed Point	at i 
				const double fsx = -fx*fraction + fxd;								//the calculated CFPoint at i

				const double fx_1  = cData[i+1] - offset;		//original Point at i+1
				const double fxd_1 = cData[i+1-delay] - offset;	//delayed Point at i+1
				const double fsx_1 = -fx_1*fraction + fxd_1;							//calculated CFPoint at i+1

				//check wether the criteria for a Peak are fullfilled
				if (((fsx-walk) * (fsx_1-walk)) <= 0 )	//one point above one below the walk
				if (TMath::Abs(fx) > threshold)				//original point above the threshold
				{
					//--it could be that the first criteria is 0 because	--//
					//--one of the Constant Fraction Signal Points or both	--//
					//--are exactly where the walk is						--//
					if (TMath::Abs(fsx-fsx_1) < 1e-8)	//both points are on the walk
					{
						//--go to next loop until at least one is over or under the walk--//
						continue;
					}
					else if ((fsx-walk) == 0)		//only first is on walk
					{
						//--Only the fist is on the walk, this is what we want--//
						//--so:do nothing--//
					}
					else if ((fsx_1-walk) == 0)		//only second is on walk
					{
						//--we want that the first point will be on the walk,--//
						//--so in the next loop this point will be the first.--//
						continue;
					}
					//does the peak have the right polarity?//
					//if two pulses are close together then the cfsignal goes through the walk//
					//three times, where only two crossings are good. So we need to check for//
					//the one where it is not good//
					if (fsx      > fsx_1)		//neg polarity
					if (cData[i] > offset)	//but pos Puls .. skip
						continue;
					if (fsx      < fsx_1)		//pos polarity
					if (cData[i] < offset)	//but neg Puls .. skip
						continue;
					

					//--later we need two more points, create them here--//
					const double fx_m1 = cData[i-1] - offset;		//the original Point at i-1
					const double fxd_m1 = cData[i-1-delay] - offset;	//the delayed Point	at i-1 
					const double fsx_m1 = -fx_m1*fraction + fxd_m1;							//the calculated CFPoint at i-1

					const double fx_2 = cData[i+2] - offset;			//original Point at i+2
					const double fxd_2 = cData[i+2-delay] - offset;	//delayed Point at i+2
					const double fsx_2 = -fx_2*fraction + fxd_2;							//calculated CFPoint at i+2

					
					//--find x with a linear interpolation between the two points--//
					const double m = fsx_1-fsx;					//(fsx-fsx_1)/(i-(i+1));
					const double xLin = i + (walk - fsx)/m;		//PSF fx = (x - i)*m + cfs[i]

					//--make a linear regression to find the slope of the leading edge--//
					double mslope,cslope;
					const double xslope[3] = {i-delay,i+1-delay,i+2-delay};
					const double yslope[3] = {fxd,fxd_1,fxd_2};
					linearRegression(3,xslope,yslope,mslope,cslope);

					//--find x with a cubic polynomial interpolation between four points--//
					//--do this with the Newtons interpolation Polynomial--//
					const double x[4] = {i-1,i,i+1,i+2};				//x vector
					const double y[4] = {fsx_m1,fsx,fsx_1,fsx_2};		//y vector
					double coeff[4] = {0,0,0,0};				//Newton coeff vector
					createNewtonPolynomial(x,y,coeff);

					//--numericaly solve the Newton Polynomial--//
					//--give the lineare approach for x as Start Value--//
					const double xPoly = findXForGivenY(x,coeff,walk,xLin);
					const double pos = xPoly + static_cast<double>(idxToFiPoint) + horpos;

					//--add a new peak--//
					MyPeak &Peak = sac.AddPeak(pu);

					//add the info//
					Peak.SetCFD(pos*sampleInterval);
					Peak.SetTime(pos*sampleInterval);
					if (fsx > fsx_1) Peak.SetPolarity(kNegative);		//Peak has Neg Pol
					if (fsx < fsx_1) Peak.SetPolarity(kPositive);		//Peak has Pos Pol
					if (TMath::Abs(fsx-fsx_1) < 1e-8) Peak.SetPolarity(kBad);//Peak has Bad Pol
					
					//slope of peak//
					Peak.SetSlope(mslope);

					//--start and stop of the puls--//
					startstop<T>(oe,oc,Peak);

					//--height of peak--//
					maximum<T>(oe,oc,Peak);

					//--width & fwhm of peak--//
					fwhm<T>(oe,oc,Peak);

					//delete bad peak
					if ((mslope < limSlope) && (iChan < 8 - 1)) sac.DelPeak();

					//--the com and integral--//
					CoM<T>(oe,oc,Peak);

					//if there is a MeanPuls given, substract it from this puls//
					if (cs->GetNbrOfMPulsPoints())
					{
						//subtract this peak from the puls
						for (int j=0; j<pLength; ++j)
							cData[j] -= (gmz(static_cast<double>(j),xPoly,cs->GetMPuls())*(mslope/MPulsSlope));
						//start from begining
						//i=delay-1;
					}
				}
			}
		}
	}
}

//########################## 8 Bit Version ###########################################################################
//______________________________________________________________________________________________________________________
void MySoftTDCCFD8Bit::FindPeaksIn(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae, bool blcorr)
{
	if (blcorr) cfdBLCorr<char>(oe, sae, cData);
	else cfd<char>(oe,sae,cData);
}

//########################## 16 Bit Version ###########################################################################
//______________________________________________________________________________________________________________________
void MySoftTDCCFD16Bit::FindPeaksIn(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae, bool blcorr)
{
	//std::cout << blcorr << std::endl;
	if (blcorr) cfdBLCorr<short>(oe, sae, cData);
	else cfd<short>(oe, sae, cData);
	
}