#ifndef __MyPulsHistoFiller_H_
#define __MyPulsHistoFiller_H_

class MyHistos;
class MyOriginalEvent;
class MyOriginalEventInfo;
class MySignalAnalyzedEvent;
class MySignalAnalyzedEventInfo;

//____________________Base Class for SignalHisto filling_____________
class MyPulsHistoFillerBase
{
public:
	MyPulsHistoFillerBase()																{}
	MyPulsHistoFillerBase(const MyOriginalEventInfo&, const MySignalAnalyzedEventInfo&, MyHistos&);
	virtual ~MyPulsHistoFillerBase()													{}

public:
	virtual void FillPulsHistos(const MyOriginalEvent&, const MySignalAnalyzedEvent&, MyHistos&)=0;
	virtual void WritePulsHistosToFile(const MyOriginalEventInfo&, const MySignalAnalyzedEventInfo&, const MyHistos&)=0;
};

//__________________8 Bit Version____________________________________
class MyPulsHistoFiller8Bit : public MyPulsHistoFillerBase
{
public:
	MyPulsHistoFiller8Bit(const MyOriginalEventInfo &oei, const MySignalAnalyzedEventInfo &saei, MyHistos &rm):
		MyPulsHistoFillerBase(oei,saei,rm)													{}
public:
	void FillPulsHistos(const MyOriginalEvent&, const MySignalAnalyzedEvent&, MyHistos&);
	void WritePulsHistosToFile(const MyOriginalEventInfo&, const MySignalAnalyzedEventInfo&, const MyHistos&);
};


//__________________16 Bit Version____________________________________
class MyPulsHistoFiller16Bit : public MyPulsHistoFillerBase
{
public:
	MyPulsHistoFiller16Bit(const MyOriginalEventInfo &oei, const MySignalAnalyzedEventInfo &saei, MyHistos &rm):
		MyPulsHistoFillerBase(oei,saei,rm)													{}

public:
	void FillPulsHistos(const MyOriginalEvent&, const MySignalAnalyzedEvent&, MyHistos&);
	void WritePulsHistosToFile(const MyOriginalEventInfo&, const MySignalAnalyzedEventInfo&, const MyHistos&);
};


//__________________Do Nothing Version________________________________
class MyPulsHistoFillerDoNothing : public MyPulsHistoFillerBase
{
public:
	MyPulsHistoFillerDoNothing():
		MyPulsHistoFillerBase()														{}

public:
	void FillPulsHistos(const MyOriginalEvent&, const MySignalAnalyzedEvent&, MyHistos&){}
	void WritePulsHistosToFile(const MyOriginalEventInfo&, const MySignalAnalyzedEventInfo&, const MyHistos&){}
};

//__________________The actual worker________________________________
class MyPulsHistoFiller
{
public:
	MyPulsHistoFiller():fPhf(0),fFill(true),fNBytes(0)								{}
	~MyPulsHistoFiller();

public:
	void Init(bool fill, const MyOriginalEventInfo&, const MySignalAnalyzedEventInfo&, MyHistos&);
	void FillPulsHistos(const MyOriginalEvent &oe, const MySignalAnalyzedEvent &sae, MyHistos &rm)
	{
		if (fPhf)fPhf->FillPulsHistos(oe,sae,rm);
	}
	void WritePulsHistosToFile(const MyOriginalEventInfo &oei, const MySignalAnalyzedEventInfo &saei, const MyHistos &rm)
	{
		if (fPhf)fPhf->WritePulsHistosToFile(oei,saei,rm);
	}

private:
	MyPulsHistoFillerBase		*fPhf;
	bool						 fFill;
	int							 fNBytes;
};


#endif