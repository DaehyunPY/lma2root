#ifndef __helperfunctions_h__
#define __helperfunctions_h__

#include <iostream>

#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannel.h"
#include "../MyEvent/MyOriginalEvent/MyPuls/MyPuls.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannel.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.h"
#include "../MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h"

//_________________________________helper function that does a linear Regression_____________________________________________________
void linearRegression(int nbrPoints, const double x[], const double y[], double &m, double &c);

//______________________________fwhm________________________________________________________________________________________
template<typename T>
void fwhm(const MyOriginalEvent &oe, const MyOriginalChannel &oc, MyPeak &p) {
  //get information from the event and channel//
  const MyPuls &Puls = oc.GetPuls(p.GetParentPulsNbr());

  const long baseline = oc.GetBaseline();
  const T *Data = static_cast<const T *>(oc.GetDataPointerForPuls(Puls));
  const int pLength = Puls.GetLength();

  //--get peak fwhm--//
  long fwhm_l = 0;
  long fwhm_r = 0;
  const double HalfMax = 0.5 * p.GetMaximum();

  ////--go from middle to left until 0.5*height find first point that is above 0.5 height--//
  for (int i = p.GetMaxPos(); i >= 0; --i) {
    if (abs(Data[i] - baseline) < HalfMax) {
      fwhm_l = i + 1;
      break;
    }
  }

  //--go from middle to right until 0.5*height (find last point that is still above 0.5 Height--//
  for (int i = p.GetMaxPos(); i < pLength; ++i) {
    if (abs(Data[i] - baseline) < HalfMax) {
      fwhm_r = i - 1;
      break;
    }
  }

  //--if we found a right side and a left side, then--//
  //--compute the fwhm with a linear interpolation--//
  //--between the points that are left and right from--//
  //--where the fwhm is, else return here--//
  if (!fwhm_r || !fwhm_l)
    return;

  double lx[4];
  double ly[4];
  lx[0] = fwhm_l - 2;
  ly[0] = abs(Data[fwhm_l - 2] - baseline);
  lx[1] = fwhm_l - 1;
  ly[1] = abs(Data[fwhm_l - 1] - baseline);
  lx[2] = fwhm_l - 0;
  ly[2] = abs(Data[fwhm_l - 0] - baseline);
  lx[3] = fwhm_l + 1;
  ly[3] = abs(Data[fwhm_l + 1] - baseline);

  double rx[4];
  double ry[4];
  rx[0] = fwhm_r - 1;
  ry[0] = abs(Data[fwhm_r - 1] - baseline);
  rx[1] = fwhm_r - 0;
  ry[1] = abs(Data[fwhm_r - 0] - baseline);
  rx[2] = fwhm_r + 1;
  ry[2] = abs(Data[fwhm_r + 1] - baseline);
  rx[3] = fwhm_r + 2;
  ry[3] = abs(Data[fwhm_r + 2] - baseline);

  double mLeft, cLeft, mRight, cRight;
  linearRegression(4, lx, ly, mLeft, cLeft);
  linearRegression(4, rx, ry, mRight, cRight);

  //y = m*x+c => x = (y-c)/m;
  const double fwhm_L = (HalfMax - cLeft) / mLeft;
  const double fwhm_R = (HalfMax - cRight) / mRight;

  const double fwhm = fwhm_R - fwhm_L;
  //--set all found parameters--//
  p.SetFWHM(fwhm);
  p.SetWidth(p.GetStopPos() - p.GetStartPos());
  p.SetPosHalfLeft(fwhm_L);
}
//_______________________________________________________________________________________________________________________________
template<typename T>
void extractVoltage(const MyOriginalChannel &oc,
                    const MyPuls &p,
                    const MyChannelSection &cs,
                    MySignalAnalyzedChannel &sac) {
  double volt = 0;
  int count = 0;
  const double gain = oc.GetVertGain();
  const double offset = oc.GetOffset();
  const T *d = static_cast<const T *>(oc.GetDataPointerForPuls(p));

  for (int j = 10; j < p.GetLength() - 10; ++j) {
    volt += (d[j] * gain) - offset;
    ++count;
  }
  volt /= count;

  sac.AddVoltage(volt, cs.GetChannelSectionNbr());
}

