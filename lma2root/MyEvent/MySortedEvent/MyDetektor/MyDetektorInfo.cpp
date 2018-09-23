#include "MyDetektorInfo.h"

#include "../../../MySettings/MySettings.h"
#include "../../MySignalAnalyzedEvent/MySignalAnalyzedEventInfo.h"
#include "../../MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannelInfo.h"
#include "../../MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.h"
#include "../../../MyDetektorHitSorter/MyDetektorHitSorter.h"



//______________________________________________________________________________________________________________________
MyDetektorInfo::MyDetektorInfo(int dn, MySettings &set, const MySignalAnalyzedEventInfo &saei):
	fDetNbr(dn)
{
	ReadSettings(set,saei);
}

//______________________________________________________________________________________________________________________
bool SetTimesumWalkCalibration(const char * detname, const char * layername, MySettings &set, PunktVec &points);
bool SetSignalProperties(const char * detname, const char * layername, const MySignalAnalyzedEventInfo &ei, MySettings &set, MyLayerProperty &prop);
bool MyDetektorInfo::ReadSettings(MySettings &set, const MySignalAnalyzedEventInfo &ei)
{
	bool changed=false;
	//get your name from the settings//
	fName = set.GetString(Form("Detektor_%d",fDetNbr+1),Form("Detektor_%d",fDetNbr+1));

	//read all the values from the settings
	//check wether something has changed
	double  tempD;
	int		tempI;
	short	tempS;
	bool	tempB;
	
	//is hex flag
	tempB = (int)(set.GetValue(Form("%s_IsHexAnode",GetName()),0)+0.1);
	changed = (tempB != fHex) || changed;
	fHex = tempB;

	//runtime
	tempD = set.GetValue(Form("%s_Runtime",GetName()),220);
	changed = (TMath::Abs(tempD - fRuntime) > 1.e-4) || changed;
	fRuntime = tempD;

	//timesums
	//u
	tempD = set.GetValue(Form("%s_TimesumULow",GetName()),0);
	changed = (TMath::Abs(tempD - fTsuLow) > 1.e-4) || changed;
	fTsuLow = tempD;
	tempD = set.GetValue(Form("%s_TimesumUHigh",GetName()),200);
	changed = (TMath::Abs(tempD - fTsuHeigh) > 1.e-4) || changed;
	fTsuHeigh = tempD;
	//v
	tempD = set.GetValue(Form("%s_TimesumVLow",GetName()),0);
	changed = (TMath::Abs(tempD - fTsvLow) > 1.e-4) || changed;
	fTsvLow = tempD;
	tempD = set.GetValue(Form("%s_TimesumVHigh",GetName()),200);
	changed = (TMath::Abs(tempD - fTsvHeigh) > 1.e-4) || changed;
	fTsvHeigh = tempD;
	//w
	tempD = set.GetValue(Form("%s_TimesumWLow",GetName()),0);
	changed = (TMath::Abs(tempD - fTswLow) > 1.e-4) || changed;
	fTswLow = tempD;
	tempD = set.GetValue(Form("%s_TimesumWHigh",GetName()),200);
	changed = (TMath::Abs(tempD - fTswHeigh) > 1.e-4) || changed;
	fTswHeigh = tempD;
	
	//scalefactors
	tempD = set.GetValue(Form("%s_ScalefactorU",GetName()),0.5);
	changed = (TMath::Abs(tempD - fSfu) > 1.e-4) || changed;
	fSfu = tempD;
	tempD = set.GetValue(Form("%s_ScalefactorV",GetName()),0.5);
	changed = (TMath::Abs(tempD - fSfv) > 1.e-4) || changed;
	fSfv = tempD;
	tempD = set.GetValue(Form("%s_ScalefactorW",GetName()),0.5);
	changed = (TMath::Abs(tempD - fSfw) > 1.e-4) || changed;
	fSfw = tempD;

	//w-layer offset
	tempD = set.GetValue(Form("%s_WLayerOffset",GetName()),0);
	changed = (TMath::Abs(tempD - fWOff) > 1.e-4) || changed;
	fWOff = tempD;

	//mcp radius
	tempD = set.GetValue(Form("%s_McpRadius",GetName()),44);
	changed = (TMath::Abs(tempD - fMCPRadius) > 1.e-4) || changed;
	fMCPRadius = tempD;

	//deadtimes
	tempD = set.GetValue(Form("%s_DeadTimeAnode",GetName()),20);
	changed = (TMath::Abs(tempD - fDeadAnode) > 1.e-4) || changed;
	fDeadAnode = tempD;
	tempD = set.GetValue(Form("%s_DeadTimeMcp",GetName()),20);
	changed = (TMath::Abs(tempD - fDeadMCP) > 1.e-4) || changed;
	fDeadMCP = tempD;

	//move center
	tempD = set.GetValue(Form("%s_CorrectCenterX",GetName()),0);
	changed = (TMath::Abs(tempD - fDetCenterX) > 1.e-4) || changed;
	fDetCenterX = tempD;
	tempD = set.GetValue(Form("%s_CorrectCenterY",GetName()),0);
	changed = (TMath::Abs(tempD - fDetCenterY) > 1.e-4) || changed;
	fDetCenterY = tempD;

	//calibration flag
	tempB = (int)(set.GetValue(Form("%s_DoCalibration",GetName()),0)+0.1);
	changed = (tempB != fCalib) || changed;
	fCalib = tempB;

	//calibration flag
	tempB = (int)(set.GetValue(Form("%s_ActivateSorter",GetName()),0)+0.1);
	changed = (tempB != fUseSorter) || changed;
	fUseSorter = tempB;

	//use mcp flag
	tempB = (int)(set.GetValue(Form("%s_UseMcp",GetName()),1)+0.1);
	changed = (tempB != fUseMCP) || changed;
	fUseMCP = tempB;

	//the sorter Method
	tempS = (short)(set.GetValue(Form("%s_SorterMethod",GetName()),MyDetektorHitSorter::kDoNothing)+0.1);
	changed = (tempS != fSorterMethod) || changed;
	fSorterMethod = tempS;

	//SignalProperties
	changed = SetSignalProperties(GetName(),"Mcp",ei,set,fMcpprop) || changed;
	changed = SetSignalProperties(GetName(),"U1",ei,set,fU1prop) || changed;
	changed = SetSignalProperties(GetName(),"U2",ei,set,fU2prop) || changed;
	changed = SetSignalProperties(GetName(),"V1",ei,set,fV1prop) || changed;
	changed = SetSignalProperties(GetName(),"V2",ei,set,fV2prop) || changed;
	if (fHex)
	{
		changed = SetSignalProperties(GetName(),"W1",ei,set,fW1prop) || changed;
		changed = SetSignalProperties(GetName(),"W2",ei,set,fW2prop) || changed;
	}

	//timesum walk calibration
	changed = SetTimesumWalkCalibration(GetName(),"U",set,fUCorrPoints) || changed;
	changed = SetTimesumWalkCalibration(GetName(),"V",set,fVCorrPoints) || changed;
	changed = SetTimesumWalkCalibration(GetName(),"W",set,fWCorrPoints) || changed;

	return changed;
}

