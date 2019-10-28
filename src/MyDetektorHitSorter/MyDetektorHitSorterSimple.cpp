#include <iostream>
#include <TMath.h>

#include "MyDetektorHitSorterSimple.h"

#include "../MyRootManager/MyHistos.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektor.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektorInfo.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektorHit.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h"

//****************************************The Class Implementation*******************************************************
MyDetektorHitSorterSimple::MyDetektorHitSorterSimple(const MyDetektorInfo &di, MyHistos &rm, int HiOff) :
    MyDetektorHitSorterQuad(di, rm, HiOff) {
  const double runttime = di.GetRunTime();
  const double sfx = di.GetSfU();
  const double maxpos = (runttime + 30) * sfx;
  double maxtof = di.GetMcpProp().GetTimeRangeHigh(0);
  for (size_t i = 0; i < di.GetMcpProp().GetNbrOfTimeRanges(); ++i)
    if (maxtof < di.GetMcpProp().GetTimeRangeHigh(i))
      maxtof = di.GetMcpProp().GetTimeRangeHigh(i);

  rm.create2d(fHiOff + kDetAll,
              "Det",
              "x [mm]",
              "y [mm]",
              200,
              -maxpos,
              maxpos,
              200,
              -maxpos,
              maxpos,
              Form("%s/SorterOutput", di.GetName()));
  rm.create1d(fHiOff + kTime, "Time", "MCP [ns]", 10000, 0, maxtof, Form("%s/SorterOutput", di.GetName()));
  rm.create2d(fHiOff + kPosXVsTime,
              "PosXVsTime",
              "MCP [ns]",
              "x [mm]",
              500,
              0,
              maxtof,
              300,
              -maxpos,
              maxpos,
              Form("%s/SorterOutput", di.GetName()));
  rm.create2d(fHiOff + kPosYVsTime,
              "PosYVsTime",
              "MCP [ns]",
              "y [mm]",
              500,
              0,
              maxtof,
              300,
              -maxpos,
              maxpos,
              Form("%s/SorterOutput", di.GetName()));

  if (di.IsHexAnode())
    std::cout << "using simple sorting for Hex-Anode.. Only U-Layer and V-Layer will be used!!!" << std::endl;
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterSimple::Sort(MySignalAnalyzedEvent &sae, MyDetektor &d, MyHistos &rm) {
  ExtractTimes(sae, d);
  FillHistosBeforeShift(d, rm);
  FillDeadTimeHistos(rm);
  SortForTimesum(d, rm);
  FillRatioHistos(d, rm);
}

//____________________________functions that will use a simple sorting__________________________________
void findBoundriesForSorting(const dVec &anode,
                             const double mcp,
                             const double ts,
                             const double rTime,
                             int &min,
                             int &max) {
  //--we know two things:--//
  //-- |x1-x2|<rTimex  and--//
  //-- x1+x2-2mcp = tsx with tsx = 0.5*(tsxhigh-tsxlow)--//
  //--with this knowledge we can calculate the boundries for the anode--//
  //--given the Timesum and the Runtime--//

  //set min and max to 0//
  min = -2;
  max = -1;

  //--find useful boundries where to search for good timesums--//
  for (int i = 0; i < anode.size(); ++i) {
    //check wether the current anode value will fall into the boundries
    if (TMath::Abs(2. * anode[i] - 2. * mcp - ts) <= rTime) {
      //if the min value is not set, set it now
      if (min == -2) min = i;
      //set the max value (the last time this will be set is if we are still inside the boundries
      max = i;
    }
      //if the min value has been set it means that we are now outside the boundries => quit here
    else if (min != -2)
      break;
  }

  if (min == -2) min = 0;
}
//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterSimple::SortForTimesum(MyDetektor &d, MyHistos &rm) {
  //--calculate the timesum from the given lower and upper boundries for it--//
  const double tsx = d.GetTsu();
  const double tsy = d.GetTsv();
  const double runttime = d.GetRunTime();
  const double tsxLow = d.GetTsuLow();
  const double tsxHigh = d.GetTsuHeigh();
  const double tsyLow = d.GetTsvLow();
  const double tsyHigh = d.GetTsvHeigh();
  const double sfx = d.GetSfU();
  const double sfy = d.GetSfV();
  const double radius = d.GetMCPRadius();

  for (int iMcp = 0; iMcp < mcpvec.size(); ++iMcp) {
    if (mcpvec[iMcp]->IsUsed()) continue;
    //--find the right indizes, only look in the right timerange--//
    int iX1min, iX1max, iX2min, iX2max, iY1min, iY1max, iY2min, iY2max;
    findBoundriesForSorting(u1d, mcpd[iMcp], tsx, runttime, iX1min, iX1max);
    findBoundriesForSorting(u2d, mcpd[iMcp], tsx, runttime, iX2min, iX2max);
    findBoundriesForSorting(v1d, mcpd[iMcp], tsy, runttime, iY1min, iY1max);
    findBoundriesForSorting(v2d, mcpd[iMcp], tsy, runttime, iY2min, iY2max);

    //go through all possible combinations//
    for (int iX1 = iX1min; iX1 <= iX1max; ++iX1) {
      if (u1vec[iX1]->IsUsed()) continue;
      for (int iX2 = iX2min; iX2 <= iX2max; ++iX2) {
        if (u2vec[iX2]->IsUsed()) continue;
        for (int iY1 = iY1min; iY1 <= iY1max; ++iY1) {
          if (v1vec[iY1]->IsUsed()) continue;
          for (int iY2 = iY2min; iY2 <= iY2max; ++iY2) {
            if (v2vec[iY2]->IsUsed()) continue;

            //calc the timesum//
            const double sumx = u1d[iX1] + u2d[iX2] - 2. * mcpd[iMcp];
            const double sumy = v1d[iY1] + v2d[iY2] - 2. * mcpd[iMcp];

            //calc pos and radius//
            const double x_mm = (u1d[iX1] - u2d[iX2]) * sfx;
            const double y_mm = (d.IsHexAnode()) ? 1. / TMath::Sqrt(3.) * (x_mm - 2 * ((v1d[iY1] - v2d[iY2]) * sfy)) :
                                (v1d[iY1] - v2d[iY2]) * sfy;
            const double radius_mm = TMath::Sqrt(x_mm * x_mm + y_mm * y_mm);


            //check wether the timesum is correct//
            if ((sumx > tsxLow) && (sumx < tsxHigh))
              if ((sumy > tsyLow) && (sumy < tsyHigh))
                //check wether the hit is inside the radius of the MCP//
                if (radius_mm < radius) {
                  //add a DetektorHit to the Detektor
                  MyDetektorHit &hit = d.AddHit();
                  hit.SetU1Nbr(u1vec[iX1]->GetPeakNbr());
                  hit.SetU2Nbr(u2vec[iX2]->GetPeakNbr());
                  hit.SetV1Nbr(v1vec[iY1]->GetPeakNbr());
                  hit.SetV2Nbr(v2vec[iY2]->GetPeakNbr());
                  hit.SetMcpNbr(mcpvec[iMcp]->GetPeakNbr());

                  hit.SetRekMeth(3);
                  hit.SetXmm(x_mm);
                  hit.SetYmm(y_mm);
                  hit.SetTime(mcpd[iMcp]);

                  //fill a Histogram with the Hit//
                  rm.fill2d(fHiOff + kDetAll, x_mm, y_mm);
                  rm.fill2d(fHiOff + kPosXVsTime, hit.Time(), x_mm);
                  rm.fill2d(fHiOff + kPosYVsTime, hit.Time(), y_mm);
                  rm.fill1d(fHiOff + kTime, hit.Time());

                  //remember that this mcp Peak has already been used//
                  mcpvec[iMcp]->IsUsed(true);
                  u1vec[iX1]->IsUsed(true);
                  u2vec[iX2]->IsUsed(true);
                  v1vec[iY1]->IsUsed(true);
                  v2vec[iY2]->IsUsed(true);
                }
          }
        }
      }
    }
  }
}