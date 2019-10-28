#ifndef __MyDetektorInfo_h__
#define __MyDetektorInfo_h__

#include <TString.h>

#include "MyLayerProperty.h"
#include "../MyPunkt/MyPunkt.h"

class MySettings;
class MySignalAnalyzedEventInfo;

typedef std::vector<MyPunkt> PunktVec;
class MyDetektorInfo
{
public:
	MyDetektorInfo()												{}
	MyDetektorInfo(int detektorNbr, MySettings&, const MySignalAnalyzedEventInfo&);

public:
	bool						 ReadSettings(MySettings&, const MySignalAnalyzedEventInfo&);

public:
	double						 GetTsuLow()const					{return fTsuLow;}
	double						 GetTsuHeigh()const					{return fTsuHeigh;}
	double						 GetTsvLow()const					{return fTsvLow;}
	double						 GetTsvHeigh()const					{return fTsvHeigh;}
	double						 GetTswLow()const					{return fTswLow;}
	double						 GetTswHeigh()const					{return fTswHeigh;}
	double						 GetTsu()const						{return 0.5*(fTsuLow+fTsuHeigh);}
	double						 GetTsv()const						{return 0.5*(fTsvLow+fTsvHeigh);}
	double						 GetTsw()const						{return 0.5*(fTswLow+fTswHeigh);}
	double						 GetTsuWidth()const					{return fTsuHeigh-fTsuLow;}
	double						 GetTsvWidth()const					{return fTsvHeigh-fTsvLow;}
	double						 GetTswWidth()const					{return fTswHeigh-fTswLow;}
	double						 GetRunTime()const					{return fRuntime;}
	double						 GetSfU()const						{return fSfu;}
	double						 GetSfV()const						{return fSfv;}
	double						 GetSfW()const						{return fSfw;}
	double						 GetWOffset()const					{return fWOff;}
	double						 GetMCPRadius()const				{return fMCPRadius;}
	double						 GetDeadTimeAnode()const			{return fDeadAnode;}
	double						 GetDeadTimeMCP()const				{return fDeadMCP;}
	double						 GetDetCenterX()const				{return fDetCenterX;}
	double						 GetDetCenterY()const				{return fDetCenterY;}
	MyLayerProperty				&GetU1Prop()						{return fU1prop;}
	const MyLayerProperty		&GetU1Prop()const					{return fU1prop;}
	MyLayerProperty				&GetU2Prop()						{return fU2prop;}
	const MyLayerProperty		&GetU2Prop()const					{return fU2prop;}
	MyLayerProperty				&GetV1Prop()						{return fV1prop;}
	const MyLayerProperty		&GetV1Prop()const					{return fV1prop;}
	MyLayerProperty				&GetV2Prop()						{return fV2prop;}
	const MyLayerProperty		&GetV2Prop()const					{return fV2prop;}
	MyLayerProperty				&GetW1Prop()						{return fW1prop;}
	const MyLayerProperty		&GetW1Prop()const					{return fW1prop;}
	MyLayerProperty				&GetW2Prop()						{return fW2prop;}
	const MyLayerProperty		&GetW2Prop()const					{return fW2prop;}
	MyLayerProperty				&GetMcpProp()						{return fMcpprop;}
	const MyLayerProperty		&GetMcpProp()const					{return fMcpprop;}
	bool						 DoCalibration()const				{return fCalib;}
	bool						 ActivateSorter()const				{return fUseSorter;}
	bool						 UseMCP()const						{return fUseMCP;}
	bool						 IsHexAnode()const					{return fHex;}
	short						 GetSorterMethod()const				{return fSorterMethod;}
	PunktVec					 GetUCorrPoints()const				{return fUCorrPoints;}
	PunktVec					 GetVCorrPoints()const				{return fVCorrPoints;}
	PunktVec					 GetWCorrPoints()const				{return fWCorrPoints;}
	int							 GetNbrSumUCorrPoints()const		{return fUCorrPoints.size();}
	int							 GetNbrSumVCorrPoints()const		{return fVCorrPoints.size();}
	int							 GetNbrSumWCorrPoints()const		{return fWCorrPoints.size();}
	double						 GetUCorrPos(int idx)const			{return fUCorrPoints[idx].x();}
	double						 GetUCorrCorr(int idx)const			{return fUCorrPoints[idx].y();}
	double						 GetVCorrPos(int idx)const			{return fVCorrPoints[idx].x();}
	double						 GetVCorrCorr(int idx)const			{return fVCorrPoints[idx].y();}
	double						 GetWCorrPos(int idx)const			{return fWCorrPoints[idx].x();}
	double						 GetWCorrCorr(int idx)const			{return fWCorrPoints[idx].y();}

	const char 					*GetName()const						{return fName.Data();}
	int							 GetDetektorNbr()const				{return fDetNbr;}

private:	
	TString						 fName;								//the name of the detektor
	int							 fDetNbr;

	MyLayerProperty				 fU1prop;							//properties of U1 Signals for this detektor
	MyLayerProperty				 fU2prop;							//properties of U2 Signals for this detektor
	MyLayerProperty				 fV1prop;							//properties of V1 Signals for this detektor
	MyLayerProperty				 fV2prop;							//properties of V2 Signals for this detektor
	MyLayerProperty				 fW1prop;							//properties of W1 Signals for this detektor
	MyLayerProperty				 fW2prop;							//properties of W2 Signals for this detektor
	MyLayerProperty				 fMcpprop;							//properties of MCP Signals for this detektor
	double						 fRuntime;							//the runtime over the anode
	double						 fTsuLow;							//lower edge of the timesum of u

	double						 fTsuHeigh;							//upper edge of the timesum of u
	double						 fTsvLow;							//lower edge of the timesum of v
	double						 fTsvHeigh;							//upper edge of the timesum of v
	double						 fTswLow;							//lower edge of the timesum of w
	double						 fTswHeigh;							//upper edge of the timesum of w
	double						 fSfu;								//scalefactor for u-layer
	double						 fSfv;								//scalefactor for v-layer
	double						 fSfw;								//scalefactor for w-layer
	double						 fWOff;								//the offset of w-layer towards u and v-layer
	double						 fMCPRadius;						//the radius of the MCP in mm
	double						 fDeadMCP;							//the Deadtime between to Signals on the MCP
	double						 fDeadAnode;						//the Deadtime between to Signals on the Layers
	double						 fDetCenterX;						//with this the center of the detektor in ns is moved to 0
	double						 fDetCenterY;						//with this the center of the detektor in ns is moved to 0
	bool						 fUseMCP;							//flag telling Achims Routine to use the MCP
	bool						 fHex;								//flag telling wether this is a Hexanode Detektor
	short						 fSorterMethod;						//flag telling which Method to sort the times is used 0=Simple Sorting, 1=Achims Sorting
	PunktVec					 fUCorrPoints;						//Vector of Correction Points for U-Layer
	PunktVec					 fVCorrPoints;						//Vector of Correction Points for V-Layer
	PunktVec					 fWCorrPoints;						//Vector of Correction Points for W-Layer
	
	//this is information for the sorter, makes no sense to record it
	bool						 fCalib;						//! flag telling wether we should do a calibration
	bool						 fUseSorter;					//! flag telling wether we should use the achims sorter now

	ClassDef(MyDetektorInfo,1)					//Contains Info about a Channel
};
#endif
