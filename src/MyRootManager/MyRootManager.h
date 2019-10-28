#ifndef __MyRootManager_H_
#define __MyRootManager_H_

#include "MyHistos.h"

class TTree;
class MySettings;
class MyOriginalEventInfo;
class MyOriginalEvent;
class MySignalAnalyzedEventInfo;
class MySignalAnalyzedEvent;
class MySortedEventInfo;
class MySortedEvent;

class MyRootManager : public MyHistos
{
public:
	MyRootManager(const bool v);
	~MyRootManager();

public:
	void		 Init(MySettings &s, MyOriginalEventInfo*, MyOriginalEvent*&, 
						             MySignalAnalyzedEventInfo*, MySignalAnalyzedEvent*&, 
									 MySortedEventInfo*, MySortedEvent*&);
	void		 FillTrees();
	void		 FlushRootFiles();

private:
	TTree		*oet;			//tree containing the Original Events
	TFile		*oef;			//pointer to the Rootfile, the oe tree gets stored in
	TTree		*saet;			//tree containing the Signal Analyzed Events
	TFile		*saef;			//pointer to the Rootfile, the sae tree gets stored in
	TTree		*set;			//tree containing the Sorted Events
	TFile		*sef;			//pointer to the Rootfile, the sa tree gets stored in
	bool		 fInitialized;	//flag to see wether the root manager is initialized


};
#endif