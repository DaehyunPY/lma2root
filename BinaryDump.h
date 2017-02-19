#pragma once
#include <string>
#include "MyArchive/MyArchive.h"
#include "MyEvent/MySortedEvent/MySortedEvent.h"
#include "MyEvent/MySortedEvent/MyDetektor/MyDetektor.h"
using std::string;

class BinaryDump
{
public:
	BinaryDump() :hitsFile(MyArchive::ArWriting) {};
	BinaryDump(const string fileName);
	virtual ~BinaryDump();

	void OpenFile(const string fileName);
	void WriteData(MySortedEvent&, unsigned int);

private:
	//export files
	MyArchive			hitsFile;
	//std::fstream		expTxt;
};

