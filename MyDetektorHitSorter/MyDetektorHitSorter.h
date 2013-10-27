#ifndef __MyDetektorHitSorter_H_
#define __MyDetektorHitSorter_H_

#include <vector>

class MyPeak;
class MySignalAnalyzedEvent;
class MySortedEventInfo;
class MySortedEvent;
class MyHistos;
class MyDetektor;
class MyDetektorInfo;

//_______________________________________________________________________MyDetektorHitSorter________________________________________________________________________________________
typedef std::vector<MyPeak*> pVec;
typedef std::vector<double> dVec;

class MyDetektorHitSorterBase
{
public:
	MyDetektorHitSorterBase(int HiOff):fHiOff(HiOff)	{}
	virtual ~MyDetektorHitSorterBase()					{}

public:
	virtual void Sort(MySignalAnalyzedEvent&, MyDetektor&, MyHistos&)=0;
	virtual void WriteCalibData(const MyDetektorInfo&)=0;

protected:
	enum EHistoIndex
	{
		kSumU=0,
		kSumV,
		kSumW,
		kSumVsURaw,
		kSumVsVRaw,
		kSumVsWRaw,
		kSumVsUShift,
		kSumVsVShift,
		kSumVsWShift,
		kSumVsUShiftCorr,
		kSumVsVShiftCorr,
		kSumVsWShiftCorr,
		kDetRaw_ns,
		kDetShi_ns,
		kDetUVRaw_ns,
		kDetUVShi_ns,
		kDetUWRaw_ns,
		kDetUWShi_ns,
		kDetVWRaw_ns,
		kDetVWShi_ns,
		kDetRaw_mm,
		kDetShi_mm,
		kDetUVRaw_mm,
		kDetUVShi_mm,
		kDetUWRaw_mm,
		kDetUWShi_mm,
		kDetVWRaw_mm,
		kDetVWShi_mm,
		kMcpDeadTime,
		kU1DeadTime,
		kU2DeadTime,
		kV1DeadTime,
		kV2DeadTime,
		kW1DeadTime,
		kW2DeadTime,
		kNbrRecHits,
		kNbrMCPHits,
		kNbrU1Hits,
		kNbrU2Hits,
		kNbrV1Hits,
		kNbrV2Hits,
		kNbrW1Hits,
		kNbrW2Hits,
		kURatio,
		kVRatio,
		kWRatio,
		kU1MCPRatio,
		kU2MCPRatio,
		kV1MCPRatio,
		kV2MCPRatio,
		kW1MCPRatio,
		kW2MCPRatio,
		kMCPRecRatio,
		kU1RecRatio,
		kU2RecRatio,
		kV1RecRatio,
		kV2RecRatio,
		kW1RecRatio,
		kW2RecRatio,
		kUsedMethod,
		kTime,
		kPosXVsTime,
		kPosYVsTime,
		kDetAll,
		kDetRisky,
		kDetNonRisky,
		kNonLinearityMap
	};


protected:
	int			 fHiOff;				//Offset for Histograms Index in the HistogramManager that are in MyHistos Class

	pVec		 u1vec;					//Vector conatining all Peak with u1
	dVec		 u1d;					//Vector of the times of all Peaks
	pVec		 u2vec;					//Vector conatining all Peak with u2
	dVec		 u2d;					//Vector of the times of all Peaks
	pVec		 v1vec;					//Vector conatining all Peak with v1
	dVec		 v1d;					//Vector of the times of all Peaks
	pVec		 v2vec;					//Vector conatining all Peak with v2
	dVec		 v2d;					//Vector of the times of all Peaks
	pVec		 w1vec;					//Vector conatining all Peak with w1
	dVec		 w1d;					//Vector of the times of all Peaks
	pVec		 w2vec;					//Vector conatining all Peak with w2
	dVec		 w2d;					//Vector of the times of all Peaks
	pVec		 mcpvec;				//Vector conatining all Peak with mcp
	dVec		 mcpd;					//Vector of the times of all Peaks
};

//the actual worker
typedef std::vector<MyDetektorHitSorterBase*> dhsVec;
class MyDetektorHitSorter
{
public:
	MyDetektorHitSorter()		{}
	~MyDetektorHitSorter();

public:
	enum ESorterMethod {kSimple=0, kAchim, kDoNothing};
	void Init(const MySortedEventInfo&, MyHistos&);
	void Sort(MySignalAnalyzedEvent&, MySortedEvent&, MyHistos&);
	void WriteCalibData(const MySortedEventInfo&);

private:
	dhsVec				fDhs;
	std::vector<int>	fSM;
};


#endif