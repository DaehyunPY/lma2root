#ifndef __MyDetektor_H_
#define __MyDetektor_H_

#include <TString.h>
#include <TObject.h>
#include <vector>

#include "MyDetektorHit.h"
#include "MyLayerProperty.h"

#include "../MyPunkt/MyPunkt.h"


class MyDetektorInfo;
class MyDetektorHitSorter;
class MyOriginalEvent;
class MySignalAnalyzedEvent;


typedef std::vector<MyDetektorHit> DetHitVec;
typedef std::vector<MyPunkt> PunktVec;
class MyDetektor
{
public:
	MyDetektor()													{}
	MyDetektor(int DetektorNbr, const MyDetektorInfo&);

public:
	MyDetektorHit				&AddHit();
	void						 ReadFromDetektorInfo(const MyDetektorInfo&);

public:
	void						 Clear()							{fHits.clear();}

public:
	size_t						 GetNbrOfHits() const				{return fHits.size();}
	MyDetektorHit				&GetHit(const int idx)				{return fHits[idx];}
	const MyDetektorHit			&GetHit(const int idx) const		{return fHits[idx];}

	int							 GetDetektorNbr()const				{return fDetNbr;}
	const char *				 GetName()const						{return fName.Data();}

	double						 GetRunTime()const					{return fRuntime;}
	double						 GetTsu()const						{return 0.5*(fTsuLow+fTsuHeigh);}
	double						 GetTsv()const						{return 0.5*(fTsvLow+fTsvHeigh);}
	double						 GetTsw()const						{return 0.5*(fTswLow+fTswHeigh);}
	double						 GetTsuWidth()const					{return fTsuLow-fTsuHeigh;}
	double						 GetTsvWidth()const					{return fTsvLow-fTsvHeigh;}
	double						 GetTswWidth()const					{return fTswLow-fTswHeigh;}
	double						 GetTsuLow()const					{return fTsuLow;}
	double						 GetTsuHeigh()const					{return fTsuHeigh;}
	double						 GetTsvLow()const					{return fTsvLow;}
	double						 GetTsvHeigh()const					{return fTsvHeigh;}
	double						 GetTswLow()const					{return fTswLow;}
	double						 GetTswHeigh()const					{return fTswHeigh;}
	double						 GetSfU()const						{return fSfu;}
	double						 GetSfV()const						{return fSfv;}
	double						 GetSfW()const						{return fSfw;}
	double						 GetWOffset()const					{return fWOff;}
	double						 GetMCPRadius()const				{return fMCPRadius;}
	double						 GetDeadTimeAnode()const			{return fDeadAnode;}
	double						 GetDeadTimeMCP()const				{return fDeadMCP;}
	double						 GetDetCenterX()const				{return fDetCenterX;}
	double						 GetDetCenterY()const				{return fDetCenterY;}
	const MyLayerProperty		&GetU1Prop()const					{return fU1prop;}
	MyLayerProperty				&GetU1Prop()						{return fU1prop;}
	const MyLayerProperty		&GetU2Prop()const					{return fU2prop;}
	MyLayerProperty				&GetU2Prop()						{return fU2prop;}
	const MyLayerProperty		&GetV1Prop()const					{return fV1prop;}
	MyLayerProperty				&GetV1Prop()						{return fV1prop;}
	const MyLayerProperty		&GetV2Prop()const					{return fV2prop;}
	MyLayerProperty				&GetV2Prop()						{return fV2prop;}
	const MyLayerProperty		&GetW1Prop()const					{return fW1prop;}
	MyLayerProperty				&GetW1Prop()						{return fW1prop;}
	const MyLayerProperty		&GetW2Prop()const					{return fW2prop;}
	MyLayerProperty				&GetW2Prop()						{return fW2prop;}
	const MyLayerProperty		&GetMcpProp()const					{return fMcpprop;}
	MyLayerProperty				&GetMcpProp()						{return fMcpprop;}
	const bool					 DoCalibration()const				{return fCalib;}
	const bool					 ActivateSorter()const				{return fUseSorter;}
	const bool					 UseMCP()const						{return fUseMCP;}
	const bool					 IsHexAnode()const					{return fHex;}	
	int							 GetNbrSumUCorrPoints()const		{return fUCorrPoints.size();}
	int							 GetNbrSumVCorrPoints()const		{return fVCorrPoints.size();}
	int							 GetNbrSumWCorrPoints()const		{return fWCorrPoints.size();}
	double						 GetUCorrPos(int idx)const			{return fUCorrPoints[idx].x();}
	double						 GetUCorrCorr(int idx)const			{return fUCorrPoints[idx].y();}
	double						 GetVCorrPos(int idx)const			{return fVCorrPoints[idx].x();}
	double						 GetVCorrCorr(int idx)const			{return fVCorrPoints[idx].y();}
	double						 GetWCorrPos(int idx)const			{return fWCorrPoints[idx].x();}
	double						 GetWCorrCorr(int idx)const			{return fWCorrPoints[idx].y();}

private:
	DetHitVec					 fHits;								//Container storing the refrences to the DetektorHits of this Detektor
	int							 fDetNbr;							//the number of this det

