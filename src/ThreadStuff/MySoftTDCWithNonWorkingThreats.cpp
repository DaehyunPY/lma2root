#include <iostream>

#include "MySoftTDC.h"

#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"

//______________________________________________________________________________________________________________________
MySoftTDC::MySoftTDC(const int NbrOfThreads) : fNThreads(NbrOfThreads) {
  //create the threads and arguments
  fThreads = new TThread *[fNThreads];
  fArg = new MyThreadArgument[fNThreads];
  for (int i = 0; i < fNThreads; ++i) {
    fArg[i].fQueue = &fQ;
    fArg[i].fSemaphore = &fSem;
  }
}

//______________________________________________________________________________________________________________________
MySoftTDC::~MySoftTDC() {
  //stop all threads with killerpulses//
  for (int i = 0; i < fNThreads; ++i)
    fThreads[i]->Kill();

  //wait until threads are finished
  for (int i = 0; i < fNThreads; ++i)
    fThreads[i]->Join();

  //delete threads and args
  for (int i = 0; i < fNThreads; ++i)
    delete fThreads[i];
  delete[] fThreads;
  delete[] fArg;
}

//______________________________________________________________________________________________________________________
void MySoftTDC::FindPeaksIn(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae) {
  //put all pulses of this event to the queue
  for (int i = 0; i < oe.GetNbrOfChannels(); ++i)
    if (oe.GetUsedChannels() & 0x1 << i)
      fQ.push(MySoftTDCMessage(&oe, &oe.GetChannel(i), &sae.GetChannel(i)));

  //wait until queue has finished working on all pulses
  fSem.Wait();
  while (!fQ.empty())
    fSem.Wait();
}