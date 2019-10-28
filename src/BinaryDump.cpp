#include "BinaryDump.h"
#include<iostream>

BinaryDump::BinaryDump(const string fileName) : hitsFile(MyArchive::ArWriting) {
  hitsFile.newFile(fileName.data());
  if (!hitsFile.fileIsOpen())
    std::cout << "Can not open the Dump file: " << fileName;

}

BinaryDump::~BinaryDump() {
  if (hitsFile.fileIsOpen())
    hitsFile.CloseFile();
}

void BinaryDump::OpenFile(const string fileName) {
  hitsFile.newFile(fileName.data());
  if (!hitsFile.fileIsOpen())
    std::cout << "Can not open the Dump file: " << fileName;

}

void BinaryDump::FlushBinFile() {
  hitsFile.FlushFile();
}

void BinaryDump::WriteData(MySortedEvent &se, unsigned int tag) {
  //--- File structure ---//
  // UINT32 Tag
  // INT16 Number of hits
  //	double time
  //	double x
  //	double y
  //	INT16 reconstructon method

  // Write Tag
  hitsFile << tag;
  // For only 1 detector, det number is 0
  MyDetektor &det = se.GetDetektor(0);
  // Write Number of hits
  hitsFile << static_cast<short>(det.GetNbrOfHits());
  // Write hits (x,y,t, reconstruction methon)
  for (size_t i = 0; i < det.GetNbrOfHits(); i++) {
    MyDetektorHit &hit = det.GetHit(i);
    hitsFile << hit.X();
    hitsFile << hit.Y();
    hitsFile << hit.Time();
    hitsFile << static_cast<short>(hit.RekMeth());
  }
}
