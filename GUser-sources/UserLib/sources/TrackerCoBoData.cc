/*!
 * \file TrackerCoBoData.cc
 * \author Rikel CHAKMA
 * \brief 
 * \details
 *
 */

#include "TrackerCoBoData.h"

Double_t ff(Double_t *y, Double_t *par)
{
	Double_t func_cosh=par[0]/(TMath::Power(TMath::CosH(TMath::Pi()*(y[0]-par[1])/par[2]),2.));

	return func_cosh;
}

void sed_calibration(double *in,double *coef,int ns)
{

	for(int i=0;i<ns;i++)
	{
		if(i!=15)
			in[i]=in[i]*coef[22]/coef[i];
	}

	//pistes bizarres ou manquantes
	/*
	   for(int i=0;i<ns;i++)
	   {
	   if(i==12) in[i]=in[i-1]/2.+in[i+1]/2.;
	   }
	   */

}

void max_c(double *strip,double *t,double *max,int *imax,int *tma,int n)
{
	*max=0;
	*imax=0;
	for(int i=0;i<n;i++)
	{	       
		if(strip[i]>*max) 
		{
			*max=strip[i];
			*imax=i;
			*tma=t[i];
		}

	}
} 

void mult_c(double *strip,int imax,int n,double thresh,int *mult,double *sum)
{
	*mult=0;
	*sum=0;
	for(int i=imax;i<n;i++)
	{
		if(strip[i]>thresh) {
			(*mult)++;
			(*sum)+=strip[i];
		}
		else break;
	} 
	for(int i=imax-1;i>0;i--)
	{
		if(strip[i]>thresh) {
			(*mult)++;
			(*sum)+=strip[i];
		}
		else break;
	} 
}

void bar_c(double *strip,int imax,int ns,double thresh,double *bar)
{
	*bar=0;
	double sumbar=0;

	for(int i=imax-ns;i<=imax+ns;i++)
	{
		if(strip[i]>0)
		{
			(*bar)+=strip[i]*(i+0.5);
			sumbar+=strip[i];
		}
	}
	(*bar)/=sumbar;

}


//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//!  Constructor
TrackerCoBoData::TrackerCoBoData(){
	s1 = MyGlobal::GetInstance();

	fEventNumber = 0;
	fTimeStamp = 0;
	ny=61;
	nx=81;
	cut=0;
	npb=3;
	threshold=20;
	fNVoie = 272;
	chan=new int[fNVoie];
	ind=new int[fNVoie];
	ampliy=new double[ny];
	amplix=new double[nx];
	startx=new double[nx];
	starty=new double[ny];
	fitx = new TF1("fitx",ff,1,80);
	fity = new TF1("fity",ff,1,60);
	for (int i=0; i<fNVoie;i++)
	{
		amplitude[i]=0.;
		tmax[i]=0;
		nv[i]=0;
		naget[i]=0;
		ind[i] =0;
		chan[i]=0;
	}
	//fichier calib Get
	coefx=new double[nx]; 
	coefy=new double[ny];
	double bufc;

	for(int i=0;i<nx;i++){coefx[i] =0; amplix[i]=0;startx[i]=0;}
	for(int i=0;i<ny;i++){coefy[i]=0; ampliy[i]=0; starty[i]=0;}
	ifstream in("ConfigFiles/calib240fC_260919");
	if(in.is_open()){	
		for(int i=0;i<nx;i++)
		{
			in>>bufc>>coefx[i]>>bufc;
		}
		for(int i=0;i<ny;i++)
		{
			in>>bufc>>coefy[i]>>bufc;
		}
		in.close();
	}
	//lecture lookuptable 
	aget0=new int[68]; 
	aget1=new int[68]; 
	aget2=new int[68]; 
	aget3=new int[68];
	for(int i =0; i <68;i++){
		aget0[i]=0;
		aget1[i]=0;
		aget2[i]=0;
		aget3[i]=0;
	}
	const int linesize=1024;
	char buf[linesize];
	ifstream infile("ConfigFiles/lookuptableV1");
	if(!infile)
	{
		cout<<"impossible d'ouvrir la table d'allocation"<<endl;
		exit(0);
	}
	infile.getline(buf,linesize);
	int l=1;
	//ICI rfEventilation channel aget vers channel detector
	while(l<=272)
	{
		if(l<=68) infile>>aget>>nc>>aget0[l-1];
		else if(l>68 && l<=136) {infile>>aget>>nc>>aget1[l-1-68];//cout<<aget1[l-1-68]<<endl;
		}
		else if(l>136 && l<=204) infile>>aget>>nc>>aget2[l-1-136];
		else infile>>aget>>nc>>aget3[l-1-204]; 
		l++;
	}
	infile.close();
	
        fAsadIndex =0;
	fCoBoIndex =0;
	fNbItems =0;

	fEvent = 0;
	fBucketIndex = 0;
	fChannelIndex = 0;
	fAgetIndex = 0;
	fSample = 0;

	imaxx=0,imaxy=0,tmaxx=0,tmaxy=0;
	maxx=0.,maxy=0.;
	multx=0,multy=0;
	sumx=0.,sumy=0.;
	barx =0,bary =0.,barxm=0.,barym=0., barxmProjection=0., barymProjection=0., barzmProjection=0.;	



}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
TrackerCoBoData::~TrackerCoBoData(){

	delete [] chan; 
	delete [] ind;
	delete [] ampliy;
	delete [] amplix;
	delete [] startx;
	delete [] starty;
	delete [] coefx; 
	delete [] coefy;

	delete [] aget0;
	delete [] aget1; 
	delete [] aget2; 
	delete [] aget3;

}

