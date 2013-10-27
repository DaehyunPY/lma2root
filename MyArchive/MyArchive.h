#ifndef __MyArchive_H_
#define __MyArchive_H_

#include <fstream>

class MyEvent;
class MyChannel;
class MyPuls;

class MyArchive
{
public:
	MyArchive(bool w):isWriting(w),filesize(0)			{}
	MyArchive(const char * FiName, bool w):isWriting(w)	{newFile(FiName);}
	~MyArchive()										{if (file.is_open()) file.close();}

	long getFileSize()									{return isWriting?file.tellg():filesize;}
	bool EndOfFile()									{return (filesize <= file.tellg());}
	bool IsWriting()									{return isWriting;}
	bool IsReading()									{return !isWriting;}
	bool fileIsOpen()									{return file.is_open();}
	bool newFile(const char * NewFileName);				

	MyArchive & operator<<(char c)						{file.write(       &c,sizeof(char)  ); return *this;}
	MyArchive & operator<<(short s)						{file.write((char*)&s,sizeof(short) ); return *this;}
	MyArchive & operator<<(long l)						{file.write((char*)&l,sizeof(long)  ); return *this;}
	MyArchive & operator<<(double d)					{file.write((char*)&d,sizeof(double)); return *this;}
	void writeArray(void * data, long sizeOfData)		{file.write((char*)data,sizeOfData);}

	MyArchive & operator>>(char &c)						{file.read(       &c,sizeof(char)  ); return *this;}
	MyArchive & operator>>(short &s)					{file.read((char*)&s,sizeof(short) ); return *this;}
	MyArchive & operator>>(long &l)						{file.read((char*)&l,sizeof(long)  ); return *this;}
	MyArchive & operator>>(double &d)					{file.read((char*)&d,sizeof(double)); return *this;}
	void readArray(void * data, long sizeOfData)		{file.read((char*)data,sizeOfData);}

	enum EWriting {ArReading=false,ArWriting=true};

	void goFirst()										{file.seekg(0,std::ios::beg);}

private:
	std::fstream file;		//the stream to the file
	long filesize;			//in case of reading then this is the filesize of the file
	bool isWriting;			//boolean describing wether this Archive is reading or not
};

#endif 