//__________________________________________Center of Mass_______________________________________
template<typename T>
void CoM(const MyOriginalEvent &oe, const MyOriginalChannel &oc, MyPeak &p) {
  //get informations from the event and the channel//
  const MyPuls &Puls = oc.GetPuls(p.GetParentPulsNbr());

  const T *Data = static_cast<const T *>(oc.GetDataPointerForPuls(Puls));
  const long baseline = oc.GetBaseline();
  const long threshold = oc.GetNoise();
  const long timestamp = Puls.GetIndexToFirstPointOfOriginalWaveform();
  const double horpos = oe.GetHorpos() * 1.e9;
  const double sampleInterval = oe.GetSampleInterval() * 1e9;

  //--this function goes through the puls from start to stop and finds the center of mass--//
  double integral = 0;
  double wichtung = 0;
  const int start = p.GetStartPos();
  const int stop = p.GetStopPos();

  for (int i = start; i <= stop; ++i) {
    integral += (abs(Data[i] - baseline) - threshold);            //calc integral
    wichtung += ((abs(Data[i] - baseline) - threshold) * i);        //calc weight
  }
  p.SetIntegral(integral);
  p.SetCoM((wichtung / integral + timestamp + horpos) * sampleInterval);
}

//______________________________________find start and stop of pulse___________________________________________
template<typename T>
void startstop(const MyOriginalEvent &oe, const MyOriginalChannel &oc, MyPeak &p) {
  //--this function will find the start and the stop of the peak--//
  const MyPuls &Puls = oc.GetPuls(p.GetParentPulsNbr());

  const T *Data = static_cast<const T *>(oc.GetDataPointerForPuls(Puls));
  const long baseline = oc.GetBaseline();
  const long threshold = oc.GetNoise();
  const long timestamp = Puls.GetIndexToFirstPointOfOriginalWaveform();
  const double horpos = oe.GetHorpos() * 1.e9;
  const long pulslength = Puls.GetLength();
  const long center = static_cast<long>(p.GetTime() / (oe.GetSampleInterval() * 1e9) - timestamp - horpos);


  //go left from center until either i == 0, or the datapoint is inside the noise
  //or we go from the previous one (i+1) to the actual one (i) through the baseline
  int i = 0;
  for (i = center; i >= 0; --i)
    if ((abs(Data[i] - baseline) < threshold) || (((Data[i] - baseline) * (Data[i + 1] - baseline)) < 0))
      break;
  int start = i;

  //go right form center until either i < pulslength, or the datapoint is inside the noise
  //or we go from the previous one (i-1) to the actual one (i) through the baseline
  for (i = center; i < pulslength; ++i)
    if ((abs(Data[i] - baseline) < threshold) || (((Data[i] - baseline) * (Data[i - 1] - baseline)) < 0))
      break;
  int stop = i;

  p.SetStartPos(start);
  p.SetStopPos(stop);
}

//___________________________________find Maximum of puls and calcs the height_____________________________________________
template<typename T>
void maximum(const MyOriginalEvent &oe, const MyOriginalChannel &oc, MyPeak &p) {
  //--this function will find the maximum of the peak and its position--//
  const MyPuls &Puls = oc.GetPuls(p.GetParentPulsNbr());

  const T *Data = static_cast<const T *>(oc.GetDataPointerForPuls(Puls));
  const long baseline = oc.GetBaseline();
  const int start = p.GetStartPos();
  const int stop = p.GetStopPos();
  const double verticalGain = oc.GetVertGain();
  long maximum = 0;
  int maxpos = 0;

  for (int i = start; i <= stop; ++i) {
    if (abs(Data[i] - baseline) > maximum) {
      maximum = abs(Data[i] - baseline);
      maxpos = i;
    }
  }
  p.SetMaxPos(maxpos);
  p.SetMaximum(maximum);
  p.SetHeight((double) maximum * verticalGain);        //this will set the height in mV
}

//_________________________________helper function that does a linear Regression_____________________________________________________
void gewichtetlinearRegression(const int nbrPoints,
                               const double x[],
                               const double y[],
                               const double correctX,
                               double &m,
                               double &c);
//_________________________________create Newton Polynomial__________________________________________________________________________
void createNewtonPolynomial(const double *x, const double *y, double *coeff);
//_________________________________evaluate Newton Polynomial________________________________________________________________________
double evalNewtonPolynomial(const double *x, const double *coeff, double X);
//_________________________________Achims Numerical Approximation____________________________________________________________________
double findXForGivenY(const double *x, const double *coeff, double Y, double Start);
//_________________________________gib mir zurk____________________________________________________________________________________
double gmz(const double i, const double x0, const dvec MPuls);
#endif
