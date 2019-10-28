#ifndef __MyVoltage_h__
#define __MyVoltage_h__

#include <TObject.h>

class MyVoltage
{
public:
	MyVoltage():
		fVolt(0),fChanSecNbr(-1)					{}
	MyVoltage(double Volt, int ChannelSectionNbr):
		fVolt(Volt),fChanSecNbr(ChannelSectionNbr)	{}
public:
	double	GetVolt()const							{return fVolt;}
	int		GetChannelSectionNbr()const				{return fChanSecNbr;}
private:
	double fVolt;									//the Voltage
	int fChanSecNbr;								//the channelsection this voltage belongs to

	ClassDef(MyVoltage,1)							//To Store Voltages measured by the Channel
};
#endif