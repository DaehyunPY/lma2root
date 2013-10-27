#include "MyDetektorHitSorterQuad.h"
#include "helperfunctionsforDetSorter.h"

#include "../MyRootManager/MyHistos.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektorInfo.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektor.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektorHit.h"

//___________________________________________________________________________________________________________________________________________________________
MyDetektorHitSorterQuad::MyDetektorHitSorterQuad(const MyDetektorInfo &di, MyHistos &rm, int HiOff):
	MyDetektorHitSorterBase(HiOff)
{
	//create all needed histograms here//
	const double tsU		= di.GetTsu();
	const double tsV		= di.GetTsv();
	const double sfu		= di.GetSfU();
	const double sfv		= di.GetSfV();
	const double runtime	= di.GetRunTime();
	const double maxpos_ns	= runtime+30;
	const double maxpos_mm	= maxpos_ns*sfu;
	double maxtof			= di.GetMcpProp().GetTimeRangeHigh(0);
	for (size_t i=0; i< di.GetMcpProp().GetNbrOfTimeRanges(); ++i)
		if (maxtof < di.GetMcpProp().GetTimeRangeHigh(i))
			maxtof = di.GetMcpProp().GetTimeRangeHigh(i);

	//timesums//
	rm.create1d(fHiOff+kSumU,"SumX","xsum [ns]",4000,-1000,1000,Form("%s/Timesums",di.GetName()));
	rm.create1d(fHiOff+kSumV,"SumY","ysum [ns]",4000,-1000,1000,Form("%s/Timesums",di.GetName()));
	rm.create2d(fHiOff+kSumVsURaw,"SumVsXRaw","x [ns]","xsum [ns]",300,-maxpos_ns,maxpos_ns,300,tsU-10,tsU+10,Form("%s/Timesums",di.GetName()));
	rm.create2d(fHiOff+kSumVsVRaw,"SumVsYRaw","y [ns]","ysum [ns]",300,-maxpos_ns,maxpos_ns,300,tsV-10,tsV+10,Form("%s/Timesums",di.GetName()));
	//det//
	rm.create2d(fHiOff+kDetRaw_ns	,"DetRaw_ns"	,"x [ns]","y [ns]",300,-maxpos_ns,maxpos_ns,300,-maxpos_ns,maxpos_ns,Form("%s/DetPictures",di.GetName()));
	rm.create2d(fHiOff+kDetRaw_mm	,"DetRaw_mm"	,"x [mm]","y [mm]",300,-maxpos_mm,maxpos_mm,300,-maxpos_mm,maxpos_mm,Form("%s/DetPictures",di.GetName()));
	//ratio//
	rm.create1d(fHiOff+kNbrRecHits,"NbrRecHits","Nbr of Reconstructed Hits",2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrMCPHits,"NbrMCPHits","Nbr of MCP Hits",2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrU1Hits ,"NbrX1Hits" ,"Nbr of X1 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrU2Hits ,"NbrX2Hits" ,"Nbr of X2 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrV1Hits ,"NbrY1Hits" ,"Nbr of Y1 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kNbrV2Hits ,"NbrY2Hits" ,"Nbr of Y2 Hits", 2000,0,1000,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kURatio,"XRatio","X1 Hits / X2 Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kVRatio,"YRatio","Y1 Hits / Y2 Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kU1MCPRatio,"X1MCPRatio","X1 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kU2MCPRatio,"X2MCPRatio","X2 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kV1MCPRatio,"Y1MCPRatio","Y1 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kV2MCPRatio,"Y2MCPRatio","Y2 Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kMCPRecRatio,"MCPRecRatio","Reconstructed Hits / MCP Hits",100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kU1RecRatio ,"X1RecRatio" ,"Reconstructed Hits / X1 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kU2RecRatio ,"X2RecRatio" ,"Reconstructed Hits / X2 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kV1RecRatio ,"Y1RecRatio" ,"Reconstructed Hits / Y1 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	rm.create1d(fHiOff+kV2RecRatio ,"Y2RecRatio" ,"Reconstructed Hits / Y2 Hits" ,100,0,4,Form("%s/InputOutputRatio",di.GetName()));
	//deadtime//
	rm.create1d(fHiOff+kMcpDeadTime,"McpDeadTime","#Delta mcp [ns]",200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kU1DeadTime ,"X1DeadTime" ,"#Delta x1 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kU2DeadTime ,"X2DeadTime" ,"#Delta x2 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kV1DeadTime ,"Y1DeadTime" ,"#Delta y1 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
	rm.create1d(fHiOff+kV2DeadTime ,"Y2DeadTime" ,"#Delta y2 [ns]" ,200,0,100,Form("%s/Deadtime",di.GetName()));
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterQuad::ExtractTimes(MySignalAnalyzedEvent &sae, const MyDetektor &d)
{
	extractTimes(sae,d.GetU1Prop(),u1vec,u1d);
	extractTimes(sae,d.GetU2Prop(),u2vec,u2d);
	extractTimes(sae,d.GetV1Prop(),v1vec,v1d);
	extractTimes(sae,d.GetV2Prop(),v2vec,v2d);
	extractTimes(sae,d.GetMcpProp(),mcpvec,mcpd);
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterQuad::FillHistosBeforeShift(const MyDetektor &d, MyHistos &rm)
{
	//fill some Histograms to check wether it is working right//
	const double tsU = d.GetTsu();
	const double tsV = d.GetTsv();

	const double sfu = d.GetSfU();
	const double sfv = d.GetSfV();

	//get first hits from the vectors
	const double u1 = (u1d.size())?u1d[0]:0;
	const double u2 = (u2d.size())?u2d[0]:0;
	const double v1 = (v1d.size())?v1d[0]:0;
	const double v2 = (v2d.size())?v2d[0]:0;
	const double mcp = (mcpd.size())?mcpd[0]:0;

	//--draw timesum raw--//
	rm.fill2d(fHiOff+kSumVsURaw,u1-u2,u1+u2-2.*mcp);
	rm.fill2d(fHiOff+kSumVsVRaw,v1-v2,v1+v2-2.*mcp);

	rm.fill1d(fHiOff+kSumU,u1+u2-2.*mcp);
	rm.fill1d(fHiOff+kSumV,v1+v2-2.*mcp);

	//--calc Pos from first hits--//
	const double u_ns = u1-u2;
	const double v_ns = v1-v2;

	//with scalefactors//
	const double u_mm = u_ns * sfu;
    const double v_mm = v_ns * sfv;

	//check for right timesum//
	const bool csu =TMath::Abs( u1+u2-2.*mcp - tsU) < 10;
	const bool csv =TMath::Abs( v1+v2-2.*mcp - tsV) < 10;

	//draw Histograms with detektor positions//
	if (csu && csv)
	{
		rm.fill2d(fHiOff+kDetRaw_ns  ,u_ns,v_ns);
		rm.fill2d(fHiOff+kDetRaw_mm  ,u_mm,v_mm);
	}
}









//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterQuad::FillDeadTimeHistos(MyHistos &rm)
{
	//--get deadtime--//
	for (int i=1;i<mcpd.size();++i)	rm.fill1d(fHiOff+kMcpDeadTime,mcpd[i]- mcpd[i-1]);
	for (int i=1;i<u1d.size();++i)	rm.fill1d(fHiOff+kU1DeadTime ,u1d[i] - u1d[i-1]);
	for (int i=1;i<u2d.size();++i)	rm.fill1d(fHiOff+kU2DeadTime ,u2d[i] - u2d[i-1]);
	for (int i=1;i<v1d.size();++i)	rm.fill1d(fHiOff+kV1DeadTime ,v1d[i] - v1d[i-1]);
	for (int i=1;i<v2d.size();++i)	rm.fill1d(fHiOff+kV2DeadTime ,v2d[i] - v2d[i-1]);
}






//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterQuad::FillRatioHistos(const MyDetektor &d, MyHistos &rm)
{
	const int nRecHits = d.GetNbrOfHits();

	//input output Ratios
	rm.fill1d(fHiOff+kNbrRecHits,nRecHits);

	rm.fill1d(fHiOff+kNbrMCPHits,mcpd.size());
	rm.fill1d(fHiOff+kNbrU1Hits, u1d.size());
	rm.fill1d(fHiOff+kNbrU2Hits, u2d.size());
	rm.fill1d(fHiOff+kNbrV1Hits, v1d.size());
	rm.fill1d(fHiOff+kNbrV2Hits, v2d.size());
	
	rm.fill1d(fHiOff+kURatio,static_cast<double>(u1d.size())/static_cast<double>(u2d.size()));
	rm.fill1d(fHiOff+kVRatio,static_cast<double>(v1d.size())/static_cast<double>(v2d.size()));

	rm.fill1d(fHiOff+kU1MCPRatio,static_cast<double>(u1d.size())/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kU2MCPRatio,static_cast<double>(u2d.size())/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kV1MCPRatio,static_cast<double>(v1d.size())/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kV2MCPRatio,static_cast<double>(v2d.size())/static_cast<double>(mcpd.size()));

	rm.fill1d(fHiOff+kMCPRecRatio,static_cast<double>(nRecHits)/static_cast<double>(mcpd.size()));
	rm.fill1d(fHiOff+kU1RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(u1d.size()));
	rm.fill1d(fHiOff+kU2RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(u2d.size()));
	rm.fill1d(fHiOff+kV1RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(v1d.size()));
	rm.fill1d(fHiOff+kV2RecRatio ,static_cast<double>(nRecHits)/static_cast<double>(v2d.size()));
}