#include <TSystem.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TColor.h>
#include <TStyle.h>
#include <TGClient.h>
#include <iostream>
// #include <conio.h>

#include "MyEventViewer.h"
#include "../MySignalAnalyzer/baselineCorrection.h"

#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalEventInfo.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalChannel/MyOriginalChannel.h"
#include "../MyEvent/MyOriginalEvent/MyPuls/MyPuls.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MySignalAnalyzedChannel.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedChannel/MyChannelSection.h"
#include "../MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h"
#include "../MySignalAnalyzer/MySignalAnalyzer.h"

//__________________helper function to create a Canvas_______________________________________________________________
TCanvas *Canv(int hor, int ver) {
  //find out which size is the max, so that all subpads are still rectangluar but not bigger than 367//
  double h = gClient->GetDisplayHeight();
  double w = gClient->GetDisplayWidth();
  h -= 26;
  w -= 10;
  double hm = h / static_cast<double>(ver);
  double wm = w / static_cast<double>(hor);
  double maxsize = (wm > hm) ? hm : wm;
  if (maxsize > 500) maxsize = 500;
  double cw = gStyle->GetScreenFactor();    //root multiplies the height and the width by this number
  //so in order to get the wanted size we need to divide the values by this number first

  //setup canvas//
  TCanvas *MyC =
      new TCanvas("MyC", "MyCanvas", -1, 1, static_cast<int>(maxsize * hor / cw), static_cast<int>(maxsize * ver / cw));
  MyC->Divide(hor, ver, static_cast<float>(0.0001), static_cast<float>(0.0001));
  MyC->ToggleEventStatus();
  for (int i = 1; i <= hor * ver; ++i) {
    MyC->cd(i);
    gPad->SetBottomMargin(static_cast<float>(0.15));
    gPad->SetRightMargin(static_cast<float>(0.15));
    gPad->SetLeftMargin(static_cast<float>(0.15));
    gPad->SetTopMargin(static_cast<float>(0.15));
  }
  return MyC;
}

//____________________________initialization of the base class___________________________________________________________________________
MyEventViewerBase::MyEventViewerBase() {
  gStyle->SetMarkerStyle(21);
  gStyle->SetMarkerSize(0.5);

  c = 0;
  ChNbr = 0;
  SecNbr = 0;
  drawThresh = true;
  drawWalk = true;
  drawPos = true;
  Pol = kPositive;
  xRange = 150;
  noPulscnter = 0;

  arrow.SetAngle(40);
}

