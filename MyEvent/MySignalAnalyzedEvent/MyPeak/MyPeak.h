#ifndef __MyPeak_H__
#define __MyPeak_H__

#include <TObject.h>

enum EPolarity
{
	kBad=0,
	kPositive,
	kNegative
};


class MyPeak
{
public:
	MyPeak()								{}
	MyPeak(int ParentChannelNbr, int ParentPulsNbr, int PeakNbr);

public:
	void   Clear();

public:
	void   SetTime(double in)				{fTime = in;}
	double GetTime() const					{return fTime;}

	void   SetCoM(double in)				{fCom = in;}
	double GetCoM() const					{return fCom;}

	void   SetCFD(double in)				{fCfd = in;}
	double GetCFD() const					{return fCfd;}
	
	void   SetIntegral(double in)			{fIntegral = in;}
	double GetIntegral() const				{return fIntegral;}

	void   SetHeight(double in)				{fHeight = in;}
	double GetHeight() const				{return fHeight;}
	
	void   SetHeightAb(double in)			{fHeightAbziehen = in;}
	double GetHeightAb() const				{return fHeightAbziehen;}
	
	void   SetWidth(double in)				{fWidth = in;}
	double GetWidth() const					{return fWidth;}

	void   SetFWHM(double in)				{fFwhm = in;}
	double GetFWHM() const					{return fFwhm;}

	void   SetPosHalfLeft(double in)		{fPosHalfLeft = in;}
	double GetPosHalfLeft() const			{return fPosHalfLeft;}

	void   SetPosHalfRight(double in)		{fPosHalfRight = in;}
	double GetPosHalfRight() const			{return fPosHalfRight;}

	void   SetSlope(double in)				{fSlope = in;}
	double GetSlope() const					{return fSlope;}

	void   SetStartPos(long in)				{fStartpos = in;}
	long   GetStartPos() const				{return fStartpos;}

	void   SetStopPos(long in)				{fStoppos = in;}
	long   GetStopPos() const				{return fStoppos;}

	void   SetMaxPos(long in)				{fMaxpos = in;}
	long   GetMaxPos() const				{return fMaxpos;}

	void   SetMaximum(double in)			{fMaximum = in;}
	double GetMaximum() const				{return fMaximum;}

	void   SetPolarity(long in)				{fPolarity = in;}
	long   GetPolarity() const				{return fPolarity;}

	void   IsUsed(bool in)					{fUsed = in;}
	bool   IsUsed() const					{return fUsed;}

	int	   GetParentPulsNbr()const			{return fPPN;}
	int    GetParentChannelNbr()const		{return fPCN;}
	int    GetPeakNbr()const				{return fPN;}

private:
	double fTime;						//the time of the peaks, calculated from either cfd or com
	double fCfd;						//the time calculated from cfd
	double fCom;						//the time calculated form com

	long   fPolarity;					//the polarity of the peak
	double fSlope;						//the slope of this peak

	long   fMaxpos;						//the position where the maximum of peak is
	double fMaximum;					//the height in bits
	double fHeight;						//the height in mV
	double fHeightAbziehen;				//the height when you use the substraction cfd

	double fFwhm;						//the fwhm of the peak
	double fWidth;						//the width at the bottom of the peak
	double fPosHalfLeft;				//the pos where the left edge crosses the half of the height
	double fPosHalfRight;				//the pos where the right edge crosses the half of the height

	double fIntegral;					//the integral of the peak

	long   fStartpos;					//the start postion of the peak
	long   fStoppos;					//the stop position of the peak

	bool   fUsed;						//flag wether this peak has been used in sorting the detektorhits

	int    fPCN;						//the number of the parent Channel
	int    fPPN;						//the number of the parent Puls
	int    fPN;							//the number of the this Peak

	ClassDef(MyPeak,1)					//A Peak
};

#endif