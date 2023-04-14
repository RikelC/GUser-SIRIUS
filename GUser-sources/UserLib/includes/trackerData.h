/*!
 * \file trackerData.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
*/
/*
#ifndef trackerData_h
#define traclerData_h 1
#include "global.h"
#include "UTypes.h"
#include "TMath.h"

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

class trackerData
{

	private:
		myGlobal *s1; 
		uint eventnumber;
		ullint timestamp;

		// ---------- Variables SED---------
		//long int timestamp;
		unsigned int event;
		unsigned int buckidx;
		unsigned int chanidx;
		unsigned int agetidx;
		unsigned int asadidx;
		unsigned int coboidx;
		unsigned int nbitems;
		unsigned int sample;


		float TRACE[NB_COBO][NB_ASAD][NB_AGET][NB_CHANNEL][NB_SAMPLES];
		int lastCellRead[NB_COBO][NB_ASAD][NB_AGET];                                                                                         
		int MultCoBo[NB_COBO];                  
		///////////////////////////////////
		// Analysis parameters: decoding //             
		///////////////////////////////////             

		float baseline[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];                                                                                  
		float sigbsline[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		float amplitude[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int tmax[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		float tstart[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int rt[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int tt[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int naget[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int nv[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];                                                                                          


		/////////////////////////////////////////
		// Analysis parameters: reconstruction //
		/////////////////////////////////////////

		int aget0[NB_CHANNEL];
		int aget1[NB_CHANNEL];
		int aget2[NB_CHANNEL];
		int aget3[NB_CHANNEL];

		float ampliy[NY];
		float amplix[NX];       
		float startx[NX];       
		float starty[NY];               
		int type2;                      


		//------- END Variables SED-----------------



	public:
		trackerData();
		~trackerData();
		void set_eventnumber(uint i){eventnumber =i;}
		void set_timestamp(ullint i){timestamp =i;}
		uint get_eventnumber()const {return eventnumber;}
		ullint get_timestamp()const {return timestamp;}

		void TreatPulseFull();
		void TreatBaselineGet();
		void TreatPulseFilter();
		void ReconstructGet();

};
#endif
*/
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
