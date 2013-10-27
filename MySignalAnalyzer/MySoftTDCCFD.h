#ifndef __MySoftTDCCFD_h__
#define __MySoftTDCCFD_h__

#include "MySoftTDC.h"

//this is called in case it is a 8 Bit Instrument
class MySoftTDCCFD8Bit : public MySoftTDC
{
public:
	void FindPeaksIn(const MyOriginalEvent&, MySignalAnalyzedEvent&);
};

//this is called in case it is a 10 Bit Instrument
class MySoftTDCCFD16Bit : public MySoftTDC
{
public:
	void FindPeaksIn(const MyOriginalEvent&, MySignalAnalyzedEvent&);
};

#endif