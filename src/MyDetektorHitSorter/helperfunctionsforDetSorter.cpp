#include <TH1.h>
#include <iostream>
#include <iomanip>

#include "helperfunctionsforDetSorter.h"

#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannel.h"
#include "../MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyLayerProperty.h"

//_________________________________extract the times from the a Channel__________________________________________________________________________________________________________________________
void extractTimes(MySignalAnalyzedEvent &sae,
                  const MyLayerProperty &lp,
                  std::vector<MyPeak *> &vec,
                  std::vector<double> &dvec) {
  //clear vectors// 
  vec.clear();
  dvec.clear();

  //get the Channel//
  MySignalAnalyzedChannel &sac = sae.GetChannel(lp.GetChannelNbr());

  //--fill the vector with peaks--//
  for (size_t i = 0; i < sac.GetNbrPeaks(); ++i) {
    MyPeak &p = sac.GetPeak(i);
    bool matchesAllProperties = false;
    for (size_t j = 0; j < lp.GetNbrOfTimeRanges(); ++j) {
      if (p.GetTime() >= lp.GetTimeRangeLow(j))    //if time value is bigger than from
        if (p.GetTime() < lp.GetTimeRangeHigh(j))    //if time value is lower than to
          if (p.GetPolarity() == lp.GetPolarity())        //and peak has right polarity
          {
            matchesAllProperties = true;
            break;
          }
    }

    if (matchesAllProperties) {
      vec.push_back(&p);                            //add peak to vector
      dvec.push_back(p.GetTime());                //add time of the peak to the vector
    }
  }
}