//########################Shows the raw events################################################################
//_________________________________________________________________________________
MyRawEventViewer::MyRawEventViewer(const MyOriginalEventInfo &oei) {
  size_t nbrChannels = oei.GetNbrOfChannels();
  long chlength = oei.GetNbrSamples();
  for (size_t chan = 0; chan < nbrChannels; ++chan) {
    char tmp[256];
    char name[64];
    char tmpbl[256];
    char namebl[64];
    char tmpcorr[256];
    char namecorr[64];
    sprintf(tmp, "channel%i", chan + 1);
    sprintf(name, "Channel %i", chan + 1);
    sprintf(tmpbl, "channelBL%i", chan + 1);
    sprintf(namebl, "ChannelBL %i", chan + 1);
    sprintf(tmpcorr, "channelC%i", chan + 1);
    sprintf(namecorr, "ChannelC %i", chan + 1);
    ChHisto.push_back(new TH1D(tmp, name, 1, 0, 1));
    ChBL.push_back(new TH1D(tmpbl, namebl, 1, 0, 1));
    ChHistoCorr.push_back(new TH1D(tmpcorr, namecorr, 1, 0, 1));

    //check if channel exists//
    if (oei.GetUsedChannels() & (0x1 << chan)) {
      //labeling the axis//
      sprintf(tmp, "t [%0.1f ns]", oei.GetSampleInterval() * 1.e9);
      ChHisto[chan]->SetXTitle(tmp);
      ChHisto[chan]->GetXaxis()->CenterTitle();
      ChHisto[chan]->SetYTitle("U [mV]");
      ChHisto[chan]->GetYaxis()->CenterTitle();
      ChHisto[chan]->SetBit(TH1::kNoStats);

      //--set the histogram to the right size--//
      ChHisto[chan]->Reset();
      ChHisto[chan]->SetBins(chlength, 0 - 0.5, chlength - 0.5);
      ChHisto[chan]->SetMaximum(oei.GetChannelInfo(chan).GetFullScale()/*/2*/);
      ChHisto[chan]->SetMinimum(-oei.GetChannelInfo(chan).GetFullScale() / 2);

      ChBL[chan]->Reset();
      ChBL[chan]->SetBins(chlength, 0 - 0.5, chlength - 0.5);
      ChBL[chan]->SetMaximum(oei.GetChannelInfo(chan).GetFullScale()/*/2*/);
      ChBL[chan]->SetMinimum(-oei.GetChannelInfo(chan).GetFullScale() / 2);

      ChHistoCorr[chan]->Reset();
      ChHistoCorr[chan]->SetBins(chlength, 0 - 0.5, chlength - 0.5);
      ChHistoCorr[chan]->SetMaximum(oei.GetChannelInfo(chan).GetFullScale()/*/2*/);
      ChHistoCorr[chan]->SetMinimum(-oei.GetChannelInfo(chan).GetFullScale() / 2);
    }
  }

  //create a canvas that will display the channels and tell the user what he can do//
  std::cout << std::endl << std::endl << std::endl;
  std::cout << "key assingment:" << std::endl;
  std::cout << "u: toggle Position Indicator" << std::endl;
  std::cout << "p: print whole Canvas as PNG" << std::endl;
  std::cout << "space: continue to next Event" << std::endl;
  std::cout << std::endl;
  c = Canv(4, nbrChannels / 4);
}
//_________________________________________________________________________________
MyRawEventViewer::~MyRawEventViewer() {
  //delete the histograms that we created before//
  for (size_t i = 0; i < ChHisto.size(); ++i) {
    delete ChHisto[i];
    delete ChBL[i];
  }

  delete c;
}
//_________________________________________________________________________________
void MyRawEventViewer::View(const MyOriginalEvent &oe,
                            MySignalAnalyzedEvent &sae,
                            const MySignalAnalyzer &,
                            bool blcorr) {
  //std::cout << "with baseline correction:" << blcorr << std::endl;
  if (oe.GetNbrBytes() == 1)
    if (blcorr) showEventImplBLCorr<char>(oe, sae);
    else showEventImpl<char>(oe, sae);
  if (oe.GetNbrBytes() == 2)
    if (blcorr) showEventImplBLCorr<short>(oe, sae);
    else showEventImpl<short>(oe, sae);
}
//_______________________________________________________________________________________________________
template<typename T>
void MyRawEventViewer::showEventImpl(const MyOriginalEvent &oe, const MySignalAnalyzedEvent &sae) {
  const T *data;
  //how many channels do we have//
  size_t nbrChannels = oe.GetNbrOfChannels();
  long chlength = oe.GetNbrSamples();

  //do this until a new event is chosen//
  while (1) {
    //go through all channels//
    for (size_t chan = 0; chan < nbrChannels; ++chan) {
      //check if channel exists//
      if (oe.GetUsedChannels() & (0x1 << chan)) {
        const MyOriginalChannel &oc = oe.GetChannel(chan);
        const MySignalAnalyzedChannel &sac = sae.GetChannel(chan);
        const double vertGain = oc.GetVertGain();
        const long offset = oc.GetBaseline();
        const double horpos = oe.GetHorpos() * 1e9;

        //--writing the mean value to histo--//
        for (size_t j = 0; j < chlength; ++j)
          ChHisto[chan]->SetBinContent(j + 1, offset * vertGain);

        //--Filling the Puls Histogram--//
        for (size_t i = 0; i < oc.GetNbrPulses(); ++i) {
          const MyPuls &p = oc.GetPuls(i);
          data = static_cast<const T *>(oc.GetDataPointerForPuls(p));
          for (int j = 0; j < p.GetLength(); ++j)
            ChHisto[chan]->SetBinContent(j + 1 + p.GetIndexToFirstPointOfOriginalWaveform(), data[j] * vertGain);
        }
        c->cd(chan + 1);        //change to the right pad
        ChHisto[chan]->Draw("p0c");

        //--draw where the peak was found, code the polarity in color--//
        for (size_t i = 0; i < sac.GetNbrPeaks(); ++i) {
          const MyPeak &p = sac.GetPeak(i);

          //the cfd value//
          if (p.GetPolarity() == kPositive) {
            arrow.SetFillColor(kRed);
            arrow.SetLineColor(kRed);
          }
          if (p.GetPolarity() == kNegative) {
            arrow.SetFillColor(kBlue);
            arrow.SetLineColor(kBlue);
          }

          const MyPuls &pp = oc.GetPuls(p.GetParentPulsNbr());
          data = static_cast<const T *>(oc.GetDataPointerForPuls(pp));
          int timestamp = pp.GetIndexToFirstPointOfOriginalWaveform();
          double time = p.GetTime() / (oe.GetSampleInterval() * 1e9);
          double timeInPuls = time - timestamp - horpos;

          double x1 = time;
          double x2 = time;
          double y1 = offset * vertGain + 5;
          double y2 = data[static_cast<int>(timeInPuls)] * vertGain + 10;

          if (drawPos)arrow.DrawArrow(x1, y1, x2, y2, 0.01, "<|");
        }
      }
    }
    //update canvas
    c->cd(0);
    gPad->Update();
    std::cout << "Tag:" << oe.GetEventID() << std::endl;
    //--wait for keystroke--//
    // while(!_kbhit()) {
    //     gSystem->Sleep(50);
    //     gSystem->ProcessEvents();
    // }
    system("/bin/stty raw");
    auto ch = getchar();
    system("/bin/stty cooked");
    if (ch == 'p') c->Print("Event.png");
    else if (ch == 'u') drawPos = (drawPos) ? false : true;
    else break;
  }
}

