#ifndef __MySignalAnalyzer_h__
#define __MySignalAnalyzer_h__

#include "MySoftTDC.h"

class MyOriginalEventInfo;
class MyOriginalEvent;
class MySignalAnalyzedEventInfo;
class MySignalAnalyzedEvent;

class MySignalAnalyzer
{
public:
	MySignalAnalyzer():fStdc(0),fNbrBytes(0),fAnalyze(true),fMethod(-1)	{}
	~MySignalAnalyzer();

public:
	enum ESignalAnalyzeMethods{kCoM=0, kCfd, kDoNothing};
	void Init(const MyOriginalEventInfo&, const MySignalAnalyzedEventInfo&);
	void FindPeaksIn(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae)const
	{
		fStdc->FindPeaksIn(oe,sae);
	}

private:
	MySoftTDC	*fStdc;
	int			 fNbrBytes;
	bool		 fAnalyze;
	int			 fMethod;
};
#endif