#include <iostream>
#include "MySortedEvent.h"
#include "MySortedEventInfo.h"

#include "./MyDetektor/MyDetektor.h"

//______________________________________________________________________________________________________________________
void MySortedEvent::ReadFromEventInfo(const MySortedEventInfo &sei)
{
	//detektors
	fDetektors.clear();
	for (size_t i=0;i<sei.GetNbrOfDetektorInfos();++i)
		fDetektors.push_back(MyDetektor(fDetektors.size(),sei.GetDetektorInfo(i)));
}