//
//___________________________________________Baseline Correction_________________________________________
//_______________________________________________________________________________________________________
template<typename T>
void MyRawEventViewer::showEventImplBLCorr(const MyOriginalEvent &oe, const MySignalAnalyzedEvent &sae) {
  const T *data;
  double *cData = 0;        //this is the array where the temppuls will be copied to
  double *BLData = 0;

  //how many channels do we have//
  size_t nbrChannels = oe.GetNbrOfChannels();
  long chlength = oe.GetNbrSamples();

  //do this until a new event is chosen//
  while (1) {
    //go through all channels//
    for (size_t chan = 0; chan < nbrChannels; ++chan) {
      //check if channel exists//
      if (oe.GetUsedChannels() & (0x1 << chan)) {
        const MyOriginalChannel &oc = oe.GetChannel(chan);
        const MySignalAnalyzedChannel &sac = sae.GetChannel(chan);
        const double vertGain = oc.GetVertGain();
        const long offset = oc.GetBaseline();
        const double horpos = oe.GetHorpos() * 1e9;

        //--writing the mean value to histo--//
        for (size_t j = 0; j < chlength; ++j)
          ChHisto[chan]->SetBinContent(j + 1, offset * vertGain);

        //--Filling the Puls Histogram--//
        for (size_t i = 0; i < oc.GetNbrPulses(); ++i) {
          const MyPuls &p = oc.GetPuls(i);
          data = static_cast<const T *>(oc.GetDataPointerForPuls(p));
          for (int j = 0; j < p.GetLength(); ++j)
            ChHisto[chan]->SetBinContent(j + 1 + p.GetIndexToFirstPointOfOriginalWaveform(), data[j] * vertGain);
        }
        c->cd(chan + 1);        //change to the right pad
        ChHisto[chan]->Draw("p0c");


        //////////////////////////////////////testing/////////////////////////////////////////////////////////////////

        //--writing the mean value to histo--//
        for (size_t j = 0; j < chlength; ++j) {
          ChBL[chan]->SetBinContent(j + 1, offset * vertGain);
          ChHistoCorr[chan]->SetBinContent(j + 1, offset * vertGain);
        }
        //--Filling the Puls Histogram--//
        for (size_t i = 0; i < oc.GetNbrPulses(); ++i) {
          const MyPuls &p = oc.GetPuls(i);
          data = static_cast<const T *>(oc.GetDataPointerForPuls(p));

          delete[] cData;
          delete[] BLData;
          cData = new double[p.GetLength() + 100];
          BLData = new double[p.GetLength() + 100];
          double offsetD = static_cast<double>(offset);
          //copy the puls to the new array//
          double *tmp1 = cData;
          const T *tmp2 = data;
          for (int i = 0; i < p.GetLength(); ++i)
            *tmp1++ = *tmp2++;

          if ((p.GetLength() > 100) && (chan != (8 - 1))) {
            BLCorr(cData, BLData, offsetD, p.GetLength(), 50, 1);

            for (int j = 0; j < p.GetLength(); ++j) {
              ChBL[chan]->SetBinContent(j + 1 + p.GetIndexToFirstPointOfOriginalWaveform(), BLData[j] * vertGain);
              ChHistoCorr[chan]->SetBinContent(j + 1 + p.GetIndexToFirstPointOfOriginalWaveform(),
                                               (cData[j] - BLData[j] + offsetD) * vertGain);
            }
          }

        }
        c->cd(chan + 1);        //change to the right pad
        ChBL[chan]->SetLineColor(kGreen);
        ChBL[chan]->Draw("same");
        ChHistoCorr[chan]->SetLineColor(kMagenta);
        ChHistoCorr[chan]->Draw("same");

        //////////////////////////////////////testing/////////////////////////////////////////////////////////////////


        //--draw where the peak was found, code the polarity in color--//
        for (size_t i = 0; i < sac.GetNbrPeaks(); ++i) {
          const MyPeak &p = sac.GetPeak(i);

          //the cfd value//
          if (p.GetPolarity() == kPositive) {
            arrow.SetFillColor(kRed);
            arrow.SetLineColor(kRed);
          }
          if (p.GetPolarity() == kNegative) {
            arrow.SetFillColor(kBlue);
            arrow.SetLineColor(kBlue);
          }

          const MyPuls &pp = oc.GetPuls(p.GetParentPulsNbr());
          data = static_cast<const T *>(oc.GetDataPointerForPuls(pp));
          int timestamp = pp.GetIndexToFirstPointOfOriginalWaveform();
          double time = p.GetTime() / (oe.GetSampleInterval() * 1e9);
          double timeInPuls = time - timestamp - horpos;

          double x1 = time;
          double x2 = time;
          double y1 = offset * vertGain + 5;
          double y2 = data[static_cast<int>(timeInPuls)] * vertGain + 10;

          if (drawPos)arrow.DrawArrow(x1, y1, x2, y2, 0.01, "<|");
        }
      }
    }
    //update canvas
    c->cd(0);
    gPad->Update();

    //--wait for keystroke--//
    // while(!_kbhit()) {
    //     gSystem->Sleep(50);
    //     gSystem->ProcessEvents();
    // }
    system("/bin/stty raw");
    auto ch = getchar();
    system("/bin/stty cooked");
    if (ch == 'p') c->Print("Event.png");
    else if (ch == 'u') drawPos = (drawPos) ? false : true;
    else break;
  }
}

