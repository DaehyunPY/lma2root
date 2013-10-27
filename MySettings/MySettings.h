#ifndef __MySettings_H_
#define __MySettings_H_

#include <TString.h>
#include <vector>
#include <iostream>

class MySetting  
{
public:
	MySetting(const char * n, double v, const bool ve): 
	  fName(n),fVal(v),fString(0),fVerbose(ve)	{if(fVerbose) std::cout << "Created \""<<fName.Data()<<"\" with "<<fVal<<std::endl;}
	MySetting(const char * n, const char * v, const bool ve): 
	  fName(n),fVal(0),fString(v),fVerbose(ve)	{if(fVerbose) std::cout << "Created \""<<fName.Data()<<"\" with "<<fString.Data()<<std::endl;}
	~MySetting()								{}

//protected:
//	MySetting()									{}
//	MySetting& operator=(const MySetting& in)	{return *this;}
//	MySetting(const MySetting& in)				{}

public:
	const char	*GetName() const				{return (fName.Data());}
	double		 GetValue() const				{return (fVal);}
	const char 	*GetString() const				{return (fString.Data());}
	void		 SetValue(double v)				{fVal = v; std::cout <<"Changing \""<<fName.Data()<<"\" to "<<fVal<<std::endl;}
	void		 SetString(const char * v)		{fString = v; std::cout <<"Changing \""<<fName.Data()<<"\" to "<<fString.Data()<<std::endl;}

private:
	TString		 fName;							//contains the name of the value
	double		 fVal;							//the value itself
	TString		 fString;						//a string connected with setting name
	const bool	 fVerbose;						//flag that tell wether this should do some output
};



class MySettings  
{
public:
	MySettings(const bool v):fVerbose(v)					{}
	~MySettings()											{/*std::cout <<"delete settings"<<std::endl;*/for(size_t i=0;i<fSettings.size();++i) delete fSettings[i];/*std::cout<<"done"<<std::endl;*/}

//protected:
//	MySettings& operator=(const MySettings& in)				{return *this;}
//	MySettings(const MySettings& in)						{}

public:
	double		 GetValue(const char * name, double def)		{return (Get(name,def)->GetValue());}
	const char  *GetString(const char * name, const char * def)	{return (Get(name,def)->GetString());}
	MySetting	*Change(const char * name, double val);
	MySetting	*Change(const char * name, const char *val);

private:
	MySetting	*Get(const char * name, double def);
	MySetting	*Get(const char * name, const char * def);
	MySetting	*Add(const char * name, double val)			{fSettings.push_back(new MySetting(name,val,fVerbose));return fSettings.back();}
	MySetting	*Add(const char * name, const char * val)	{fSettings.push_back(new MySetting(name,val,fVerbose));return fSettings.back();}

	std::vector<MySetting*> fSettings;						//vector of Pointers to setting
	const bool				fVerbose;						//flag that tell wether this should do some output
};

#endif 