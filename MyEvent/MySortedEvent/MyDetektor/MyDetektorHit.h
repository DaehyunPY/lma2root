#ifndef __MyDetektorHit_H_
#define __MyDetektorHit_H_

#include <TObject.h>

class MyDetektorHit
{
public:
	MyDetektorHit(int ParentDetektorNbr=-1, int HitNbr=-1):
	  fPDNbr(ParentDetektorNbr),fHitNbr(HitNbr),
	  fU1Nbr(-1), fU2Nbr(-1), fV1Nbr(-1), fV2Nbr(-1), fW1Nbr(-1), fW2Nbr(-1), fMcpNbr(-1),
	  fX_mm(0), fY_mm(0), fTime(0), fRekmeth(-1)
													{}

public:
	void		 SetU1Nbr(int in)					{fU1Nbr = in;}
	void		 SetU2Nbr(int in)					{fU2Nbr = in;}
	void		 SetV1Nbr(int in)					{fV1Nbr = in;}
	void		 SetV2Nbr(int in)					{fV2Nbr = in;}
	void		 SetW1Nbr(int in)					{fW1Nbr = in;}
	void		 SetW2Nbr(int in)					{fW2Nbr = in;}
	void		 SetMcpNbr(int in)					{fMcpNbr = in;}

	void		 SetXmm(double  in)					{fX_mm = in;}
	void		 SetYmm(double  in)					{fY_mm = in;}
	void		 SetTime(double in)					{fTime = in;}
	void		 SetTof(double in)					{fTof = in;}
	void		 SetRekMeth(int in)					{fRekmeth = in;}

	int			 GetParentDetektorNbr()const		{return fPDNbr;}
	int			 GetHitNbr()const					{return fHitNbr;}

	int			 GetU1Nbr()const					{return fU1Nbr;}
	int			 GetU2Nbr()const					{return fU2Nbr;}
	int			 GetV1Nbr()const					{return fV1Nbr;}
	int			 GetV2Nbr()const					{return fV2Nbr;}
	int			 GetW1Nbr()const					{return fW1Nbr;}
	int			 GetW2Nbr()const					{return fW2Nbr;}
	int			 GetMcpNbr()const					{return fMcpNbr;}

	double		 X()const							{return fX_mm;}
	double		 Y()const							{return fY_mm;}
	double		 Time()const						{return fTime;}
	double		 Tof()const							{return fTof;}
	int			 RekMeth()const						{return fRekmeth;}

protected:
	int			 fPDNbr;							//the number of the parent Detektor
	int			 fHitNbr;							//the number of this hit

	int			 fU1Nbr;							//the number of the u1 Peak
	int			 fU2Nbr;							//the number of the u2 Peak
	int			 fV1Nbr;							//the number of the v1 Peak
	int			 fV2Nbr;							//the number of the v2 Peak
	int			 fW1Nbr;							//the number of the w1 Peak
	int			 fW2Nbr;							//the number of the w2 Peak
	int			 fMcpNbr;							//the number of the mcp Peak

	double		 fX_mm;								//the x component of the detector in mm
	double		 fY_mm;								//the y component of the detector in mm
	double		 fTime;								//the mcp time of this hit on the detector
	double		 fTof;								//this is to set the tof of this detektorhit
	int			 fRekmeth;							//the Reconstruction Method for this Hit

	ClassDef(MyDetektorHit,1)						//this a container class that contains one hit on the detector and reference to the Peaks 
};
#endif
