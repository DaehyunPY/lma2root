#include <TStopwatch.h>
#include <TString.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <termios.h>

#include "MyFileProcessor.h"

#include "../MyArchive/MyArchive.h"
#include "../MySettings/MySettings.h"



//_________________________________________________________________________________
MyFileProcessor::~MyFileProcessor()
{
	//std::cout << "delete fileprocessor"<<std::endl;
	//std::cout << "done"<<std::endl;

	// Close log file [2013/10/28 Matsunami]
	CloseLogFile();
}

//_________________________________________________________________________________
MyFileProcessor::MyFileProcessor(const bool v):fVerbose(v),fRm(v)
{
	fOEp	= &fOE;
	fSAEp	= &fSAE;
	fSEp	= &fSE;
}

//________________________[2013/10/28 Matsunami] log file__________________________
void MyFileProcessor::OpenLogFile(MySettings &set)
{
	// Open log file [2013/10/28 Matsunami]
	fLogFileName = set.GetString("LogFileName","log.txt");
	fLog.open(fLogFileName.Data(), std::ios::out);
	//if the file has not been opened give an error message and return false
	if(!fLog.is_open())
	{
		std::cerr << "something went wrong opening the file \""<< fLogFileName <<"\""<<std::endl;
	}
	else // Write header of log
	{
		fLog	<< "LMAFileName"		<<fLogDelimiter
				<< "NbrEvents"			<<fLogDelimiter
				<< "SkipEvents"			<<fLogDelimiter
				<< "Time_Reality_sec"	<<fLogDelimiter
				<< "Time_CPU_sec"		<<fLogDelimiter
				<< "EventID_First"		<<fLogDelimiter
				<< "EventID_Last"		<<std::endl;
	}
}

void MyFileProcessor::CloseLogFile()
{
	if (fLog.is_open()) fLog.close();
}

const std::string MyFileProcessor::fLogDelimiter = "\t";

bool MyFileProcessor::LogFileIsOpen() {return fLog.is_open();}

//_________________________________________________________________________________
void MyFileProcessor::WriteConfigureFiles()
{
	//write the calibration settings and Meanpulses to files
	fPhf.WritePulsHistosToFile(fOEI,fSAEI,fRm);
	fDhs.WriteCalibData(fSEI);
}

