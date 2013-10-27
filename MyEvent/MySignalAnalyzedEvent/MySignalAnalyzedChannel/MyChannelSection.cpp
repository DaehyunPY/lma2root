#include "MyChannelSection.h"

#include "../../../MySettings/MySettings.h"
#include "../../MyOriginalEvent/MyOriginalEventInfo.h"


//______________________________________________________________________________________________________________________
MyChannelSection::MyChannelSection(int chnbr, int secnbr, MySettings &set, const MyOriginalEventInfo &oei):
	  fLow(-1),fHigh(-1),
	  fChNbr(chnbr), fPolarity(-1),fSecNbr(secnbr),
	  fD(0),fW(0),fF(0),fT(0),fMPuls(0),fMSlope(0)	
{
	fName=Form("Chan%02dSec%02d",fChNbr+1,fSecNbr+1);
	ReadSettings(set,oei);
}

//______________________________________________________________________________________________________________________
void MyChannelSection::operator=(const MyChannelSection &ics)
{
	//copy the informations from the incomming channelsection (ics)//
	fLow		= ics.fLow;
	fHigh		= ics.fHigh;
	fChNbr		= ics.fChNbr;
	fSecNbr		= ics.fSecNbr;
	fD			= ics.fD;
	fW			= ics.fW;
	fF			= ics.fF;
	fT			= ics.fT;
	//fMH			= ics.fMH;//the peak height in mV-------------------------!!!!!!!!!!!
	fPolarity	= ics.fPolarity;
	fMSlope		= ics.fMSlope;
	fVoltage	= ics.fVoltage;
	fMPuls		= ics.fMPuls;
	fName		= ics.fName;
}

//______________________________________________________________________________________________________________________
bool MyChannelSection::ReadSettings(MySettings &set, const MyOriginalEventInfo &oei)
{
	bool changed=false;
	//read all the values from the settings
	//check wether something has changed
	double  tempD;
	int		tempI;
	bool    tempB;

	//time range lower edge//
	tempI = (int)(set.GetValue(Form("%s_RangeFrom",GetName()),0)+0.1);
	changed = (tempI != fLow) || changed;
	fLow = tempI;
	//time range upper edge//
	tempI = (int)(set.GetValue(Form("%s_RangeTo",GetName()),oei.GetNbrSamples())+0.1);
	changed = (tempI != fHigh) || changed;
	fHigh = tempI;
	//Votage Flag//
	tempB = (int)(set.GetValue(Form("%s_IsOnlyVoltage",GetName()),0)+0.1);
	changed = (tempB != fVoltage) || changed;
	fVoltage = tempB;
	//Delay//
	tempI = (int)(set.GetValue(Form("%s_Delay",GetName()),5)+0.1);
	changed = (tempI != fD) || changed;
	fD = tempI;
	//Walk//
	tempD = set.GetValue(Form("%s_Walk",GetName()),0);
	changed = (TMath::Abs(tempD - fW) > 1.e-4) || changed;
	fW = tempD;
	//Fraction//
	tempD = set.GetValue(Form("%s_Fraction",GetName()),0.4);
	changed = (TMath::Abs(tempD - fF) > 1.e-4) || changed;
	fF = tempD;
	//Threshold//
	tempD = set.GetValue(Form("%s_Threshold",GetName()),oei.GetChannelInfo(fChNbr).GetNoise()*oei.GetChannelInfo(fChNbr).GetVertGain());
	changed = (TMath::Abs(tempD - fT) > 1.e-4) || changed;
	fT = tempD;
	//MaxHeight//
	//tempD = set.GetValue(Form("%s_MaxHeight",GetName()),5000);
	//changed = (TMath::Abs(tempD - fMH) > 1.e-4) || changed;
	//fMH = tempD;

	//MeanPulsSlope//
	tempD = set.GetValue(Form("%s_MeanPulsSlope",GetName()),0);
	changed = (TMath::Abs(tempD - fMSlope) > 1.e-4) || changed;
	fMSlope = tempD;
	//MeanPuls//
	tempI = (int)(set.GetValue(Form("%s_NbrMeanPulsPoints",GetName()),0)+0.1); //read how many point the meanpuls has
	if (fMPuls.size() != tempI) //if it doesn't match the points this meanpuls has, reread it from the settings
	{
		fMPuls.clear();
		changed = true;
		for (int i=0;i<tempI;++i)
		{
			tempD = set.GetValue(Form("%s_MeanPuls%02d",GetName(),i),0);
			fMPuls.push_back(tempD);
		}
	}

	return changed;
}
