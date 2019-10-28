#ifndef __MySoftTDCCoM_h__
#define __MySoftTDCCoM_h__

#include "MySoftTDC.h"

//this is called in case it is a 8 Bit Instrument
class MySoftTDCCoM8Bit : public MySoftTDC {
 public:
  MySoftTDCCoM8Bit(int NbrOfThreads = 4);
  ~MySoftTDCCoM8Bit() {}

};

//this is called in case it is a 10 Bit Instrument
class MySoftTDCCoM16Bit : public MySoftTDC {
 public:
  MySoftTDCCoM16Bit(int NbrOfThreads = 4);
  ~MySoftTDCCoM16Bit() {}
};

#endif