/*!
 * \file trackerData.cc
 * \author Rikel CHAKMA
 * \brief 
 * \details
 *
 */
/*
#include "trackerData.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//!  Constructor
trackerData::trackerData(){
	s1 = myGlobal::getInstance();

	eventnumber = 0;
	timestamp = 0;


}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
trackerData::~trackerData(){

}*/
/*
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void trackerData::TreatPulseFilter()
{
	cout<<"toto "<<endl;
	float max=0;
	int imax=0;
	int nag=0;
	int nch=0;


	//calcul max
	for(int i1=0;i1<NB_COBO;i1++)
		for(int i2=0;i2<NB_ASAD;i2++)
			for(int i3=0;i3<NB_AGET;i3++)
				for(int i4=0;i4<NB_CHANNEL;i4++)
				{
					for(int i5=BLCALC;i5<NB_SAMPLES;i5++)
					{
						if(TRACE[i1][i2][i3][i4][i5]>max)
						{
							max=TRACE[i1][i2][i3][i4][i5];
							imax=i5;
							nag=i3;
							nch=i4;

						}
					}
					//cout<<"max: "<<max<<endl;      
					amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=max;
					naget[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=nag;
					nv[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=nch;
					max=nag=nch=0;
				}
}

void trackerData::ReconstructGet()
{
	cout<<"I am SED recons---"<<endl;

	for(int i=0;i<NY;i++) ampliy[i]=0;
	for(int i=0;i<NX;i++) amplix[i]=0;

	for(int j=68;j<NB_AGET*NB_CHANNEL;j++)
	{
		if(amplitude[j]>THRESHOLD && j<136)
		{

			if(aget1[j-68]!=0 && aget1[j-68]!=1000)
			{
				ampliy[aget1[j-68]-1]=amplitude[j];
				starty[aget1[j-68]-1]=tstart[j];
			}
		}



		if(amplitude[j]>THRESHOLD && j>=136 && j<204)
			if(aget2[j-136]!=0 && aget2[j-136]!=1000)
			{
				amplix[aget2[j-136]-1]=amplitude[j];
				startx[aget2[j-136]-1]=tstart[j];
			}

		if(amplitude[j]>THRESHOLD && j>=204)
			if(aget3[j-204]!=0 && aget3[j-204]!=1000)
			{

				amplix[aget3[j-204]-1]=amplitude[j];
				startx[aget3[j-204]-1]=tstart[j];
			}
	}


	//MAX
	int imaxx,imaxy;
	float maxx,maxy;
	max_c(amplix,maxx,imaxx,NX);
	max_c(ampliy,maxy,imaxy,NY);

	//multiplicity
	int multx,multy;
	float sumx,sumy;
	mult_c(amplix,imaxx,NX,THRESHOLD,multx,sumx);
	mult_c(ampliy,imaxy,NY,THRESHOLD,multy,sumy);


	//barycentre
	float barx,bary,barxm,barym;

	bar_c(amplix,imaxx,NPB,THRESHOLD,barx);
	bar_c(ampliy,imaxy,NPB,THRESHOLD,bary);

	barxm=barx*2.77;
	barym=bary*2.79;


}

void trackerData::TreatPulseFull()
{
	TreatBaselineGet();

	float max=4100;
	int imax=0;


	//calcul max
	for(int i1=0;i1<NB_COBO;i1++)
		for(int i2=0;i2<NB_ASAD;i2++)
			for(int i3=0;i3<NB_AGET;i3++)
				for(int i4=0;i4<NB_CHANNEL;i4++)
				{
					for(int i5=BLCALC;i5<NB_SAMPLES;i5++)
					{
						if(TRACE[i1][i2][i3][i4][i5]<max)
						{
							max=TRACE[i1][i2][i3][i4][i5];
							imax=i5;
						}
					}
					amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=max;

					//amplitude-baseline
					amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=-(amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]-baseline[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]);

					if(amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]<NSIG*sigbsline[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4])
					{
						amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=0;
						imax=0;max=4100;
						naget[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=i3;
						nv[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=i4;
						continue;
					}
					else
					{
						tmax[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=imax;

						//calcul tstart, tt et tm
						float seuil=baseline[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]-0.8*amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4];
						while(TRACE[i1][i2][i3][i4][imax]<seuil && imax>BLCALC) imax--;
						float a=TRACE[i1][i2][i3][i4][imax+1]-TRACE[i1][i2][i3][i4][imax];
						float b=TRACE[i1][i2][i3][i4][imax]-a*imax;

						if(a!=0)
							tstart[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=(seuil-b)/a;
						else 
							tstart[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=0;

						imax=tmax[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4];
						while(TRACE[i1][i2][i3][i4][imax]<(baseline[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]-sigbsline[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]) && imax<NB_SAMPLES) imax++;
						tt[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=imax-(int)tstart[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4];

						rt[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=tmax[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]-(int)tstart[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4];

						//voie+aget
						naget[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=i3;
						nv[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=i4;
						imax=0;max=4100;
					}
				}
}


void trackerData::TreatBaselineGet()
{
	double somme=0,sigbsl2=0;

	for(int i1=0;i1<NB_COBO;i1++)
		for(int i2=0;i2<NB_ASAD;i2++)
			for(int i3=0;i3<NB_AGET;i3++)
				for(int i4=0;i4<NB_CHANNEL;i4++)
				{
					for(int i5=0;i5<BLCALC;i5++)
					{
						somme+=TRACE[i1][i2][i3][i4][i5];
					}
					baseline[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=somme/BLCALC;
					somme=0;
					for(int i5=0;i5<BLCALC;i5++)
					{
						sigbsl2+=pow(TRACE[i1][i2][i3][i4][i5]-baseline[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4],2)/BLCALC;
					}
					sigbsline[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=TMath::Sqrt(sigbsl2);
					sigbsl2=0;
				}
}


void max_c(float *strip,float& max,int& imax, int n)
{
	max=0;
	imax=0;
	for(int i=0;i<n;i++)
	{
		if(strip[i]>max)
		{
			max=strip[i];
			imax=i;
		}
	}
}

void mult_c(float *strip,int imax,int n,float thresh,int& mult,float& sum)
{
	mult=0;
	sum=0;
	for(int i=imax;i<n;i++)
	{
		if(strip[i]>thresh)
		{
			mult++;
			sum+=strip[i];
		}
		else break;
	}
	for(int i=imax-1;i>0;i--)
	{
		if(strip[i]>thresh)
		{
			mult++;
			sum+=strip[i];
		}
		else break;
	}
}

void bar_c(float *strip,int imax,int ns,float thresh,float& bar)
{
	bar=0;
	double sumbar=0;

	for(int i=imax-ns;i<=imax+ns;i++)
	{
		if(strip[i]>0)
		{
			bar+=strip[i]*(i+0.5);
			sumbar+=strip[i];
		}
	}
	bar/=sumbar;

}
*/
