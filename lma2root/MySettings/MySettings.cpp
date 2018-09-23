#include "MySettings.h"

//_________________________________________________________________________________
MySetting * MySettings::Get(const char * wantedName, double def)
{
	//go through all Settings and check wether it already exits
	for (int i=0;i<fSettings.size();++i)
	{
		//if it exits return its value
		if (!strcmp(fSettings[i]->GetName(),wantedName))
			return (fSettings[i]);
	}

	//if it doesn't exist create a setting with the default value
	return (Add(wantedName,def));
}

//_________________________________________________________________________________
MySetting * MySettings::Get(const char * wantedName, const char * def)
{
	//go through all Settings and check wether it already exits
	for (int i=0;i<fSettings.size();++i)
	{
		//if it exits return its value
		if (!strcmp(fSettings[i]->GetName(),wantedName))
			return (fSettings[i]);
	}

	//if it doesn't exist create a setting with the default value
	return (Add(wantedName,def));
}

//_________________________________________________________________________________
MySetting* MySettings::Change(const char * name, double val)
{
	//go through all existing settings and look if a setting with this name already exists//
	for (int i=0;i<fSettings.size();++i)
	{
		if (!strcmp(fSettings[i]->GetName(),name))
		{	
			//if it exits change its value and return the changed setting//
			fSettings[i]->SetValue(val);
			return fSettings[i];
		}
	}
	//if it doesn't exist add a setting with the name and return it
	return (Add(name,val));
}

//_________________________________________________________________________________
MySetting* MySettings::Change(const char * name, const char * val)
{
	//go through all existing settings and look if a setting with this name already exists//
	for (int i=0;i<fSettings.size();++i)
	{
		if (!strcmp(fSettings[i]->GetName(),name))
		{	
			//if it exits change its value and return the changed setting//
			fSettings[i]->SetString(val);
			return fSettings[i];
		}
	}
	//if it doesn't exist add a setting with the name and return it
	return (Add(name,val));
}
