#include <iostream>
#include "MySignalAnalyzer.h"

#include "MySoftTDCCFD.h"
#include "MySoftTDCCoM.h"

#include "../MyEvent/MyOriginalEvent/MyOriginalEventInfo.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEventInfo.h"

//_________________________________________________________________________________
MySignalAnalyzer::~MySignalAnalyzer() {
  //std::cout << "delete signalanalyzer"<<std::endl;
  delete fStdc;
  //std::cout << "done"<<std::endl;
}

//_________________________________________________________________________________
void MySignalAnalyzer::Init(const MyOriginalEventInfo &oei, const MySignalAnalyzedEventInfo &saei) {
  //check wether nbrbytes have changed
  //or method has changed//
  if ((oei.GetNbrBytes() != fNbrBytes) || (saei.GetUsedMethod() != fMethod)) {
    delete fStdc;
    if (saei.GetUsedMethod() == kCoM)
      if (oei.GetNbrBytes() == 1)
        fStdc = new MySoftTDCCoM8Bit();
      else if (oei.GetNbrBytes() == 2)
        fStdc = new MySoftTDCCoM16Bit();
      else {
        std::cout << "No Analysis Method for Pulses with " << oei.GetNbrBytes() * 8 << " Bit is available" << std::endl;
        exit(0);
      }
    else if (saei.GetUsedMethod() == kCfd)
      if (oei.GetNbrBytes() == 1)
        fStdc = new MySoftTDCCFD8Bit();
      else if (oei.GetNbrBytes() == 2)
        fStdc = new MySoftTDCCFD16Bit();
      else {
        std::cout << "No Analysis Method for Pulses with " << oei.GetNbrBytes() * 8 << " Bit is available" << std::endl;
        exit(0);
      }
    else if (saei.GetUsedMethod() == kDoNothing)
      fStdc = new MySoftTDCDoNothing();
    else {
      std::cout << "Analysis Method " << saei.GetUsedMethod() << " not available" << std::endl;
      exit(0);
    }

    fNbrBytes = oei.GetNbrBytes();
    fMethod = saei.GetUsedMethod();
  }
}