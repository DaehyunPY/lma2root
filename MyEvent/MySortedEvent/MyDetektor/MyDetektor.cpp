#include <iostream>

#include "MyDetektor.h"
#include "MyDetektorInfo.h"


//______________________________________________________________________________________________________________________
MyDetektor::MyDetektor(int dn, const MyDetektorInfo &di):
	fDetNbr(dn)
{
	ReadFromDetektorInfo(di);
}

//________________________________________________________________________________________________________________________
MyDetektorHit& MyDetektor::AddHit()
{
	//add a hit to this
	fHits.push_back(MyDetektorHit(fDetNbr,fHits.size()));
	return fHits.back();
}

//______________________________________________________________________________________________________________________
void MyDetektor::ReadFromDetektorInfo(const MyDetektorInfo &di)
{
	//copy information from the Detektor Info Class//
	fName			= di.GetName();

	fU1prop			= di.GetU1Prop();
	fU2prop			= di.GetU2Prop();
	fV1prop			= di.GetV1Prop();
	fV2prop			= di.GetV2Prop();
	fW1prop			= di.GetW1Prop();
	fW2prop			= di.GetW2Prop();
	fMcpprop		= di.GetMcpProp();
	
	fRuntime		= di.GetRunTime();
	fTsuLow			= di.GetTsuLow();
	fTsuHeigh		= di.GetTsuHeigh();
	fTsvLow			= di.GetTsvLow();
	fTsvHeigh		= di.GetTsvHeigh();
	fTswLow			= di.GetTswLow();
	fTswHeigh		= di.GetTswHeigh();
	fSfu			= di.GetSfU();
	fSfv			= di.GetSfV();
	fSfw			= di.GetSfW();
	fWOff			= di.GetWOffset();
	fMCPRadius		= di.GetMCPRadius();
	fDeadMCP		= di.GetDeadTimeMCP();
	fDeadAnode		= di.GetDeadTimeAnode();
	fDetCenterX		= di.GetDetCenterX();
	fDetCenterY		= di.GetDetCenterY();
	fCalib			= di.DoCalibration();
	fUseSorter		= di.ActivateSorter();
	fUseMCP			= di.UseMCP();
	fHex			= di.IsHexAnode();
	fSorterMethod	= di.GetSorterMethod();

	fUCorrPoints	= di.GetUCorrPoints();
	fVCorrPoints	= di.GetVCorrPoints();
	fWCorrPoints	= di.GetWCorrPoints();
}