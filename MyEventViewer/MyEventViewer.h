#ifndef __MyEventViewer_H_
#define __MyEventViewer_H_

#include <vector>
#include <TLine.h>
#include <TArrow.h>
#include <TSpline.h>

class MyOriginalEvent;
class MyOriginalEventInfo;
class MySignalAnalyzedEvent;
class MySignalAnalyzer;
class TCanvas;
class TH1D;

//__________________________the base class for all both event viewers_________________________
typedef std::vector<TH1D*> histVec;
class MyEventViewerBase
{
public:
	MyEventViewerBase();
	virtual ~MyEventViewerBase()	{}

public:
	virtual void View(const MyOriginalEvent&, MySignalAnalyzedEvent&, const MySignalAnalyzer&, bool)=0;

protected:
	TCanvas		*c;
	TH1D		*pulshist;
	TH1D		*cfdhist;
	histVec		 ChHisto;
	histVec		 ChBL;
	histVec		 ChHistoCorr;

	TArrow		 arrow;
	TLine		 line;
	bool		 drawThresh;
	bool		 drawWalk;
	bool		 drawPos;
	int			 xRange;
	int			 SecNbr;
	int			 ChNbr;
	int			 Pol;
	int			 noPulscnter;
};


//__________________________raw event viewer_________________________
class MyRawEventViewer : public MyEventViewerBase
{
public:
	MyRawEventViewer(const MyOriginalEventInfo&);
	~MyRawEventViewer();

public:
	void View(const MyOriginalEvent&, MySignalAnalyzedEvent&, const MySignalAnalyzer&, bool blcorr = false);

private:
	template <typename T> void showEventImpl(const MyOriginalEvent&, const MySignalAnalyzedEvent&);
	template <typename T> void showEventImplBLCorr(const MyOriginalEvent&, const MySignalAnalyzedEvent&);
};

//__________________________the cfd settings adjuster_________________________
class MyCFDAdjuster : public MyEventViewerBase
{
public:
	MyCFDAdjuster();
	~MyCFDAdjuster();

public:
	void View(const MyOriginalEvent&, MySignalAnalyzedEvent&, const MySignalAnalyzer&, bool blcorr = false);

private:
	template <typename T> void adjustCfdImpl(const MyOriginalEvent&, MySignalAnalyzedEvent&, const MySignalAnalyzer&);
};

//__________________________the do nothing version_________________________
class MyEventViewerDoNothing : public MyEventViewerBase
{
public:
	MyEventViewerDoNothing()	{}
	~MyEventViewerDoNothing()	{}

public:
	void View(const MyOriginalEvent&, MySignalAnalyzedEvent&, const MySignalAnalyzer&, bool blcorr = false)	{}
};


//_________________________________the actual worker_______________________
class MyEventViewer
{
public:
	MyEventViewer():fEv(0),fMode(-1)			{}
	~MyEventViewer();

public:	
	enum EViewMode{kDoNothing=0,kShowRaw,kAdjustCFD};
	void Init(int mode, const MyOriginalEventInfo&);
	void View(const MyOriginalEvent &oe, MySignalAnalyzedEvent &sae, const MySignalAnalyzer &sa, bool blcorr=false)
	{
		fEv->View(oe,sae,sa, blcorr);
	}

private:
	MyEventViewerBase	*fEv;
	int					 fMode;
};

#endif