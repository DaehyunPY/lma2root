#include <fstream>
#include <iomanip>
#include <TMath.h>

#include "MyDetektorHitSorterAchimQuad.h"

#include "../MyRootManager/MyHistos.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektorInfo.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektor.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektorHit.h"
#include "../MyEvent/MySignalAnalyzedEvent/MyPeak/MyPeak.h"
using namespace std;

//________________________________________Achims Sorter Hex___________________________________________________________________________________________________________________
MyDetektorHitSorterAchimQuad::MyDetektorHitSorterAchimQuad(const MyDetektorInfo &di, MyHistos &rm, int HiOff) :
    MyDetektorHitSorterQuad(di, rm, HiOff), MyDetektorHitSorterAchim(di) {
  //create all needed histograms here//
  const double sfu = di.GetSfU();
  const double runtime = di.GetRunTime();
  const double maxpos_ns = runtime + 30;
  const double maxpos_mm = maxpos_ns * sfu;
  double maxtof = di.GetMcpProp().GetTimeRangeHigh(0);
  for (size_t i = 0; i < di.GetMcpProp().GetNbrOfTimeRanges(); ++i)
    if (maxtof < di.GetMcpProp().GetTimeRangeHigh(i))
      maxtof = di.GetMcpProp().GetTimeRangeHigh(i);

  //timesums//
  rm.create2d(fHiOff + kSumVsUShift,
              "SumVsXShift",
              "x [ns]",
              "xsum [ns]",
              300,
              -maxpos_ns,
              maxpos_ns,
              300,
              -10,
              +10,
              Form("%s/Timesums", di.GetName()));
  rm.create2d(fHiOff + kSumVsVShift,
              "SumVsYShift",
              "y [ns]",
              "ysum [ns]",
              300,
              -maxpos_ns,
              maxpos_ns,
              300,
              -10,
              +10,
              Form("%s/Timesums", di.GetName()));
  rm.create2d(fHiOff + kSumVsUShiftCorr,
              "SumVsXShiftCorr",
              "x [ns]",
              "xsum [ns]",
              300,
              -maxpos_ns,
              maxpos_ns,
              300,
              -10,
              +10,
              Form("%s/Timesums", di.GetName()));
  rm.create2d(fHiOff + kSumVsVShiftCorr,
              "SumVsYShiftCorr",
              "y [ns]",
              "ysum [ns]",
              300,
              -maxpos_ns,
              maxpos_ns,
              300,
              -10,
              +10,
              Form("%s/Timesums", di.GetName()));
  //det//
  rm.create2d(fHiOff + kDetShi_ns,
              "DetShift_ns",
              "x [ns]",
              "y [ns]",
              300,
              -maxpos_ns,
              maxpos_ns,
              300,
              -maxpos_ns,
              maxpos_ns,
              Form("%s/DetPictures", di.GetName()));
  rm.create2d(fHiOff + kDetShi_mm,
              "DetShift_mm",
              "x [mm]",
              "y [mm]",
              300,
              -maxpos_mm,
              maxpos_mm,
              300,
              -maxpos_mm,
              maxpos_mm,
              Form("%s/DetPictures", di.GetName()));
  //output//
  rm.create1d(fHiOff + kUsedMethod,
              "UsedMethod",
              "Reconstruction Method Number",
              60,
              0,
              30,
              Form("%s/SorterOutput", di.GetName()));
  rm.create1d(fHiOff + kTime, "Time", "MCP [ns]", 1000, 0, maxtof, Form("%s/SorterOutput", di.GetName()));
  rm.create2d(fHiOff + kDetAll,
              "DetAll",
              "x [mm]",
              "y [mm]",
              300,
              -maxpos_mm,
              maxpos_mm,
              300,
              -maxpos_mm,
              maxpos_mm,
              Form("%s/SorterOutput", di.GetName()));
  rm.create2d(fHiOff + kDetRisky,
              "DetRisky",
              "x [mm]",
              "y [mm]",
              300,
              -maxpos_mm,
              maxpos_mm,
              300,
              -maxpos_mm,
              maxpos_mm,
              Form("%s/SorterOutput", di.GetName()));
  rm.create2d(fHiOff + kDetNonRisky,
              "DetNonRisky",
              "x [mm]",
              "y [mm]",
              300,
              -maxpos_mm,
              maxpos_mm,
              300,
              -maxpos_mm,
              maxpos_mm,
              Form("%s/SorterOutput", di.GetName()));
  rm.create2d(fHiOff + kPosXVsTime,
              "PosXVsTofAll",
              "MCP [ns]",
              "x [mm]",
              500,
              0,
              maxtof,
              300,
              -maxpos_mm,
              maxpos_mm,
              Form("%s/SorterOutput", di.GetName()));
  rm.create2d(fHiOff + kPosYVsTime,
              "PosYVsTofAll",
              "MCP [ns]",
              "y [mm]",
              500,
              0,
              maxtof,
              300,
              -maxpos_mm,
              maxpos_mm,
              Form("%s/SorterOutput", di.GetName()));
  //calibration//
  rm.create2d(fHiOff + kNonLinearityMap,
              "NonlinearityMap",
              "x [mm]",
              "y [mm]",
              200,
              -100 - 0.5,
              100 - 0.5,
              200,
              -100 - 0.5,
              100 - 0.5,
              Form("%s/SorterOutput", di.GetName()));
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterAchimQuad::SortImpl(MySignalAnalyzedEvent &sae, MyDetektor &d, MyHistos &rm) {
  ExtractTimes(sae, d);
  CreateTDCArrays();
  FillHistosBeforeShift(d, rm);
  Shift(d);
  FillHistosAfterShift(d, rm);
  FillDeadTimeHistos(rm);
  if (d.ActivateSorter()) SortWithAchimSorter();
  CreateDetHits(d, rm);
  FillRatioHistos(d, rm);
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterAchimQuad::CreateTDCArrays() {
  //assign vectors to tdc array//
  if (u1d.size())fAs->tdc[fAs->Cu1] = &u1d[0];
  if (u2d.size())fAs->tdc[fAs->Cu2] = &u2d[0];
  if (v1d.size())fAs->tdc[fAs->Cv1] = &v1d[0];
  if (v2d.size())fAs->tdc[fAs->Cv2] = &v2d[0];
  if (mcpd.size())fAs->tdc[fAs->Cmcp] = &mcpd[0];

  //fill the counter array//
  fCnt[fAs->Cu1] = u1vec.size();
  fCnt[fAs->Cu2] = u2vec.size();
  fCnt[fAs->Cv1] = v1vec.size();
  fCnt[fAs->Cv2] = v2vec.size();
  fCnt[fAs->Cmcp] = mcpvec.size();
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterAchimQuad::CreateDetHits(MyDetektor &d, MyHistos &rm) {
  for (int i = 0; i < fNRecHits; ++i) {
    //add a hit to the detektor//
    MyDetektorHit &hit = d.AddHit();

    //set infos from achims routine//
    hit.SetXmm(fAs->output_hit_array[i]->x);
    hit.SetYmm(fAs->output_hit_array[i]->y);
    hit.SetTime(fAs->output_hit_array[i]->time);
    hit.SetRekMeth(fAs->output_hit_array[i]->method);

    //set which peaks have been used//
    if (fAs->output_hit_array[i]->iCu1 != -1) hit.SetU1Nbr(u1vec[fAs->output_hit_array[i]->iCu1]->GetPeakNbr());
    if (fAs->output_hit_array[i]->iCu2 != -1) hit.SetU2Nbr(u2vec[fAs->output_hit_array[i]->iCu2]->GetPeakNbr());
    if (fAs->output_hit_array[i]->iCv1 != -1) hit.SetV1Nbr(v1vec[fAs->output_hit_array[i]->iCv1]->GetPeakNbr());
    if (fAs->output_hit_array[i]->iCv2 != -1) hit.SetV2Nbr(v2vec[fAs->output_hit_array[i]->iCv2]->GetPeakNbr());
    if (fAs->output_hit_array[i]->iCmcp != -1) hit.SetMcpNbr(mcpvec[fAs->output_hit_array[i]->iCmcp]->GetPeakNbr());

    //fill the output histograms//
    rm.fill1d(fHiOff + kUsedMethod, fAs->output_hit_array[i]->method);
    rm.fill1d(fHiOff + kTime, fAs->output_hit_array[i]->time);
    rm.fill2d(fHiOff + kPosXVsTime, fAs->output_hit_array[i]->time, fAs->output_hit_array[i]->x);
    rm.fill2d(fHiOff + kPosYVsTime, fAs->output_hit_array[i]->time, fAs->output_hit_array[i]->y);

    rm.fill2d(fHiOff + kDetAll, fAs->output_hit_array[i]->x, fAs->output_hit_array[i]->y);
    if (fAs->output_hit_array[i]->method == 3)
      rm.fill2d(fHiOff + kDetNonRisky, fAs->output_hit_array[i]->x, fAs->output_hit_array[i]->y);
    if (fAs->output_hit_array[i]->method != 3)
      rm.fill2d(fHiOff + kDetRisky, fAs->output_hit_array[i]->x, fAs->output_hit_array[i]->y);
  }
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterAchimQuad::Calibrate() {
  fSwc->fill_sum_histograms();
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterAchimQuad::WriteCalibData(const MyDetektorInfo &di) {
  ofstream file;
  file.open(Form("%s_CalibrationData.txt", di.GetName()));
  file.fill('0');
  file << std::fixed << std::setprecision(4);

  file << "-----Calibration Data for" << di.GetName() << "--------------" << std::endl;
  file << "-----Add the part that you want to parameter input txt-file-----" << std::endl;
  file << std::endl;
  file << std::endl;
  file << std::endl;


  //calibrate the timesum
  if (fSwc) {
    file << "######TimesumWalkCorrectionStuff################" << std::endl;
    file << std::endl;
    fSwc->generate_sum_walk_profiles();

    //U-Layer//
    if (fSwc->sumu_profile) {
      file << "#U-Layer" << std::endl;
      file << di.GetName() << "_UNbrOfCorrPts=" << fSwc->sumu_profile->number_of_columns << std::endl;
      for (int binx = 0; binx < fSwc->sumu_profile->number_of_columns; ++binx) {
        file << di.GetName() << "_PosU" << std::setw(3) << binx << "="
             << fSwc->sumu_profile->get_bin_center_x(static_cast<double>(binx)) << "\t";
        file << di.GetName() << "_CorU" << std::setw(3) << binx << "=" << fSwc->sumu_profile->get_y(binx) << std::endl;
      }
      file << std::endl;
    }

    //V-Layer//
    if (fSwc->sumv_profile) {
      file << "#V-Layer" << std::endl;
      file << di.GetName() << "_VNbrOfCorrPts=" << fSwc->sumv_profile->number_of_columns << std::endl;
      for (int binx = 0; binx < fSwc->sumv_profile->number_of_columns; ++binx) {
        file << di.GetName() << "_PosV" << std::setw(3) << binx << "="
             << fSwc->sumv_profile->get_bin_center_x(static_cast<double>(binx)) << "\t";
        file << di.GetName() << "_CorV" << std::setw(3) << binx << "=" << fSwc->sumv_profile->get_y(binx) << std::endl;
      }
      file << std::endl;
    }
  }
  file << std::endl;
  file << std::endl;
  file << std::endl;

}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterAchimQuad::Shift(const MyDetektor &d) {
  fAs->shift_sums(-1, d.GetTsu(), d.GetTsv(), d.GetTsw());                // shift all time sums to zero
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterAchimQuad::FillHistosAfterShift(const MyDetektor &d, MyHistos &rm) {
  //get some infos first//
  const double tsU = d.GetTsu();
  const double tsV = d.GetTsv();

  const double sfu = d.GetSfU();
  const double sfv = d.GetSfV();

  //get shifted things//
  const double u1 = (u1d.size()) ? u1d[0] : 0;
  const double u2 = (u2d.size()) ? u2d[0] : 0;
  const double v1 = (v1d.size()) ? v1d[0] : 0;
  const double v2 = (v2d.size()) ? v2d[0] : 0;
  const double mcp = (mcpd.size()) ? mcpd[0] : 0;

  //draw shifted Sums//
  if (u1d.size() || u2d.size() || mcpd.size())
    rm.fill2d(fHiOff + kSumVsUShift, u1 - u2, u1 + u2 - 2. * mcp);
  if (v1d.size() || v2d.size() || mcpd.size())
    rm.fill2d(fHiOff + kSumVsVShift, v1 - v2, v1 + v2 - 2. * mcp);

  //draw shifted and corrected Sums//
  if (u1d.size() || u2d.size() || mcpd.size())
    // rm.fill2d(fHiOff+kSumVsUShiftCorr,u1-u2,fAs->correct_sum(u1,u2,0)-2.*mcp);
    rm.fill2d(fHiOff + kSumVsUShiftCorr, u1 - u2, fAs->signal_corrector->correct_sum(u1, u2, 0) - 2. * mcp);
  if (v1d.size() || v2d.size() || mcpd.size())
    // rm.fill2d(fHiOff+kSumVsVShiftCorr,v1-v2,fAs->correct_sum(v1,v2,1)-2.*mcp);
    rm.fill2d(fHiOff + kSumVsVShiftCorr, v1 - v2, fAs->signal_corrector->correct_sum(v1, v2, 1) - 2. * mcp);

  //--calc Pos from first hits--//
  const double u_ns = u1 - u2;
  const double v_ns = v1 - v2;

  //with scalefactors//
  const double u_mm = u_ns * sfu;
  const double v_mm = v_ns * sfv;

  //check for right timesum//
  const bool csu = (TMath::Abs(u1 + u2 - 2. * mcp) < 10) && u1d.size() && u2d.size() && mcpd.size();
  const bool csv = (TMath::Abs(v1 + v2 - 2. * mcp) < 10) && v1d.size() && v2d.size() && mcpd.size();

  //draw Histograms with detektor positions//
  if (csu && csv) {
    rm.fill2d(fHiOff + kDetShi_ns, u_ns, v_ns);
    rm.fill2d(fHiOff + kDetShi_mm, u_mm, v_mm);
  }
}