//____________________________________________lokal global function to get the right section from the channels__________________________________________________________________________
bool SetTimesumWalkCalibration(const char * detname, const char * layername, MySettings &set, PunktVec &points)
{
	bool changed=false;

	//Nbr of correction Points for Layer//
	points.clear();
	int nCorrPoints = (int) set.GetValue(Form("%s_%sNbrOfCorrPts",detname,layername),0);
	//std::cout <<Form("%s_%sNbrOfCorrPts",detname,layername)<<std::endl;
	for (int i=0;i<nCorrPoints;++i)
	{
		double Pos = set.GetValue(Form("%s_Pos%s%03d",detname,layername,i),0);
		//std::cout << Form("%s_Pos%s%03d",detname,layername,i)<<std::endl;
		double Cor = set.GetValue(Form("%s_Cor%s%03d",detname,layername,i),0);
		//std::cout << Form("%s_Cor%s%03d",detname,layername,i)<<std::endl;
		points.push_back(MyPunkt(Pos,Cor));
	}
	return changed;
}

//____________________________________________lokal global function to get the right section from the channels__________________________________________________________________________
bool SetSignalProperties(const char * detname, const char * layername, const MySignalAnalyzedEventInfo &ei, MySettings &set, MyLayerProperty &prop)
{
	bool changed = false;
	TString s;
	s = set.GetString(Form("%s_%s",detname,layername),"");
	if (s.CompareTo("") == 0)	//fehler
	{
		std::cout << "there is no Channel Assignement for "<<layername<<" of Detektor "<<detname<<std::endl;
		exit(1);
	}
	TString ChanNbr = s(4,2);
	TString SecNbr = s(9,2);
	if (!ChanNbr.IsDigit() || !SecNbr.IsDigit())	//fehler
	{
		std::cout << "there is no correct Channel Assignement for "<<layername<<" of Detektor "<<detname<<std::endl;
		exit(1);
	}
	//extract the right channelsection from the right Channel and initialize the property with it//
	prop = ei.GetChannelInfo(ChanNbr.Atoi() - 1).GetChannelSection(SecNbr.Atoi() - 1);
	//tell the property its name//
	prop.SetName(Form("%s_%s",detname,layername));
	//let the property read its values from the settings//
	prop.ReadSettings(set);

	return changed;
}

