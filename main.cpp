#include <iostream>
#include <TApplication.h>
#include <TString.h>

#include "./MySettings/MySettings.h"
#include "./MyRootManager/MyHistos.h"
#include "./MyFileProcessor/MyFileProcessor.h"


//_________________________________________________________________________________
bool ChangeSettingsAndGetFileName(TString &filename, MySettings &set)
{
	TString LeftHand;
	TString RightHand;

	while(1)
	{
		char tmp[128];
		std::cin >> tmp;
		filename = tmp;
		//std::cout << filename.Data()<<std::endl;
		//--when q return true--//
		if (!filename.CompareTo("q"))
			return true;
		
		//--if there is a '#--' its a commented line that should be printed--//
		else if ( filename.Contains("#") && filename.Contains("--") )
		{
			std::cout <<std::endl<<filename.Data()<<std::endl;
		}
		//--if there is a '#' its a commented line--//
		else if (filename.Contains("#"))
		{
			//do nothing//
		}
		//--when there is a "=" then check wether a Parameter is changed--//
		else if (filename.Contains("="))
		{
			//--separate input into the lefthand and righthand side of the "="--//
			LeftHand = filename(0,filename.Index("="));
			RightHand = filename(filename.Index("=")+1,(filename.Length()-filename.Index("=")));
			if (RightHand.IsFloat())
				set.Change(LeftHand.Data(),RightHand.Atof());
			else 
				set.Change(LeftHand.Data(),RightHand.Data());
		}
		//--else it must have been a filename, so return--//
		else
		{
			return false;
		}
	}
}


//________________________run___________________________________________________
void run(const bool verbose)
{
	//create rootthings manager, container for settings,lma filename and the fileprocessor//
	MyFileProcessor		 fp(verbose);	//the file processor
	MySettings			 set(verbose);	//contains all the settings
	TString				 LMAFileName;	//contains the filename of the *.lma file to be analyzed

	//enter the main analysis loop//
	while(true)
	{
		//change settings or get filename//
		//if true is returned, 'q' has been given, so quit here//
		if(ChangeSettingsAndGetFileName(LMAFileName,set))
		{
			std::cout << "Ok, this was the last file..."<<std::endl;
			break;
		}

		//open log file//cannot put it on before "ChangeSettingsAndGetFileName(LMAFileName,set)"....
		if (!fp.LogFileIsOpen()) fp.OpenLogFile(set);


		//process file//
		//if true is returned, 'q' has been pressed during the analysis, so quit here//
		if (fp.ProcessFile(LMAFileName,set))
		{
			std::cout << "User requested breaking...."<<std::endl;
			break;
		}
	}
	//now write the calibration files, if requested//
	if (static_cast<int>(set.GetValue("WriteCalibrationStuff",false)))
		fp.WriteConfigureFiles();

	//close log file//
	fp.CloseLogFile();
}


//___________________main entry point will call run_______________________________________
int main(int argc, char* argv[])
{
	//create an application (to be able to create canvases and interakt with them//
	TApplication theApp("App",&argc,argv);

	//find out wether we need to be verbose//
	bool verbose=false;
	if ( argc == 2 ) // argc should be 2 for correct execution
	{
		TString progArg(argv[1]);
		if (progArg.CompareTo("-verbose")==0)
			verbose=true;
	}
#ifdef _DEBUG
	//if we are building a Debug version, we should always be verbose
	//we also want the user to be able to attach a debugger to the process
	////--wait for keystroke--//
	std::cout << "This is the Debug Version of this Program!!" <<std::endl;
	std::cout << "If you want you can now attach the Debugger to this process."<<std::endl;
	std::cout << "To continue this program press any key"<<std::endl;
	while (!_kbhit()){}
	char ch;
	while(_kbhit()) char ch = _getch();
	verbose = true;
#endif 

	//now run program//
	run(verbose);

	std::cout << "..exiting normaly"<<std::endl;
	theApp.Terminate();
	return 0;
}