//########################Helps to find the right CFD Settings################################################################
//_________________________________________________________________________________
MyCFDAdjuster::MyCFDAdjuster() {
  //create Canvas//
  c = Canv(1, 1);

  //output info about the keys//
  std::cout << std::endl << std::endl << std::endl;
  std::cout << "key assingment:" << std::endl;
  std::cout << "a: increase Fraction" << std::endl;
  std::cout << "y: decrease Fraction" << std::endl;
  std::cout << "s: increase Delay" << std::endl;
  std::cout << "x: decrease Delay" << std::endl;
  std::cout << "d: increase Threshold" << std::endl;
  std::cout << "c: decrease Threshold" << std::endl;
  std::cout << "f: increase Walk" << std::endl;
  std::cout << "v: decrease Walk" << std::endl;
  std::cout << "b: increase X-Range" << std::endl;
  std::cout << "g: decrease X-Range" << std::endl;
  std::cout << "q: change Channel" << std::endl;
  std::cout << "w: change ChannelSection" << std::endl;
  std::cout << "u: toggle Position Indicator" << std::endl;
  std::cout << "i: toggle Walk indicator" << std::endl;
  std::cout << "o: toggle Threshold indicator" << std::endl;
  std::cout << "p: toggle Polarity" << std::endl;
  std::cout << "space: continue to next Puls" << std::endl;
  std::cout << std::endl;

  //create the histos//
  pulshist = new TH1D("Puls", "Puls", 150, 0 - 0.5, 150 - 0.5);
  pulshist->SetMarkerStyle(7);
  pulshist->SetXTitle("time [Sample Interval]");
  pulshist->GetXaxis()->CenterTitle();
  pulshist->SetYTitle("mV");
  pulshist->GetYaxis()->CenterTitle();

  cfdhist = new TH1D("CFD", "CFD", 150, 0 - 0.5, 150 - 0.5);
  cfdhist->SetMarkerStyle(7);
  cfdhist->SetLineColor(kBlue);
  cfdhist->SetMarkerColor(kBlue);
}
//_________________________________________________________________________________
MyCFDAdjuster::~MyCFDAdjuster() {
  delete c;
  delete pulshist;
  delete cfdhist;
}
//_________________________________________________________________________________
void MyCFDAdjuster::View(const MyOriginalEvent &oe,
                         MySignalAnalyzedEvent &sae,
                         const MySignalAnalyzer &pa,
                         bool blcorr) {
  if (oe.GetNbrBytes() == 1)
    adjustCfdImpl<char>(oe, sae, pa);
  if (oe.GetNbrBytes() == 2)
    adjustCfdImpl<short>(oe, sae, pa);
}
//________________________CFD Adjustment_______________________________________________________________________________
template<typename T>
void MyCFDAdjuster::adjustCfdImpl(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae, const MySignalAnalyzer &sa) {
  //if the channelnbr is already beyond the maximum nbr of channels, reset it to the first channel//
  if (ChNbr >= oe.GetNbrOfChannels()) {
    ChNbr = 0;
    return;
  }

  //if the channel was ignored in the acquisition, advance to the next channel//
  if (!(oe.GetUsedChannels() & (0x1 << ChNbr))) {
    ++ChNbr;
    return;
  }

  //get channel and section//
  const MyOriginalChannel &oc = oe.GetChannel(ChNbr);
  MySignalAnalyzedChannel &sac = sae.GetChannel(ChNbr);

  //if this channel has no channelsection, advance to the next channel//
  if (!sac.GetNbrOfChannelSections()) {
    ++ChNbr;
    return;
  }

  //check wether section number for the selected channel exists//
  //otherwise choose the first channelsection//
  if (SecNbr >= sac.GetNbrOfChannelSections())
    SecNbr = 0;
  MyChannelSection &cs = sac.GetChannelSection(SecNbr);

  //count how many times there was no puls on the selected channel//
  //if this count exceeds 20, then change to the next channel//
  if (oc.GetNbrPulses() == 0) {
    ++noPulscnter;
  } else
    noPulscnter = 0;
  if (noPulscnter > 20) {
    ++ChNbr;
    noPulscnter = 0;
    return;
  }



  //go through all Pulses in this channel//
  for (size_t i = 0; i < oc.GetNbrPulses(); ++i) {
    const MyPuls &Puls = oc.GetPuls(i);
    //check wether Puls fits in the chosen timerange//
    //otherwise continue//
    if (Puls.GetIndexToFirstPointOfOriginalWaveform() < cs.GetTimeRangeLow()) continue;
    if (Puls.GetIndexToFirstPointOfOriginalWaveform() > cs.GetTimeRangeHigh()) continue;

    bool cont = true;
    bool BreakHere = false;

    //we found a puls that fits to the channel section that we selected, now we can work with it
    while (cont) {
      //extract some infos//
      const T *data = static_cast<const T *>(oc.GetDataPointerForPuls(Puls));
      const long length = Puls.GetLength();
      const long delay = static_cast<long>(cs.GetDelay() / (oe.GetSampleInterval() * 1e9));
      const double fraction = cs.GetFraction();
      const long offset = oc.GetBaseline();
      const double vertGain = oc.GetVertGain();
      const double threshold = cs.GetThreshold();
      const double walk = cs.GetWalk();
      const short fullscale = oc.GetFullScale();
      const long timestamp = Puls.GetIndexToFirstPointOfOriginalWaveform();
      const long chlength = oe.GetNbrSamples();
      const double horpos = oe.GetHorpos() * 1e9;


      //--reset the histos--//
      pulshist->Reset();
      pulshist->SetBit(TH1::kNoStats);
      pulshist->SetBins(chlength, 0 - 0.5, chlength - 0.5);
      cfdhist->Reset();
      cfdhist->SetBins(chlength, 0 - 0.5, chlength - 0.5);



      //--writing the mean value to it--//
      for (size_t j = 0; j < chlength; ++j) {
        pulshist->SetBinContent(j + 1, offset * vertGain);
        cfdhist->SetBinContent(j + 1, offset * vertGain);
      }


      //wichever is shorter the size of the histogram or the length of puls//
      double LowEndOfHist = pulshist->GetXaxis()->GetBinLowEdge(pulshist->GetXaxis()->GetFirst());
      double UpEndOfHist = pulshist->GetXaxis()->GetBinUpEdge(pulshist->GetXaxis()->GetLast());
      int RangeOfHistogram = static_cast<int>(UpEndOfHist - LowEndOfHist);
      int max = (length < RangeOfHistogram) ? length : RangeOfHistogram;

      //--draw the puls--//
      for (int i = 0; i < max; ++i)
        pulshist->SetBinContent(i + 1 + timestamp, data[i] * vertGain);    //normal puls
      pulshist->Draw("p0c");

      //--draw cfd puls--//
      for (int j = delay; j < max; ++j) {
        double fx = (data[j] - static_cast<double>(offset)) * vertGain;
        double fxd = (data[j - delay] - static_cast<double>(offset)) * vertGain;
        double fsx = -fx * fraction + fxd;
        cfdhist->SetBinContent(j + 1 + timestamp, fsx + offset * vertGain);
      }
      cfdhist->Draw("same p0c");




      //set the axis range//
      double minXAxis = timestamp;
      double maxXAxis = timestamp + xRange;
      double height = 0;
      for (int j = 0; j < sac.GetNbrPeaks(); ++j) {
        const MyPeak &Peak = sac.GetPeak(j);
        if (Peak.GetParentPulsNbr() == Puls.GetPulsNbr())
          if (Peak.GetPolarity() == Pol) {
            const double pos = Peak.GetTime() / (oe.GetSampleInterval() * 1e9);
            minXAxis = pos - xRange / 3.;
            maxXAxis = pos + 2. * xRange / 3.;
            height = Peak.GetHeight();
            break;
          }
      }
      pulshist->SetAxisRange(minXAxis, maxXAxis, "X");
      pulshist->SetAxisRange(-fullscale / 2, fullscale / 2, "Y");




      //--draw where the peak was found, code the polarity in color--//
      for (size_t j = 0; j < sac.GetNbrPeaks(); ++j) {
        const MyPeak &Peak = sac.GetPeak(j);
        //check wether peak is belong to puls, continue if not//
        if (Peak.GetParentPulsNbr() != Puls.GetPulsNbr())continue;

        //the cfd value//
        if (Peak.GetPolarity() == kPositive) {
          arrow.SetFillColor(kRed);
          arrow.SetLineColor(kRed);
        }
        if (Peak.GetPolarity() == kNegative) {
          arrow.SetFillColor(kGreen);
          arrow.SetLineColor(kGreen);
        }

        double posInPeak = Peak.GetTime() / (oe.GetSampleInterval() * 1e9) - timestamp - horpos;
        double pos = Peak.GetTime() / (oe.GetSampleInterval() * 1e9) - horpos;
        double y1 = offset * vertGain + walk;
        double y2 = data[static_cast<int>(posInPeak)] * vertGain;

        if (drawPos)arrow.DrawArrow(pos, y1, pos, y2, 0.02, "<|");
      }

      //draw the threshold line if requested//
      line.SetLineColor(kRed);
      if (drawThresh) line.DrawLine(minXAxis, offset * vertGain + threshold, maxXAxis, offset * vertGain + threshold);
      if (drawThresh) line.DrawLine(minXAxis, offset * vertGain - threshold, maxXAxis, offset * vertGain - threshold);


      //draw the Walk line if requested//
      line.SetLineColor(kMagenta);
      if (drawWalk) line.DrawLine(minXAxis, offset * vertGain + walk, maxXAxis, offset * vertGain + walk);



      //--output the values that are set--//
      char out[256];
      if (Pol == kPositive)
        sprintf(out,
                "Sec:%s P:%s  D:%i F:%0.2f W:%0.2f T:%0.2f H:%0.1f",
                cs.GetName(),
                "Pos",
                static_cast<long>(delay * (oe.GetSampleInterval() * 1e9)),
                fraction,
                walk,
                threshold,
                height);
      else if (Pol == kNegative)
        sprintf(out,
                "Sec:%s P:%s  D:%i F:%0.2f W:%0.2f T:%0.2f H:%0.1f",
                cs.GetName(),
                "Neg",
                static_cast<long>(delay * (oe.GetSampleInterval() * 1e9)),
                fraction,
                walk,
                threshold,
                height);
      else
        sprintf(out,
                "Sec:%s P:%s  D:%i F:%0.2f W:%0.2f T:%0.2f H:%0.1f",
                cs.GetName(),
                "NO",
                static_cast<long>(delay * (oe.GetSampleInterval() * 1e9)),
                fraction,
                walk,
                threshold,
                height);
      std::cout << "                                                           \r" << out;


      //--Update the histogram--//
      gPad->Update();

      //--wait for keystroke--//
      // while(!_kbhit()) {
      //     gSystem->Sleep(50);
      // 	   gSystem->ProcessEvents();
      // }
      //--act according to the keystroke that was made--//
      system("/bin/stty raw");
      auto ch = getchar();
      system("/bin/stty cooked");
      if (ch == 'y') cs.SetFraction(cs.GetFraction() - 0.01);
      else if (ch == 'a') cs.SetFraction(cs.GetFraction() + 0.01);
      else if (ch == 'x') cs.SetDelay(cs.GetDelay() - 1);
      else if (ch == 's') cs.SetDelay(cs.GetDelay() + 1);
      else if (ch == 'c') cs.SetThreshold(cs.GetThreshold() - 1);
      else if (ch == 'd') cs.SetThreshold(cs.GetThreshold() + 1);
      else if (ch == 'v') cs.SetWalk(cs.GetWalk() - 1);
      else if (ch == 'f') cs.SetWalk(cs.GetWalk() + 1);
      else if (ch == 'b') xRange -= 10;
      else if (ch == 'g') xRange += 10;
      else if (ch == 'q') {
        ++ChNbr;
        BreakHere = true;
        cont = false;
      } else if (ch == 'w') {
        ++SecNbr;
        BreakHere = true;
        cont = false;
      } else if (ch == 'i') drawWalk = (drawWalk) ? false : true;
      else if (ch == 'o') drawThresh = (drawThresh) ? false : true;
      else if (ch == 'u') drawPos = (drawPos) ? false : true;
      else if (ch == 'p') Pol = (Pol == kPositive) ? kNegative : kPositive;
      else cont = false;

      if (cont) {  // If we want to draw the same event another time
        sac.Clear();  // Clear all found Peaks in Event
        sa.FindPeaksIn(oe, sae);  // Now find them again with the new settings
      }
    }
    if (BreakHere) break;
  }
}

//##################################The actual worker################################
MyEventViewer::~MyEventViewer() {
  //std::cout << "delete eventviewer"<<std::endl;
  delete fEv;
  //std::cout << "done"<<std::endl;
}

void MyEventViewer::Init(int mode, const MyOriginalEventInfo &oei) {
  //if the viewmode has changed, check for what//
  if (mode != fMode) {
    delete fEv;
    if (mode == kShowRaw)
      fEv = new MyRawEventViewer(oei);
    else if (mode == kAdjustCFD)
      fEv = new MyCFDAdjuster();
    else if (mode == kDoNothing)
      fEv = new MyEventViewerDoNothing();
    else {
      std::cout << "You requested a EventViewer Mode(" << mode << "), that does not exist!" << std::endl;
      exit(1);
    }
    fMode = mode;
  }
}