//_________________________________________________________________________________
bool MyFileProcessor::ProcessFile(const TString &fiName, MySettings &set, bool isFirstFile)
{
	bool endnow = false;
	
	//--create a stopwatch to time the analysis and start it--//
	TStopwatch watch;
	watch.Start();

	//--open an Archive with filename and check wether the file was opened correctly--//
	MyArchive ar(fiName.Data(),MyArchive::ArReading);
	if(!ar.fileIsOpen())return endnow;
	
	//--create an event counter--//
	size_t EventCounter=0;
	size_t SkipEventCounter=0;

	const size_t IntervalOfTerminalCounterUpdate = static_cast<size_t>(set.GetValue("IntervalOfTerminalCounterUpdate",1)); //[2013/10/28 Matsunami] Interval for updating event counter which is displayed on terminal.

	//--do some output--//
	std::cout <<"working on File: "<<fiName.Data()<<std::endl;

	//--read file Header if something has changed then tell the event to reread the event Header--//
	if(fOEI.ReadFileHeader(ar))
	{
		if (fVerbose)
			std::cout << "something in the file header has changed, copy changes from event header to event"<<std::endl;
		fOE.ReadFromEventInfo(fOEI);
	}
	//--read settings for signal analyzed event, if something has changed then tell the event to reread the event Header--//
	if(fSAEI.ReadSettings(set,fOEI))
	{
		if (fVerbose)
			std::cout << "something in the file header or settings for signal analyzed event has changed, copy changes from event header to event"<<std::endl;
		fSAE.ReadFromEventInfo(fSAEI);
	}
	//--read settings for sorted event, if something has changed then tell the event to reread the event Header--//
	if(fSEI.ReadSettings(set,fSAEI))
	{
		if (fVerbose)
			std::cout << "something in the settings for sorted event has changed, copy changes from event header to event"<<std::endl;
		fSE.ReadFromEventInfo(fSEI);
	}

	//--set up the trees--//
	fRm.Init(set,&fOEI,fOEp,&fSAEI,fSAEp,&fSEI,fSEp);

	//--set up the workers--//
	//signalanalyzer//
	fSa.Init(fOEI,fSAEI);
	//eventviewer&adjuster//
	fEv.Init(static_cast<int>(set.GetValue("ViewMode",MyEventViewer::kDoNothing)+0.1),fOEI);
	//fEv.Init(MyEventViewer::kShowRaw,fOEI);
	//pulshistofiller//
	fPhf.Init(static_cast<int>(set.GetValue("FillPulsHistos",true)+0.1),fOEI,fSAEI,fRm);
	//sorter//
	fDhs.Init(fSEI,fRm);

	// Export bnary file
	bool dumpBin = static_cast<int>(set.GetValue("DumpBinary", false) + 0.1);
	string fileName = string(set.GetString("BinaryFileName", "export.hit"));
	if (isFirstFile && dumpBin) fBD.OpenFile(fileName);

	//--refine signals--//
	bool Diff_Mode = static_cast<int>(set.GetValue("DifferentialMode", false)+0.1);
	bool Smoothing_Mode =  static_cast<int>(set.GetValue("SmoothingMode", false)+0.1);
	bool RemoveNoiseLongPulse_Mode = static_cast<int>(set.GetValue("RemoveNoiseLongPuls", false)+0.1);
	int MaxPulseLength = static_cast<int>(set.GetValue("MaxPulseLength", 1000000)+0.1);
	int MaxPeaks = static_cast<int>(set.GetValue("MaxPeaks", 1000)+0.1);
	double dt_ns = set.GetValue("DifferentialTimeWidth", 1);
	double DifferentialMultiplier = set.GetValue("DifferentialMultiplier", 1);
	double SmoothingTime_ns = set.GetValue("SmoothingWidth", 0.);

	//Clear Unnecessary Channel//motomura 2009/6/28
	bool ClearUnnecessaryChannel = static_cast<int>(set.GetValue("ClearUnnecessaryChannel",true)+0.1);

	bool CheckDataByte = static_cast<int>(set.GetValue("CheckDataByte",false)+0.1);
	//if (CheckDataByte) fOE.MakeDataHisto(fRm);
	// ON/OFF baseline correction
	bool BaselineCorr = static_cast<int>(set.GetValue("BaselineCorrection", false) + 0.1);

	//First EventID//
	fOE.Clear();
	fOE.Serialize(ar);
	long firstEventID = fOE.GetEventID();
	ar.goFirst();
	fOEI.ReadFileHeader(ar);

	//--go through file and extract the events from it--//
	while (!ar.EndOfFile())
	{
		//--clear the events--//
		fOE.Clear();
		fSAE.Clear();
		fSE.Clear();


		//--read event from file--//
		fOE.Serialize(ar);

		//--copy the eventid from originalevent to the other two events--//
		fSAE.CopyEventIDFrom(fOE);
		fSE.CopyEventIDFrom(fOE);

		//---output event number---////motomura 2009/6/28

		if (EventCounter % IntervalOfTerminalCounterUpdate == 0) std::cout << "\r" << "Event Counter :"<< std::setw(5) << std::setfill(' ') << EventCounter+1;
		//std::cout << "\r" << "EventID :"<< std::setw(10) << std::setfill(' ') << fOE.GetEventID();
		//SelectEvent//
		//if (fOE.GetEventID()!=1841952038) continue;
		//--DataSize--//
		//if (CheckDataByte) fOE.FillDataHisto(fRm);

		//---diff mode---//
		//remove long noize signal
		if (RemoveNoiseLongPulse_Mode) fOE.RemoveNoiseLongPulse(MaxPulseLength);
		//differential 
		if (Diff_Mode) fOE.Differential(dt_ns, DifferentialMultiplier);
		//smoothing
		if (Smoothing_Mode) fOE.Smoothing(SmoothingTime_ns);
		
		//--analyze the pulses--//
		fSa.FindPeaksIn(fOE, fSAE, BaselineCorr);
		//--show the raw event or let user adjust the cfd settings--//
		fEv.View(fOE, fSAE, fSa, BaselineCorr);
		//--fill the puls histos--//
		fPhf.FillPulsHistos(fOE,fSAE,fRm);

		//--Skip event for too many peaks--//motomura
		if (RemoveNoiseLongPulse_Mode)
		{
			int NbrOfPeaks = 0;
			for (size_t i = 0; i<7; i++)
				if (fSAE.GetChannel(i).GetNbrPeaks() > NbrOfPeaks)
					NbrOfPeaks = fSAE.GetChannel(i).GetNbrPeaks();
			if (NbrOfPeaks > MaxPeaks) 
			{
				++SkipEventCounter;
				std::cout << "\t" << "---skip event---" ;
				continue;
			}
		}

		//--sort for detektorhits--//
		fDhs.Sort(fSAE,fSE,fRm);
		//[2013/10/28 Matsunami]
		if (EventCounter % IntervalOfTerminalCounterUpdate == 0)
		{
			std::cout << "\t" << "Number of Hits : ";
			for(size_t i=0; i<fDhs.GetNbrRecHitsVec().size(); i++)
				std::cout << std::setw(3) << std::setfill(' ') << fDhs.GetNbrRecHitsVec().at(i);
		}

		//----- Clear unnecessary events-----////motomura 2009/6/28
		if (ClearUnnecessaryChannel)
		{
			//Remain just ch7
			for (size_t i=0; i<6;++i)
				fOE.ClearChannel(i);
			fOE.ClearChannel(8-1);

		}

		if (ClearUnnecessaryChannel)
			for (size_t i=0; i<6;++i) //ch7,ch8
				fSAE.ClearChannel(i);

		//--fill the trees--//
		fRm.FillTrees();

		//--dump to binary file--//
		if (dumpBin)
			fBD.WriteData(fSE, fOE.GetEventID());
			//fBD.WriteData(fSE, EventCounter);//test code 

		//--increase the event counter--//
		++EventCounter;

		//--check wether q was pressed--//
		//if it was pressed then end here//
		switch(getchar()) {
			case 'q':
				endnow=true;
				break;
		}
		// if (_kbhit())
		// {
		// 	char c = _getch();
		// 	if (c == 'q')
		// 	{
		// 		endnow=true;
		// 		break;
		// 	}
		// }
	}
	//Last EventID//
	long endEventID = fOE.GetEventID();
	//flush the root files//
	fRm.FlushRootFiles();
	//flush hits file
	if (dumpBin) fBD.FlushBinFile();
	//--Stop the Stopwatch--//
	watch.Stop();
	//--do some status output--//
	std::cout <<std::endl;
	std::cout << "Nbr Events:      "<<EventCounter<<std::endl;
	std::cout << "Skip Events:     "<<SkipEventCounter<<std::endl;
	Double_t time_reality = watch.RealTime();
	std::cout << "Time (Reality):  "<<time_reality<<"s ("<<static_cast<double>(EventCounter)/time_reality/1000.<<" KHz)"<<std::endl;
	Double_t time_cpu = watch.CpuTime();
	std::cout << "Time (CPU):      "<<time_cpu    <<"s ("<<static_cast<double>(EventCounter)/time_cpu    /1000.<<" KHz)"<<std::endl;
	std::cout << "EventID:      "<<firstEventID<< " to " <<endEventID<<std::endl;
	//std::cout << "Data acquisition time (ms):      "<<endEventID-firstEventID<<std::endl;
	//std::cout << "Data acquisition rate (Hz):      "<<static_cast<double>(EventCounter-1)/((endEventID-firstEventID)/1000.)<<std::endl;
	//std::cout << "Eventloss:      "<<((endEventID-firstEventID)/50.+1)-(EventCounter)<<std::endl;
	std::cout <<std::endl;

	// Write to log file [2013/10/28 Matsunami]
	fLog	<< /*"LMAFileName"*/		fiName.Data()		<<fLogDelimiter
			<< /*"NbrEvents"*/			EventCounter		<<fLogDelimiter
			<< /*"SkipEvents"*/			SkipEventCounter	<<fLogDelimiter
			<< /*"Time_Reality_sec"*/	time_reality		<<fLogDelimiter
			<< /*"Time_CPU_sec"*/		time_cpu			<<fLogDelimiter
			<< /*"EventID_First"*/		firstEventID		<<fLogDelimiter
			<< /*"EventID_Last"*/		endEventID			<<std::endl;

	return endnow;
}
