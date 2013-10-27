#include <iostream>

#include "MyArchive.h"

//______________________________________________________________________________________________________________________
bool MyArchive::newFile(const char * NewFileName)
{
	//--first check wether there is a file already open--//
	if (file.is_open())
	{
		//if this Archive is writing we need to flush before we close the file
		if(isWriting)
		{
			file.flush();
			file.close();
		}
		//otherwise we just close the file here
		else
		{
			file.close();
		}
	}

	//--if this Archive is reading open the file for reading--//
	if (!isWriting)
	{	
		file.open(NewFileName,std::ios::in | std::ios::binary);
		//if the file has not been opened give an error message and return false
		if(!file.is_open())
		{
			std::cerr << "something went wrong opening the file \""<<NewFileName<<std::endl;
			return false;
		}

		//--get the filesize--//
		file.seekg(0, std::ios::end);
		filesize = file.tellg();
		file.seekg(0, std::ios::beg);
	}
	
	//--if the Archive is writing open the file for writing--//
	else if (isWriting)
	{	
		file.open(NewFileName,std::ios::out | std::ios::binary);
		//if the file has not been opened give an error message and return false
		if(!file.is_open())
		{
			std::cerr << "something went wrong opening the file \""<<NewFileName<<std::endl;
			return false;
		}
	}
	
	//if the file was opened fine return true
	return true;
}