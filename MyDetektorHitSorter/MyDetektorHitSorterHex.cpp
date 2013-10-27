#include <TMath.h>

#include "MyDetektorHitSorterHex.h"
#include "helperfunctionsforDetSorter.h"

#include "../MyRootManager/MyHistos.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektorInfo.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektor.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektorHit.h"

//___________________________________________________________________________________________________________________________________________________________
MyDetektorHitSorterHex::MyDetektorHitSorterHex(const MyDetektorInfo &di, MyHistos &rm, int HiOff):
	MyDetektorHitSorterBase(HiOff)
{
	//create all needed histograms here//
	const double tsU		= di.GetTsu();
	const double tsV		= di.GetTsv();
	const double tsW		= di.GetTsw();
	const double sfu		= di.GetSfU();
	const double runtime	= di.GetRunTime();
	const double maxpos_ns	= runtime+30;
	const double maxpos_mm	= maxpos_ns*sfu;
	double maxtof			= di.GetMcpProp().GetTimeRangeHigh(0);
	for (size_t i=0; i< di.GetMcpProp().GetNbrOfTimeRanges(); ++i)
		if (maxtof < di.GetMcpProp().GetTimeRangeHigh(i))
			maxtof = di.GetMcpProp().GetTimeRangeHigh(i);

	//timesums//
	rm.create1d(fHiOff+kSumU,"SumU","usum [ns]",4000,-1000,1000,Form("%s/Timesums",di.GetName()));
	rm.create1d(fHiOff+kSumV,"SumV","vsum [ns]",4000,-1000,1000,Form("%s/Timesums",di.GetName()));
	rm.create1d(fHiOff+kSumW,"SumW","wsum [ns]",4000,-1000,1000,Form("%s/Timesums",di.GetName()));
	rm.create2d(fHiOff+kSumVsURaw,"SumVsURaw","u [ns]","usum [ns]",300,-maxpos_ns,maxpos_ns,300,tsU-10,tsU+10,Form("%s/Timesums",di.GetName()));
	rm.create2d(fHiOff+kSumVsVRaw,"SumVsVRaw","v [ns]","vsum [ns]",300,-maxpos_ns,maxpos_ns,300,tsV-10,tsV+10,Form("%s/Timesums",di.GetName()));
	rm.create2d(fHiOff+kSumVsWRaw,"SumVsWRaw","w [ns]","wsum [ns]",300,-maxpos_ns,maxpos_ns,300,tsW-10,tsW+10,Form("%s/Timesums",di.GetName()));
	//det//
	rm.create2d(fHiOff+kDetRaw_ns	,"DetRaw_ns"	,"x [ns]","y [ns]",300,-maxpos_ns,maxpos_ns,300,-maxpos_ns,maxpos_ns,Form("%s/DetPictures",di.GetName()));
	rm.create2d(fHiOff+kDetUVRaw_ns ,"DetUVRaw_ns"	,"x [ns]","y [ns]",300,-maxpos_ns,maxpos_ns,300,-maxpos_ns,maxpos_ns,Form("%s/DetPictures",di.GetName()));
	rm.create2d(fHiOff+kDetVWRaw_ns ,"DetVWRaw_ns"	,"x [ns]","y [ns]",300,-maxpos_ns,maxpos_ns,300,-maxpos_ns,maxpos_ns,Form("%s/DetPictures",di.GetName()));
	rm.create2d(fHiOff+kDetUWRaw_ns ,"DetUWRaw_ns"	,"x [ns]","y [ns]",300,-maxpos_ns,maxpos_ns,300,-maxpos_ns,maxpos_ns,Form("%s/DetPictures",di.GetName()));
	rm.create2d(fHiOff+kDetRaw_mm	,"DetRaw_mm"	,"x [mm]","y [mm]",300,-maxpos_mm,maxpos_mm,300,-maxpos_mm,maxpos_mm,Form("%s/DetPictures",di.GetName()));
	rm.create2d(fHiOff+kDetUVRaw_mm ,"DetUVRaw_mm"	,"x [mm]","y [mm]",300,-maxpos_mm,maxpos_mm,300,-maxpos_mm,maxpos_mm,Form("%s/DetPictures",di.GetName()));
	rm.create2d(fHiOff+kDetVWRaw_mm ,"DetVWRaw_mm"	,"x [mm]","y [mm]",300,-maxpos_mm,maxpos_mm,300,-maxpos_mm,maxpos_mm,Form("%s/DetPictures",di.GetName()));
	rm.create2d(fHiOff+kDetUWRaw_mm ,"DetUWRaw_mm"	,"x [mm]","y [mm]",300,-maxpos_mm,maxpos_mm,300,-maxpos_mm,maxpos_mm,Form("%s/DetPictures",di.GetName()));
	//ratio//
	rm.create1d(fHiOff+kNbrRecHits,"NbrRecHits","Nbr of Reconstructed Hits",2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrMCPHits,"NbrMCPHits","Nbr of MCP Hits",2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrU1Hits ,"NbrU1Hits" ,"Nbr of U1 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrU2Hits ,"NbrU2Hits" ,"Nbr of U2 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrV1Hits ,"NbrV1Hits" ,"Nbr of V1 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrV2Hits ,"NbrV2Hits" ,"Nbr of V2 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrW1Hits ,"NbrW1Hits" ,"Nbr of W1 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrW2Hits ,"NbrW2Hits" ,"Nbr of W2 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kURatio,"URatio","U1 Hits / U2 Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kVRatio,"VRatio","V1 Hits / V2 Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kWRatio,"WRatio","W1 Hits / W2 Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kU1MCPRatio,"U1MCPRatio","U1 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kU2MCPRatio,"U2MCPRatio","U2 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kV1MCPRatio,"V1MCPRatio","V1 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kV2MCPRatio,"V2MCPRatio","V2 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kW1MCPRatio,"W1MCPRatio","W1 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kW2MCPRatio,"W2MCPRatio","W2 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kMCPRecRatio,"MCPRecRatio","Reconstructed Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kU1RecRatio ,"U1RecRatio" ,"Reconstructed Hits / U1 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kU2RecRatio ,"U2RecRatio" ,"Reconstructed Hits / U2 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kV1RecRatio ,"V1RecRatio" ,"Reconstructed Hits / V1 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kV2RecRatio ,"V2RecRatio" ,"Reconstructed Hits / V2 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kW1RecRatio ,"W1RecRatio" ,"Reconstructed Hits / W1 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kW2RecRatio ,"W2RecRatio" ,"Reconstructed Hits / W2 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	//deadtime//
	rm.create1d(fHiOff+kMcpDeadTime,"McpDeadTime","#Delta mcp [ns]",200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kU1DeadTime ,"U1DeadTime" ,"#Delta u1 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kU2DeadTime ,"U2DeadTime" ,"#Delta u2 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kV1DeadTime ,"V1DeadTime" ,"#Delta v1 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kV2DeadTime ,"V2DeadTime" ,"#Delta v2 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kW1DeadTime ,"W1DeadTime" ,"#Delta w1 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kW2DeadTime ,"W2DeadTime" ,"#Delta w2 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterHex::ExtractTimes(MySignalAnalyzedEvent &sae, const MyDetektor &d)
{
	extractTimes(sae,d.GetU1Prop(),u1vec,u1d);
	extractTimes(sae,d.GetU2Prop(),u2vec,u2d);
	extractTimes(sae,d.GetV1Prop(),v1vec,v1d);
	extractTimes(sae,d.GetV2Prop(),v2vec,v2d);
	extractTimes(sae,d.GetW1Prop(),w1vec,w1d);
	extractTimes(sae,d.GetW2Prop(),w2vec,w2d);
	extractTimes(sae,d.GetMcpProp(),mcpvec,mcpd);
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterHex::FillHistosBeforeShift(const MyDetektor &d, MyHistos &rm)
{
	//fill some Histograms to check wether it is working right//
	const double tsU = d.GetTsu();
	const double tsV = d.GetTsv();
	const double tsW = d.GetTsw();

	const double sfu = d.GetSfU();
	const double sfv = d.GetSfV();
	const double sfw = d.GetSfW();

	//get first hits from the vectors
	const double u1 = (u1d.size())?u1d[0]:0;
	const double u2 = (u2d.size())?u2d[0]:0;
	const double v1 = (v1d.size())?v1d[0]:0;
	const double v2 = (v2d.size())?v2d[0]:0;
	const double w1 = (w1d.size())?w1d[0]:0;
	const double w2 = (w2d.size())?w2d[0]:0;
	const double mcp = (mcpd.size())?mcpd[0]:0;

	//--draw timesum raw--//
	rm.fill2d(fHiOff+kSumVsURaw,u1-u2,u1+u2-2.*mcp);
	rm.fill2d(fHiOff+kSumVsVRaw,v1-v2,v1+v2-2.*mcp);
	rm.fill2d(fHiOff+kSumVsWRaw,w1-w2,w1+w2-2.*mcp);

	rm.fill1d(fHiOff+kSumU,u1+u2-2.*mcp);
	rm.fill1d(fHiOff+kSumV,v1+v2-2.*mcp);
	rm.fill1d(fHiOff+kSumW,w1+w2-2.*mcp);
	

	//--calc Pos from first hits--//

	const double u_ns = u1-u2;
	const double v_ns = v1-v2;
	const double w_ns = w1-w2;

	//without scalefactors//
	const double Xuv_ns = u_ns;
    const double Yuv_ns = 1/TMath::Sqrt(3.) * (u_ns - 2.*v_ns);
    const double Xuw_ns = Xuv_ns;
    const double Yuw_ns = 1/TMath::Sqrt(3.) * (2.*w_ns - u_ns);
    const double Xvw_ns = (v_ns + w_ns);
    const double Yvw_ns = 1/TMath::Sqrt(3.) * (w_ns - v_ns); 

	//with scalefactors//
	const double Xuv_mm = u_ns * sfu;
    const double Yuv_mm = 1/TMath::Sqrt(3.) * (u_ns * sfu - 2.*v_ns * sfv);
    const double Xuw_mm = Xuv_mm;
    const double Yuw_mm = 1/TMath::Sqrt(3.) * (2.*w_ns * sfw  - u_ns * sfu);
    const double Xvw_mm = (v_ns * sfv + w_ns * sfw);
    const double Yvw_mm = 1/TMath::Sqrt(3.) * (w_ns * sfw - v_ns * sfv); 

	//check for right timesum//
	const bool csu = TMath::Abs( u1+u2-2.*mcp - tsU) < 10;
	const bool csv = TMath::Abs( v1+v2-2.*mcp - tsV) < 10;
	const bool csw = TMath::Abs( w1+w2-2.*mcp - tsW) < 10;

	//draw Histograms with detektor positions//
	if (csu && csv)
	{
		rm.fill2d(fHiOff+kDetUVRaw_ns,Xuv_ns,Yuv_ns);
		rm.fill2d(fHiOff+kDetRaw_ns  ,Xuv_ns,Yuv_ns);

		rm.fill2d(fHiOff+kDetUVRaw_mm,Xuv_mm,Yuv_mm);
		rm.fill2d(fHiOff+kDetRaw_mm  ,Xuv_mm,Yuv_mm);
	}
	if (csw && csv)
	{
		rm.fill2d(fHiOff+kDetVWRaw_ns,Xvw_ns,Yvw_ns);
		rm.fill2d(fHiOff+kDetRaw_ns  ,Xvw_ns,Yvw_ns);

		rm.fill2d(fHiOff+kDetVWRaw_mm,Xvw_mm,Yvw_mm);
		rm.fill2d(fHiOff+kDetRaw_mm  ,Xvw_mm,Yvw_mm);
	}
	if (csu && csw)
	{
		rm.fill2d(fHiOff+kDetUWRaw_ns,Xuw_ns,Yuw_ns);
		rm.fill2d(fHiOff+kDetRaw_ns  ,Xuw_ns,Yuw_ns);

		rm.fill2d(fHiOff+kDetUWRaw_mm,Xuw_mm,Yuw_mm);
		rm.fill2d(fHiOff+kDetRaw_mm  ,Xuw_mm,Yuw_mm);
	}
}









//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterHex::FillDeadTimeHistos(MyHistos &rm)
{
	//--get deadtime--//
	for (int i=1;i<mcpd.size();++i)	rm.fill1d(fHiOff+kMcpDeadTime,mcpd[i]- mcpd[i-1]);
	for (int i=1;i<u1d.size();++i)	rm.fill1d(fHiOff+kU1DeadTime ,u1d[i] - u1d[i-1]);
	for (int i=1;i<u2d.size();++i)	rm.fill1d(fHiOff+kU2DeadTime ,u2d[i] - u2d[i-1]);
	for (int i=1;i<v1d.size();++i)	rm.fill1d(fHiOff+kV1DeadTime ,v1d[i] - v1d[i-1]);
	for (int i=1;i<v2d.size();++i)	rm.fill1d(fHiOff+kV2DeadTime ,v2d[i] - v2d[i-1]);
	for (int i=1;i<w1d.size();++i)	rm.fill1d(fHiOff+kW1DeadTime ,w1d[i] - w1d[i-1]);
	for (int i=1;i<w2d.size();++i)	rm.fill1d(fHiOff+kW2DeadTime ,w2d[i] - w2d[i-1]);
}








//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterHex::FillRatioHistos(const MyDetektor &d, MyHistos &rm)
{
	const int nRecHits = d.GetNbrOfHits();

	//input output Ratios
	rm.fill1d(fHiOff+kNbrRecHits,nRecHits);

	rm.fill1d(fHiOff+kNbrMCPHits,mcpd.size());
	rm.fill1d(fHiOff+kNbrU1Hits, u1d.size());
	rm.fill1d(fHiOff+kNbrU2Hits, u2d.size());
	rm.fill1d(fHiOff+kNbrV1Hits, v1d.size());
	rm.fill1d(fHiOff+kNbrV2Hits, v2d.size());
	rm.fill1d(fHiOff+kNbrW1Hits, w1d.size());
	rm.fill1d(fHiOff+kNbrW2Hits, w2d.size());
	
	rm.fill1d(fHiOff+kURatio,static_cast<double>(u1d.size())/static_cast<double>(u2d.size()));
	rm.fill1d(fHiOff+kVRatio,static_cast<double>(v1d.size())/static_cast<double>(v2d.size()));
	rm.fill1d(fHiOff+kWRatio,static_cast<double>(w1d.size())/static_cast<double>(w2d.size()));

	rm.fill1d(fHiOff+kU1MCPRatio,static_cast<double>(u1d.size())/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kU2MCPRatio,static_cast<double>(u2d.size())/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kV1MCPRatio,static_cast<double>(v1d.size())/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kV2MCPRatio,static_cast<double>(v2d.size())/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kW1MCPRatio,static_cast<double>(w1d.size())/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kW2MCPRatio,static_cast<double>(w2d.size())/static_cast<double>(mcpd.size()));

	rm.fill1d(fHiOff+kMCPRecRatio,static_cast<double>(nRecHits)/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kU1RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(u1d.size()));
	rm.fill1d(fHiOff+kU2RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(u2d.size()));
	rm.fill1d(fHiOff+kV1RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(v1d.size()));
	rm.fill1d(fHiOff+kV2RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(v2d.size()));
	rm.fill1d(fHiOff+kW1RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(w1d.size()));
	rm.fill1d(fHiOff+kW2RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(w2d.size()));
}
