#include "baselineCorrection.h"


//################BaseLine Correction (test)#############################
void BLCorr(double* data, double* BLdata, double offset, const long ndata, const int aveWindow, const int nAve)//positive
{
	double temp;
	int n;
	double* tempData = new double[ndata];

	for (int i=0; i<ndata; i++)
	{
		temp=0.;
		for (int j=i-aveWindow; j<=i+aveWindow;++j)
		{
			if ((j<0)||(j>=ndata)) temp+=offset;
			else temp+=data[j];
		}
		BLdata[i]=temp/(2*aveWindow+1);

		for (int k=0; k<nAve; ++k)
		{
			temp = 0.;
			n=0;
			for (int j=i-aveWindow; j<=i+aveWindow;++j)
			{
				if (((j>=0)&&(j<ndata))&&(BLdata[i]>data[j]))
				{
					temp+=data[j];
					n++;
				}
			}
			if (n>0) BLdata[i]=temp/n;
      else break;
		}
	}

	for (int i=0; i<ndata; i++)
	{
		temp=0.;
		for (int j=i-aveWindow; j<=i+aveWindow;++j)
		{
			if ((j<0)||(j>=ndata)) temp+=offset;
			else temp+=BLdata[j];
		}
		tempData[i]=temp/(2*aveWindow+1);
	}

	for (int i=0; i<ndata; i++)
		BLdata[i] = tempData[i];

	delete [] tempData;	
}


//void BLCorr(double* data, double* BLdata, double offset, const long ndata)//positive
//{
//	double temp;
//	int n;
//	double* tempData = new double[ndata];
//
//
//	for (int i=0; i<ndata; i++)
//	{
//		temp=0.;
//		for (int j=i-50; j<=i+50;++j)
//		{
//			if ((j<0)||(j>=ndata)) temp+=offset;
//			else temp+=data[j];
//		}
//		BLdata[i]=temp/101;
//
//		for (int k=0; k<2; ++k)
//		{
//			temp = 0.;
//			n=0;
//			for (int j=i-50; j<=i+50;++j)
//			{
//				if (((j>=0)&&(j<ndata))&&(BLdata[i]>data[j]))
//				{
//					temp+=data[j];
//					n++;
//				}
//			}
//			if (n>0) BLdata[i]=temp/n;
//		}
//	}
//
//	for (int i=0; i<ndata; i++)
//	{
//		temp=0.;
//		for (int j=i-50; j<=i+50;++j)
//		{
//			if ((j<0)||(j>=ndata)) temp+=offset;
//			else temp+=BLdata[j];
//		}
//		tempData[i]=temp/101;
//	}
//
//	for (int i=0; i<ndata; i++)
//		BLdata[i] = tempData[i];
//
//	delete [] tempData;	
//
//}
//

//void BLCorr(double* data, double* BLdata, double offset, const long ndata)//negative
//{
//	double temp;
//	int n;
//	double* tempData = new double[ndata];
//
//
//	for (int i=0; i<ndata; i++)
//	{
//		temp=0.;
//		for (int j=i-50; j<=i+50;++j)
//		{
//			if ((j<0)||(j>=ndata)) temp+=offset;
//			else temp+=data[j];
//		}
//		BLdata[i]=temp/101;
//
//		for (int k=0; k<2; ++k)
//		{
//			temp = 0.;
//			n=0;
//			for (int j=i-50; j<=i+50;++j)
//			{
//				if (((j>=0)&&(j<ndata))&&(BLdata[i]<data[j]))// >posi or <nega
//				{
//					temp+=data[j];
//					n++;
//				}
//			}
//			if (n>0) BLdata[i]=temp/n;
//		}
//	}
//
//	for (int i=0; i<ndata; i++)
//	{
//		temp=0.;
//		for (int j=i-50; j<=i+50;++j)
//		{
//			if ((j<0)||(j>=ndata)) temp+=offset;
//			else temp+=BLdata[j];
//		}
//		tempData[i]=temp/101;
//	}
//
//	for (int i=0; i<ndata; i++)
//		BLdata[i] = tempData[i];
//
//	delete [] tempData;	
//
//}