#ifndef __MySoftTDC_h__
#define __MySoftTDC_h__
#include <iostream>
#include <TThread.h>
#include "../ThreadStuff/MyQueue.h"
#include "../ThreadStuff/MySemaphore.h"

class MyOriginalEvent;
class MyOriginalChannel;
class MySignalAnalyzedEvent;
class MySignalAnalyzedChannel;

//a class that combines the two channels for analysis
class MySoftTDCMessage {
 public:
  MySoftTDCMessage() :
      fOE(0), fOC(0), fSAC(0) {}
  MySoftTDCMessage(const MyOriginalEvent *oe, const MyOriginalChannel *oc, MySignalAnalyzedChannel *sac) :
      fOE(oe), fOC(oc), fSAC(sac) {}
  const MyOriginalEvent *GetOriginalEvent() { return fOE; }
  const MyOriginalChannel *GetOriginalChannel() { return fOC; }
  MySignalAnalyzedChannel *GetSignalAnalyzedChannel() { return fSAC; }
 private:
  const MyOriginalEvent *fOE;
  const MyOriginalChannel *fOC;
  MySignalAnalyzedChannel *fSAC;
};
//a class that contains the Arguments passed to the threads
typedef MyQueue<MySoftTDCMessage> stdcQueue;
class MyThreadArgument {
 public:
  stdcQueue *fQueue;
  MySemaphore *fSemaphore;
};

//this class is placeholder for two other classes wich will be called 
//according to how many bits the instrument has
class MySoftTDC {
 public:
  MySoftTDC() : fNThreads(0), fThreads(0), fArg(0) {}
  MySoftTDC(const int NbrOfThreads);
  virtual ~MySoftTDC();

 public:
  virtual void FindPeaksIn(const MyOriginalEvent &, MySignalAnalyzedEvent &);

 protected:
  stdcQueue fQ;            //the queue to talk to the threads
  MySemaphore fSem;            //the semaphore to signal the producer
  MyThreadArgument *fArg;            //the arguments for the threads
  TThread **fThreads;        //the Threads
  int fNThreads;        //the nbr of Threads
};

//this class does nothing 
class MySoftTDCDoNothing : public MySoftTDC {
 public:
  void FindPeaksIn(const MyOriginalEvent &, MySignalAnalyzedEvent &) {}
};

#endif