	//these values are stored in the Info Class, they don't need to be recorded
	TString						 fName;								//! the name of this Detektor
	MyLayerProperty				 fU1prop;							//! properties of U1 Signals for this detektor
	MyLayerProperty				 fU2prop;							//! properties of U2 Signals for this detektor
	MyLayerProperty				 fV1prop;							//! properties of V1 Signals for this detektor
	MyLayerProperty				 fV2prop;							//! properties of V2 Signals for this detektor
	MyLayerProperty				 fW1prop;							//! properties of W1 Signals for this detektor
	MyLayerProperty				 fW2prop;							//! properties of W2 Signals for this detektor
	MyLayerProperty				 fMcpprop;							//! properties of MCP Signals for this detektor
	double						 fRuntime;							//! the runtime over the anode
	double						 fTsuLow;							//! lower edge of the timesum of u
	double						 fTsuHeigh;							//! upper edge of the timesum of u
	double						 fTsvLow;							//! lower edge of the timesum of v
	double						 fTsvHeigh;							//! upper edge of the timesum of v
	double						 fTswLow;							//! lower edge of the timesum of w
	double						 fTswHeigh;							//! upper edge of the timesum of w
	double						 fSfu;								//! scalefactor for u-layer
	double						 fSfv;								//! scalefactor for v-layer
	double						 fSfw;								//! scalefactor for w-layer
	double						 fWOff;								//! the offset of w-layer towards u and v-layer
	double						 fMCPRadius;						//! the radius of the MCP in mm
	double						 fDeadMCP;							//! the Deadtime between to Signals on the MCP
	double						 fDeadAnode;						//! the Deadtime between to Signals on the Layers
	double						 fDetCenterX;						//! with this the center of the detektor in ns is moved to 0
	double						 fDetCenterY;						//! with this the center of the detektor in ns is moved to 0
	bool						 fCalib;							//! flag telling wether we should do a calibration
	bool						 fUseMCP;							//! flag telling Achims Routine to use the MCP
	bool						 fUseSorter;						//! flag telling wether we should use the achims sorter now
	bool						 fHex;								//! flag telling wether this is a Hexanode Detektor
	short						 fSorterMethod;						//! flag telling which Method to sort the times is used 0=Simple Sorting, 1=Achims Sorting
	PunktVec					 fUCorrPoints;						//! Vector of Correction Points for U-Layer
	PunktVec					 fVCorrPoints;						//! Vector of Correction Points for V-Layer
	PunktVec					 fWCorrPoints;						//! Vector of Correction Points for W-Layer

	ClassDef(MyDetektor,1)											//a Detektor containing detektorhits
};
#endif





