#ifndef __MyChannelSection_h_
#define __MyChannelSection_h_

#include <TString.h>
#include <vector>

class MySettings;
class MyOriginalEventInfo;

typedef std::vector<double> dvec;
class MyChannelSection
{
public:
	MyChannelSection()							{}
	MyChannelSection(int chnbr, int secnbr, MySettings&, const MyOriginalEventInfo&);
	MyChannelSection(const MyChannelSection &cs)				{*this = cs;}
	
	void			operator=(const MyChannelSection &);
	bool			operator==(const MyChannelSection&cs)const	{return (this == &cs);}
	
public:
	bool			ReadSettings(MySettings&, const MyOriginalEventInfo&);

	void			ClearCompletely()			{fMPuls.clear();}

	//void			SetChannelNbr(long in)		{fChNbr = in;}
	const long		GetChannelNbr() const		{return fChNbr;}

	//void			SetSectionNbr(long in)		{fSecNbr = in;}
	const long		GetChannelSectionNbr()const	{return fSecNbr;}

	//void			SetTimeRangeLow(long in) 	{fLow = in;}
	const long		GetTimeRangeLow() const		{return fLow;}

	//void			SetTimeRangeHigh(long in) 	{fHigh = in;}
	const long		GetTimeRangeHigh() const	{return fHigh;}

	void			SetDelay(long in)			{fD = (in<0)? 0:in;}
	long			GetDelay() const			{return fD;}

	void			SetWalk(double in)			{fW = in;}
	double			GetWalk() const				{return fW;}

	void			SetThreshold(double in)		{fT = in;}
	double			GetThreshold() const		{return fT;}

	//void			SetMaxHeight(double in)		{fMH = in;}//the peak height in mV-------------------------!!!!!!!!!!!
	//double			GetMaxHeight() const		{return fMH;}//the peak height in mV-------------------------!!!!!!!!!!!

	void			SetFraction(double in)		{fF = in;}
	double			GetFraction() const			{return fF;}

	void			SetPolarity(long in)		{fPolarity = in;}
	long			GetPolarity() const			{return fPolarity;}

	const dvec	   &GetMPuls() const			{return fMPuls;}
	int				GetNbrOfMPulsPoints()const	{return fMPuls.size();}

	//void			SetMPulsSlope(double in)	{fMSlope = in;}
	const double	GetMPulsSlope() const		{return fMSlope;}

	const bool		IsOnlyVoltage()const		{return fVoltage;}
	
	void			AppendName(const char *in)	{fName=in;}
	const char	   *GetName()const				{return fName.Data();}

//private:
//	void			AddMPPoint(double in)		{fMPuls.push_back(in);}

private:
	long			fLow;						//the lower timestamp value
	long			fHigh;						//the upper timestamp value
	long			fChNbr;						//the ChannelNbr this Section belongs to
	long			fSecNbr;					//the numer this sections has withing the Channel
	long			fD;							//the delay value of the cfd
	double			fW;							//the walk value of the cfd
	double			fF;							//the fraction value of the cfd
	double			fT;							//the threshold value of the cfd in mV
	//double			fMH;						//the peak height in mV-------------------------!!!!!!!!!!!
	long			fPolarity;					//the Polarity the Signal have
	double			fMSlope;					//the slope of the Mean Signal
	bool			fVoltage;					//flag that shows that this channel section is only for recording Voltages
	dvec			fMPuls;						//the Mean Signalform
	TString			fName;						//the name of the layer this channelsection belong to

	ClassDef(MyChannelSection,1)				//Defines a Section of a Channel and its Properties
};


#endif