void TrackerCoBoData::Reset(){

	for(int i1=0;i1<NB_COBO;i1++)
		for(int i2=0;i2<NB_ASAD;i2++)
			for(int i3=0;i3<NB_AGET;i3++)
				for(int i4=0;i4<NB_CHANNEL;i4++)
				{
					amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;
					tmax[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;
					for(int i5=0;i5<NB_SAMPLES;i5++) fTrace[i1][i2][i3][i4][i5]=0;
				}
}

void TrackerCoBoData::PerformAnalysis( TH1* hxinit, TH1* hyinit, TH1* hx, TH1* hy ){
	for(int i=0;i<ny;i++) ampliy[i]=0;
	for(int i=0;i<nx;i++) amplix[i]=0;

	//ICI remplissage amplitude channel detector
	for(int j=68;j<NB_AGET*NB_CHANNEL;j++) 
	{ 
		//------------y--------------------	
		if(amplitude[j]>threshold && j<136)  
		{
			if(hyinit)hyinit->Fill(j-68);
			if(aget1[j-68]!=0 && aget1[j-68]!=1000)
			{
				if(hy)hy->Fill(aget1[j-68]-1);
				ampliy[aget1[j-68]-1]=amplitude[j];
				starty[aget1[j-68]-1]=tmax[j];
				//	 cout<<aget1[j-68]-1<<endl;

			}
		} 
		//---------x-----------
		if(amplitude[j]>threshold && j>=136)  
		{
			if(hxinit)hxinit->Fill(j-136); 			    
		}  
		if(amplitude[j]>threshold && j>=136 && j<204)
			if(aget2[j-136]!=0 && aget2[j-136]!=1000)
			{
				if(hx)hx->Fill(aget2[j-136]-1);
				amplix[aget2[j-136]-1]=amplitude[j];
				startx[aget2[j-136]-1]=tmax[j];

			}
		if(amplitude[j]>threshold && j>=204)
			if(aget3[j-204]!=0 && aget3[j-204]!=1000)
			{
				if(hx)hx->Fill(aget3[j-204]-1);
				amplix[aget3[j-204]-1]=amplitude[j];
				startx[aget3[j-204]-1]=tmax[j];				
			}
	}
	//reconstruction

	//MAX
	max_c(amplix,startx,&maxx,&imaxx,&tmaxx,nx);	
	max_c(ampliy,starty,&maxy,&imaxy,&tmaxy,ny);

	//multiplicity
	mult_c(amplix,imaxx,nx,threshold,&multx,&sumx);
	mult_c(ampliy,imaxy,ny,threshold,&multy,&sumy);

	//barycentre
	//modif charge X vs Y		
	//   amplix[imaxx]+=0.2*ampliy[imaxy];			  	     
	//   ampliy[imaxy]+=0.2*amplix[imaxx];

	bar_c(amplix,imaxx,npb,threshold,&barx);
	bar_c(ampliy,imaxy,npb,threshold,&bary);

	barxm=barx*2.77;
	barym=bary*2.79;
	/*// Before reversing the physical strip direction
  barxmProjection = (barxm - centerX)*TMath::Cos(45.* TMath::Pi()/180.);
	barzmProjection = (barxm - centerX)*TMath::Sin(45.* TMath::Pi()/180.);
	barymProjection = (barym - centerY);

*/
	barymProjection = (barym - centerY)*TMath::Cos(45.* TMath::Pi()/180.);
	barzmProjection = (barym - centerY)*TMath::Sin(45.* TMath::Pi()/180.);
	barxmProjection = (barxm - centerX);
}

void TrackerCoBoData::TreatPulseGet()
{
	double max=0;
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

						//if(fTrace[i1][i2][i3][i4][i5]>max || fTrace[i1][i2][i3][i4][i5]>3500)
						if(fTrace[i1][i2][i3][i4][i5]>max)
						{
							max=fTrace[i1][i2][i3][i4][i5];
							imax=i5;
							nag=i3;
							nch=i4;		      
						}
					}
					//cout<<"imax: "<<imax<<" "<<"nch: "<<nch<<endl;
					amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=max;
					tmax[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=imax;
					naget[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=nag;
					nv[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=nch;
					max=nag=nch=imax=0;
				}
}


//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

