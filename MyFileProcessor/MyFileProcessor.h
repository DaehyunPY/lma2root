#ifndef __MyFileProcessor_H_
#define __MyFileProcessor_H_

#include <conio.h>
#include <fstream>

#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalEventInfo.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEventInfo.h"
#include "../MyEvent/MySortedEvent/MySortedEvent.h"
#include "../MyEvent/MySortedEvent/MySortedEventInfo.h"

#include "../MyPulsHistoFiller/MyPulsHistoFiller.h"
#include "../MyEventViewer/MyEventViewer.h"
#include "../MySignalAnalyzer/MySignalAnalyzer.h"
#include "../MyDetektorHitSorter/MyDetektorHitSorter.h"
#include "../MyRootManager/MyRootManager.h"

class TString;
class MySettings;

class MyFileProcessor  
{
public:
	MyFileProcessor(const bool v);
	~MyFileProcessor();

public:
	bool ProcessFile(const TString &FileName, MySettings&);
	void WriteConfigureFiles();

private:
	MyOriginalEvent				fOE;		//the event as it is stored in the lma file
	MyOriginalEvent			   *fOEp;		//a Pointer to the above, needed for the trees
	MyOriginalEventInfo			fOEI;		//infos about the events as they are stored in the lma file
	MySignalAnalyzedEvent		fSAE;		//this contains the resulting peaks of the analysis of the pulses 
	MySignalAnalyzedEvent	   *fSAEp;		//a Pointer to the above, needed for the tree
	MySignalAnalyzedEventInfo	fSAEI;		//infos about the singal analyzed event, that don't change for each event
	MySortedEvent				fSE;		//the event, that contains the detektorhits
	MySortedEvent			   *fSEp;		//a pointer to the above, needed for the tree
	MySortedEventInfo			fSEI;		//infos about the sorted event that don't change for each event

	MyPulsHistoFiller			fPhf;		//this creates the mean puls histograms
	MyEventViewer				fEv;		//one can view the events with this, also this allowes you to adjust the cfd settings
	MySignalAnalyzer			fSa;		//analyzes the pulses and searches for peaks 
	MyDetektorHitSorter			fDhs;		//sorts the signalanalyzed events for detektorhits
	MyRootManager				fRm;		//contains all histograms, manages the rootfile and the trees

	const bool					fVerbose;

//[2013/10/28 Matsunami] log file
private:
	std::ofstream				fLog;					//Output a log file include lma file names and each first and last event IDs, etc.
	TString						fLogFileName;			//File name of the log file.
	static const std::string	fLogDelimiter;			//Field delimiter of the log file.

public:
	void						OpenLogFile(MySettings &set);	//fopen
	void						CloseLogFile();					//fclose
	bool						LogFileIsOpen();				//fstream::is_open
};

#endif