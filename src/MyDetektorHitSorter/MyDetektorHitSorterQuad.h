#ifndef __MyDetektorHitSorterQuad_H_
#define __MyDetektorHitSorterQuad_H_

#include "MyDetektorHitSorter.h"

class MyDetektorInfo;

//_______________________________________________________________________MyDetektorHitSorter________________________________________________________________________________________
class MyDetektorHitSorterQuad : public MyDetektorHitSorterBase {
 public:
  MyDetektorHitSorterQuad(const MyDetektorInfo &, MyHistos &, int HiOff);
  virtual ~MyDetektorHitSorterQuad() {}

 public:
  virtual void Sort(MySignalAnalyzedEvent &, MyDetektor &, MyHistos &) = 0;
  virtual void WriteCalibData(const MyDetektorInfo &) = 0;

 protected:
  void FillHistosBeforeShift(const MyDetektor &, MyHistos &);
  void FillDeadTimeHistos(MyHistos &);
  void ExtractTimes(MySignalAnalyzedEvent &, const MyDetektor &);
  void FillRatioHistos(const MyDetektor &, MyHistos &);
};
#endif