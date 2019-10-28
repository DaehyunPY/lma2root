#include "MySoftTDCCFD.h"
#include "helperfunctionsforSTDC.h"

#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannel.h"
#include "../MyEvent/MyOriginalEvent/MyPuls/MyPuls.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannel.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.h"
#include "../MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h"

//________________________________Implematation of Constant Fraction Method______________________________________________________
//________________this will be a thread that is waiting for Pulses to be added to a queue________________________________________
template<typename T>
void *cfd(void *arg) {
  //some local variables that we need//
  double *cData = 0;    //this is the array where the temppuls will be copied to
  size_t maxsize = 0;    //the size of of the array
  MySoftTDCMessage stdcm;            //a container for the events to come

//#ifdef _USETHREADS
  //get the queue and semaphore from the argument//
  stdcQueue *q = static_cast<MyThreadArgument *>(arg)->fQueue;
  MySemaphore *sem = static_cast<MyThreadArgument *>(arg)->fSemaphore;

  while (1) {
    //wait until a Puls is in the queue
    //if there is a Puls in the queue retrieve it
    q->wait_and_pop(stdcm);

    //tell the semaphore that we are now working
    sem->Decrement();
//#endif _USETHREADS

    //now extract information from the Channel and the Event
    const MyOriginalEvent &oe = *stdcm.GetOriginalEvent();
    const MyOriginalChannel &oc = *stdcm.GetOriginalChannel();
    MySignalAnalyzedChannel &sac = *stdcm.GetSignalAnalyzedChannel();

    const long baseline = oc.GetBaseline();
    const double horpos = oe.GetHorpos() * 1.e9;
    const double vertGain = oc.GetVertGain();
    const double sampleInterval = oe.GetSampleInterval() * 1e9;    //convert the s to ns

    //go through all pulses of channel//
    for (int pu = 0; pu < oc.GetNbrPulses(); ++pu) {
      const MyPuls &Puls = oc.GetPuls(pu);
      const long idxToFiPoint = Puls.GetIndexToFirstPoint();
      const T *Data = static_cast<T *>(Puls.GetData());
      const size_t pLength = Puls.GetLength();
      const MyChannelSection *cs = sac.GetPointerToChannelSectionForIndex(idxToFiPoint);
      if (!cs) continue;

      //check wether this puls only records a voltage//
      if (cs->IsOnlyVoltage()) {
        //extract the  voltage and end here//
        extractVoltage<T>(oc, Puls, *cs, sac);
        continue;
      }

      //get the meanpuls stuff//
      const double MPulsSlope = cs->GetMPulsSlope();

      //--get the right cfd settings--//
      const long delay = static_cast<long>(cs->GetDelay() / sampleInterval);    //ns -> sampleinterval units
      const double walk = cs->GetWalk() / vertGain;                                //mV -> ADC Bytes
      const double threshold = cs->GetThreshold() / vertGain;                        //mV -> ADC Bytes
      const double fraction = cs->GetFraction();


      //copy the waveform to the new array//
      //if the copy puls is not big enough to hold the original puls then resize container//
      if (pLength > maxsize) {
        delete[] cData;
        cData = new double[pLength + 100];
        maxsize = pLength + 100;
      }
      double *tmp1 = cData;    //we can't use memcopy here, because we need the typecast
      const T *tmp2 = Data;
      for (int i = 0; i < pLength; ++i)
        *tmp1++ = *tmp2++;

      //--go through the puls--//
      for (int i = delay; i < pLength - 2; ++i) {
        const double fx = cData[i] - static_cast<double>(baseline);        //the original Point at i
        const double fxd = cData[i - delay] - static_cast<double>(baseline);    //the delayed Point	at i
        const double fsx = -fx * fraction + fxd;                                //the calculated CFPoint at i

        const double fx_1 = cData[i + 1] - static_cast<double>(baseline);        //original Point at i+1
        const double fxd_1 = cData[i + 1 - delay] - static_cast<double>(baseline);    //delayed Point at i+1
        const double fsx_1 = -fx_1 * fraction + fxd_1;                            //calculated CFPoint at i+1

        //check wether the criteria for a Peak are fullfilled
        if (((fsx - walk) * (fsx_1 - walk)) <= 0)    //one point above one below the walk
          if (TMath::Abs(fx) > threshold)                //original point above the threshold
          {
            //--it could be that the first criteria is 0 because	--//
            //--one of the Constant Fraction Signal Points or both	--//
            //--are exactly where the walk is						--//
            if (TMath::Abs(fsx - fsx_1) < 1e-8)    //both points are on the walk
            {
              //--go to next loop until at least one is over or under the walk--//
              continue;
            } else if ((fsx - walk) == 0)        //only first is on walk
            {
              //--Only the fist is on the walk, this is what we want--//
              //--so:do nothing--//
            } else if ((fsx_1 - walk) == 0)        //only second is on walk
            {
              //--we want that the first point will be on the walk,--//
              //--so in the next loop this point will be the first.--//
              continue;
            }
            //does the peak have the right polarity?//
            //if two pulses are close together then the cfsignal goes through the walk//
            //three times, where only two crossings are good. So we need to check for//
            //the one where it is not good//
            if (fsx > fsx_1)        //neg polarity
              if (cData[i] > baseline)    //but pos Puls .. skip
                continue;
            if (fsx < fsx_1)        //pos polarity
              if (cData[i] < baseline)    //but neg Puls .. skip
                continue;


            //--later we need two more points, create them here--//
            const double fx_m1 = cData[i - 1] - static_cast<double>(baseline);        //the original Point at i-1
            const double fxd_m1 = cData[i - 1 - delay] - static_cast<double>(baseline);    //the delayed Point	at i-1
            const double fsx_m1 = -fx_m1 * fraction + fxd_m1;                            //the calculated CFPoint at i-1

            const double fx_2 = cData[i + 2] - static_cast<double>(baseline);            //original Point at i+2
            const double fxd_2 = cData[i + 2 - delay] - static_cast<double>(baseline);    //delayed Point at i+2
            const double fsx_2 = -fx_2 * fraction + fxd_2;                            //calculated CFPoint at i+2


            //--find x with a linear interpolation between the two points--//
            const double m = fsx_1 - fsx;                    //(fsx-fsx_1)/(i-(i+1));
            const double xLin = i + (walk - fsx) / m;        //PSF fx = (x - i)*m + cfs[i]

            //--make a linear regression to find the slope of the leading edge--//
            double mslope, cslope;
            const double xslope[3] = {i - delay, i + 1 - delay, i + 2 - delay};
            const double yslope[3] = {fxd, fxd_1, fxd_2};
            linearRegression(3, xslope, yslope, mslope, cslope);

            //--find x with a cubic polynomial interpolation between four points--//
            //--do this with the Newtons interpolation Polynomial--//
            const double x[4] = {i - 1, i, i + 1, i + 2};                //x vector
            const double y[4] = {fsx_m1, fsx, fsx_1, fsx_2};        //y vector
            double coeff[4] = {0, 0, 0, 0};                //Newton coeff vector
            createNewtonPolynomial(x, y, coeff);

            //--numericaly solve the Newton Polynomial--//
            //--give the lineare approach for x as Start Value--//
            const double xPoly = findXForGivenY(x, coeff, walk, xLin);
            const double pos = xPoly + static_cast<double>(idxToFiPoint) + horpos;

            //--add a new peak--//
            MyPeak &Peak = sac.AddPeak(pu);

            //add the info//
            Peak.SetCFD(pos * sampleInterval);
            Peak.SetTime(pos * sampleInterval);
            if (fsx > fsx_1) Peak.SetPolarity(kNegative);        //Peak has Neg Pol
            if (fsx < fsx_1) Peak.SetPolarity(kPositive);        //Peak has Pos Pol
            if (TMath::Abs(fsx - fsx_1) < 1e-8) Peak.SetPolarity(kBad);//Peak has Bad Pol

            //slope of peak//
            Peak.SetSlope(mslope);

            //--start and stop of the puls--//
            startstop<T>(oe, oc, Peak);

            //--height of peak--//
            maximum<T>(oe, oc, Peak);

            //--width & fwhm of peak--//
            fwhm<T>(oe, oc, Peak);

            //--the com and integral--//
            CoM<T>(oe, oc, Peak);

            //if there is a MeanPuls given, substract it from this puls//
            if (cs->GetNbrOfMPulsPoints()) {
              //subtract this peak from the puls
              for (int j = 0; j < pLength; ++j)
                cData[j] -= (gmz(static_cast<double>(j), xPoly, cs->GetMPuls()) * (mslope / MPulsSlope));
              //start from begining
              i = delay - 1;
            }
          }
      }
    }
//#ifdef _USETHREADS
    //tell the semaphore that we are done working
    sem->Increment();
  }
//#endif
  delete[] cData;
  return 0;
}

//########################## 8 Bit Version ###########################################################################
//______________________________________________________________________________________________________________________
MySoftTDCCFD8Bit::MySoftTDCCFD8Bit(int NbrOfThreads) : MySoftTDC(NbrOfThreads) {
  for (int i = 0; i < fNThreads; ++i) {
    fThreads[i] = new TThread(Form("t%d", i), cfd<char>, (void *) &fArg[i]);
    fThreads[i]->Run();
  }
}

//########################## 16 Bit Version ###########################################################################
//______________________________________________________________________________________________________________________
MySoftTDCCFD16Bit::MySoftTDCCFD16Bit(int NbrOfThreads) : MySoftTDC(NbrOfThreads) {
  for (int i = 0; i < fNThreads; ++i) {
    fThreads[i] = new TThread(Form("t%d", i), cfd<short>, (void *) &fArg[i]);
    fThreads[i]->Run();
  }
}