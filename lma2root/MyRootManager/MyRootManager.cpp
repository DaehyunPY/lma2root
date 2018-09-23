#include <TTree.h>
#include <TFile.h>
#include <TString.h>

#include "MyRootManager.h"

#include "../MySettings/MySettings.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalEvent.h"
#include "../MyEvent/MyOriginalEvent/MyOriginalEventInfo.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEvent.h"
#include "../MyEvent/MySignalAnalyzedEvent/MySignalAnalyzedEventInfo.h"
#include "../MyEvent/MySortedEvent/MySortedEvent.h"
#include "../MyEvent/MySortedEvent/MySortedEventInfo.h"


//___________________________________________________________________________
MyRootManager::MyRootManager(const bool v):
	MyHistos(v),oet(0),saet(0),set(0),fInitialized(false)
{
}

//___________________________________________________________________________
MyRootManager::~MyRootManager()
{
	//std::cout << "deleting rootmanager"<<std::endl;
	//delete the trees and the files containing the trees//
	if (oet)
	{
		delete oet;
		delete oef;
	}
	if (saet)
	{
		delete saet;
		delete saef;
	}
	if (set)
	{
		delete set;
		delete sef;
	}
	//std::cout << "done"<<std::endl;
}

//___________________________________________________________________________
void MyRootManager::FlushRootFiles()
{
	//write the trees to the file, when this is deleted//
	//std::cout << "flushing root files"<<std::endl;
	if (oet) 
	{
		oef = oet->GetCurrentFile();
		oef->cd();
		oet->Write(0,TObject::kOverwrite);
		oef->SaveSelf();
	}
	if (saet) 
	{
		saef = saet->GetCurrentFile();
		saef->cd();
		saet->Write(0,TObject::kOverwrite);
		saef->SaveSelf();
	}
	if (set) 
	{
		sef = set->GetCurrentFile();
		sef->cd();
		set->Write(0,TObject::kOverwrite);
		sef->SaveSelf();
	}
	FlushRootFile();
}

//___________________________________________________________________________
void MyRootManager::FillTrees()
{
	if (oet)	oet->Fill();
	if (saet)	saet->Fill();
	if (set)	set->Fill();
}

//___________________________________________________________________________
void MyRootManager::Init(MySettings &s, MyOriginalEventInfo *oei, MyOriginalEvent *&oe, 
						                MySignalAnalyzedEventInfo *saei, MySignalAnalyzedEvent *&sae, 
										MySortedEventInfo *sei, MySortedEvent *&se)
{
	TString fn;
	//if the tree does not exist yet get filenames from the settings//
	if (!oet)
	{
		//if the filenames exits and has a .root at the end, create a file and a tree//
		fn = s.GetString("OriginalEventFileName","");
		if (fn.CompareTo("") != 0)
		{
			if (fn.EndsWith(".root"))
			{
				//TFile f1("Original.root","recreate");
				//TTree T1("OriginalEvent","Contains the Events from the lma file");
				//T1.Branch("OriginalEvent","MyOriginalEvent",&Event);

				oef = TFile::Open(fn.Data(),"recreate");
				oet = new TTree("OriginalEvent","Contains the Events from the lma file");
				oet->Branch("OriginalEvent","MyOriginalEvent",&oe);
				oet->GetUserInfo()->Add(oei);
			}
			else
				std::cout << "We are not creating a File and Tree for Original Event since the provided filename ("<<fn.Data()<<") misses a .root fileending!"<<std::endl;
		}
	}
	if(!saet)
	{
		fn = s.GetString("SignalAnalyzedEventFileName","");
		if (fn.CompareTo("") != 0)
		{
			if (fn.EndsWith(".root"))
			{
				saef = TFile::Open(fn.Data(),"recreate");
				saet = new TTree("SignalAnalyzedEvent","Contains the Signalanalyzed Events");
				saet->Branch("SignalAnalyzedEvent","MySignalAnalyzedEvent",&sae);
				saet->GetUserInfo()->Add(saei);
			}
			else
				std::cout << "We are not creating a File and Tree for Signal Analyzed Event since the provided filename ("<<fn.Data()<<") misses a .root fileending!"<<std::endl;
		}
	}
	if (!set)
	{
		fn = s.GetString("SortedEventFileName","");
		if (fn.CompareTo("") != 0)
		{
			if (fn.EndsWith(".root"))
			{
				sef = TFile::Open(fn.Data(),"recreate");
				set = new TTree("SortedEvent","Contains the Sorted for Detektorhits Events");
				set->Branch("SortedEvent","MySortedEvent",&se);
				set->GetUserInfo()->Add(sei);
			}
			else
				std::cout << "We are not creating a File and Tree for Sorted Event since the provided filename ("<<fn.Data()<<") misses a .root fileending!"<<std::endl;
		}
	}

	//create the file where the histograms are stored in//
	if(!fInitialized)
	{
		fn = s.GetString("HistogramFileName","Histograms.root");
		if (fn.EndsWith(".root"))
			OpenRootFile(fn);
		else
			OpenRootFile((fn.Append(".root")));
	}
	fInitialized = true;
}