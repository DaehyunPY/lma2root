#include <iostream>
#include "MySignalAnalyzedEvent.h"
#include "MySignalAnalyzedEventInfo.h"

#include "./MySignalAnalyzedChannel/MySignalAnalyzedChannel.h"
#include "../MyOriginalEvent/MyOriginalChannel/MyOriginalChannel.h"
#include "../MyOriginalEvent/MyOriginalEvent.h"

//______________________________________________________________________________________________________________________
void MySignalAnalyzedEvent::ReadFromEventInfo(const MySignalAnalyzedEventInfo &saei)
{
	//create the amount of Channels requested
	fChannels.clear();
	for (size_t i=0;i<saei.GetNbrOfChannels();++i)
		fChannels.push_back(MySignalAnalyzedChannel(i,saei.GetChannelInfo(i)));
	
	//the method
	fUsedMethod = saei.GetUsedMethod();
}

void MySignalAnalyzedEvent::ClearChannel(size_t ch)//motomura
{
	fChannels[ch].Clear();
}
