#include <iostream>
#include <iomanip>

#include "MyDetektorHitSorterAchim.h"

#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektor.h"
#include "../MyEvent/MySortedEvent/MyDetektor/MyDetektorInfo.h"


//________________________________________Achims Sorter Base___________________________________________________________________________________________________________________
MyDetektorHitSorterAchim::MyDetektorHitSorterAchim(const MyDetektorInfo &di)
{
	fAs = new sort_class();
	fSfc = 0;
	fSwc = 0;
	fAlreadyInitialized=false;
	fNRecHits=0;
	InitAchimSorter(di);
}
//___________________________________________________________________________________________________________________________________________________________
MyDetektorHitSorterAchim::~MyDetektorHitSorterAchim()
{
	delete fSfc;
	delete fSwc;	
	delete fAs;
}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterAchim::SortWithAchimSorter()
{
#ifdef _DEBUG
	static int counter;
	counter++;
	std::cout <<counter<<" before Achims"<<std::endl;
#endif
	fNRecHits = fAs->sort();
#ifdef _DEBUG
	std::cout <<"after Achims"<<std::endl;
#endif
	if (fNRecHits < 0) // error?
	{ 
		char error_text[500];
		fAs->get_error_text(fNRecHits,500,error_text);
		std::cout << "Achims Sorter: "<<error_text<<std::endl;
		exit(1);
	}
	//---Number of Hits---////motomura 2009/6/28 ////Matsunami 2013/10/28 comment out (move)
	//std::cout << "\t" << "Number of Hits : " << std::setw(3) << std::setfill(' ')<< fNRecHits;

}

//___________________________________________________________________________________________________________________________________________________________
void MyDetektorHitSorterAchim::InitAchimSorter(const MyDetektorInfo &di)
{
	if (fAlreadyInitialized) return;

	//set some constants//
	fAs->TDC_resolution_ns					= 0.1;
	fAs->tdc_array_row_length				= 1000;
	fAs->dont_overwrite_original_data		= true;
	fAs->count								= &fCnt[0];
	fAs->Cu1								= 0;
	fAs->Cu2								= 1;
	fAs->Cv1								= 2;
	fAs->Cv2								= 3;
	fAs->Cw1								= 4;
	fAs->Cw2								= 5;
	fAs->Cmcp								= 6;
	fAs->use_sum_correction					= static_cast<bool>(di.GetNbrSumUCorrPoints());
	fAs->use_pos_correction					= false;
	
	//this is needed to tell achims routine that we care for our own arrays//
	fAs->tdc[fAs->Cu1]						= (double*)(0x1);
	fAs->tdc[fAs->Cu2]						= (double*)(0x1);
	fAs->tdc[fAs->Cv1]						= (double*)(0x1);
	fAs->tdc[fAs->Cv2]						= (double*)(0x1);
	fAs->tdc[fAs->Cw1]						= (double*)(0x1);
	fAs->tdc[fAs->Cw2]						= (double*)(0x1);
	fAs->tdc[fAs->Cmcp]						= (double*)(0x1);

	//set the variables that we get from the detektor//
	fAs->uncorrected_time_sum_half_width_u	= di.GetTsuWidth();
	fAs->uncorrected_time_sum_half_width_v	= di.GetTsvWidth();
	fAs->uncorrected_time_sum_half_width_w	= di.GetTswWidth();
	fAs->fu									= di.GetSfU();
	fAs->fv									= di.GetSfV();
	fAs->fw									= di.GetSfW();
	fAs->max_runtime						= di.GetRunTime();
	fAs->dead_time_anode					= di.GetDeadTimeAnode();
	fAs->dead_time_mcp						= di.GetDeadTimeMCP();
	fAs->MCP_radius							= di.GetMCPRadius();
	fAs->use_HEX							= di.IsHexAnode();
	fAs->use_MCP							= di.UseMCP();

	//set the sum walk correction points, Achims Routine will internaly find out how many we gave it//
	for (int i=0;i<di.GetNbrSumUCorrPoints();++i)
		fAs->sum_corrector_U->set_point(di.GetUCorrPos(i),di.GetUCorrCorr(i));

	for (int i=0;i<di.GetNbrSumVCorrPoints();++i)
		fAs->sum_corrector_V->set_point(di.GetVCorrPos(i),di.GetVCorrCorr(i));

	for (int i=0;i<di.GetNbrSumWCorrPoints();++i)
		fAs->sum_corrector_W->set_point(di.GetWCorrPos(i),di.GetWCorrCorr(i));

	//init() must be called only once
	int error_code = fAs->init(); 
	if (error_code != 0)
	{
		char error_text[500];
		fAs->get_error_text(error_code,500,error_text);
		std::cout << "Achims Sorter: "<<error_text<<std::endl;
		exit(1);
	}
	else
	{
		fAlreadyInitialized=true;
		fSwc = sum_walk_calibration_class::new_sum_walk_calibration_class(fAs,49);
		fSfc = new scalefactors_calibration_class(true,fAs->max_runtime*0.78,fAs->fu,fAs->fv,fAs->fw);
	}
}
