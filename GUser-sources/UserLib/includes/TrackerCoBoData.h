/*!
 * \file TrackerCoBoData.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */
#pragma once

#ifndef TrackerCoBoData_h
#define traclerData_h 1
#include "Global.h"
#include "UTypes.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "Rtypes.h"
#include <iostream>
#include <fstream>

#ifndef NUMEXO_NB_CHANNELS
#define NUMEXO_NB_CHANNELS 16
#endif
#ifndef NB_COBO
#define NB_COBO 1
#endif
#ifndef NB_ASAD
#define NB_ASAD 1
#endif
#ifndef NB_AGET
#define NB_AGET 4
#endif
#ifndef NB_CHANNEL
#define NB_CHANNEL 68
#endif
#ifndef NB_SAMPLES
#define NB_SAMPLES 256
#endif
#ifndef BLCALC
#define BLCALC 50
#endif
#ifndef NSIG
#define NSIG 5.0
#endif
#ifndef NX
#define NX 81
#endif
#ifndef NY
#define NY 61
#endif
#ifndef THRESHOLD
#define THRESHOLD 50.0
#endif
#ifndef NPB
#define NPB 3
#endif



//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

class TrackerCoBoData
{

	private:
		MyGlobal *s1; 
		uint fEventNumber;
		ullint fTimeStamp;

		// ---------- Variables SED---------
		unsigned int fAsadIndex;
		unsigned int fCoBoIndex;
		unsigned int fNbItems;

		unsigned int fEvent;
		unsigned int fBucketIndex;
		unsigned int fChannelIndex;
		unsigned int fAgetIndex;
		unsigned int fSample;


		int fNVoie;
		double fTrace[NB_COBO][NB_ASAD][NB_AGET][NB_CHANNEL][NB_SAMPLES];

		double amplitude[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int tmax[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int naget[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int nv[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];                                                                                          


		/////////////////////////////////////////
		// Analysis parameters: reconstruction //
		/////////////////////////////////////////
		int ny;
		int nx;

		int cut;
		int npb;
		double threshold;


		int *chan;//!
		int *ind;//!

		double *ampliy;//!
		double *amplix;//!

		double *startx;//!
		double *starty;//!

		TF1 *fitx;
		TF1 *fity;


		//fichier calib Get

		double *coefx;//! 
		double *coefy;//!
		//lecture lookuptable 
		int *aget0; //!
		int *aget1; //!
		int *aget2; //!
		int *aget3;//!
		int aget;
		int nc;

		int imaxx,imaxy,tmaxx,tmaxy;
		double maxx,maxy;
		int multx,multy;
		double sumx,sumy;
		double barx,bary,barxm,barym, barxmProjection, barymProjection, barzmProjection;	
		const double centerX =110.;//cm
		const double centerY = 80.;//cm
		//------- END Variables SED-----------------



	public:
		TrackerCoBoData();
		~TrackerCoBoData();
		//Setters	
		void SetEventNumber(uint i){fEventNumber =i;}
		void SetTimeStamp(ullint i){fTimeStamp =i;}
		void SetCoboIdx(unsigned int x){ fCoBoIndex = x;}
		void SetAsaIdx(unsigned int x){ fAsadIndex = x;};
		void SetNItems(unsigned int x){fNbItems = x;};
		void SetChanIdx(unsigned int x){fChannelIndex = x;}
		void SetBuckIdx(unsigned int x){fBucketIndex =x;}
		void SetAgetIdx(unsigned int x){ fAgetIndex = x;}
		void SetTrace(){ 
			fTrace[fCoBoIndex][fAsadIndex][fAgetIndex][fChannelIndex][fBucketIndex]=fSample;
		}
		//Getters
		uint GetEventNumber()const {return fEventNumber;}
		ullint GetTimeStamp()const {return fTimeStamp;}
		unsigned int GetCoboIdx() const { return fCoBoIndex;}
		unsigned int GetAsaIdx() const { return fAsadIndex;};
		unsigned int GetNItems() const {return fNbItems;};
		unsigned int *GetSampleAddress() {return &fSample;}
		unsigned int *GetBuckIdxAddress(){return &fBucketIndex;}
		unsigned int *GetChanIdxAddress() {return &fChannelIndex;}
		unsigned int *GetAgetIdxAddress() {return &fAgetIndex;}

		double GetAmpliX(int i){return amplix[i];}	 
		double GetAmpliY(int i){return ampliy[i];}	 
		double GetStartX(int i){return startx[i];}	 
		double GetStartY(int i){return starty[i];}	 
		unsigned int GetChanIdx()const {return fChannelIndex;}
		int GetImaxX() const{return imaxx;}
		int GetImaxY() const{return imaxy;}
		int GetTmaxX() const {return tmaxx;}
		int GetTmaxY() const {return tmaxy;}
		double GetMaxX() const {return maxx;}
		double GetMaxY() const {return maxy;}
		int GetMultX() const {return multx;}
		int GetMultY() const {return multy;}
		double GetSumX() const {return sumx;}
		double GetSumY() const {return sumy;}
		double GetBarX() const {return barx;}
		double GetBarY() const {return bary;}
		double GetBarXm() const {return barxm;}
		double GetBarYm() const{ return barym;}	
		double GetBarXmProjection() const {return barxmProjection;}
		double GetBarZmProjection() const {return barzmProjection;}
		double GetBarYmProjection() const{ return barymProjection;}	
		double *GetMaxXAddress() {return &maxx;}
		double *GetMaxYAddress() {return &maxy;}
		int *GetMultXAddress() {return &multx;}
		int *GetMultyAddress() {return &multy;}

		double *GetSumXAddress() {return &sumx;}
		double *GetSumYAddress() {return &sumy;}
		double *GetBarXAddress() {return &barx;}
		double *GetBarYAddress() {return &bary;}
		double *GetBarXmAddress() {return &barxm;}
		double *GetBarYmAddress() { return &barym;}	
		ullint *GetTimeStampAddress() {return &fTimeStamp;}
		uint * GetEventNumberAddress() {return &fEventNumber;}
		void Reset();
		void TreatPulseGet();
		void PerformAnalysis(TH1*, TH1*, TH1*, TH1*);

};
#endif

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
