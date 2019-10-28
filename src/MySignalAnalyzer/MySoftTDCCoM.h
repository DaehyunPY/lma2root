#ifndef __MySoftTDCCoM_h__
#define __MySoftTDCCoM_h__

#include "MySoftTDC.h"

//this is called in case it is a 8 Bit Instrument
class MySoftTDCCoM8Bit : public MySoftTDC {
 public:
  void FindPeaksIn(const MyOriginalEvent &, MySignalAnalyzedEvent &, bool);
};

//this is called in case it is a 10 Bit Instrument
class MySoftTDCCoM16Bit : public MySoftTDC {
 public:
  void FindPeaksIn(const MyOriginalEvent &, MySignalAnalyzedEvent &, bool);
};

#endif