#ifndef digitalFilters_h
#define digitalFilters_h 1
#include <math.h>
#include <iostream>
#include <string>
#include "TMath.h"
#include "global.h"
#include "dssdData.h"
#include <fstream>
#include "TH1.h"
using namespace std;
class dssdData;
class digitalFilters
{
	private:
		myGlobal* s1;
		UShort_t **kPar; 
		UShort_t **mPar; 
		Double_t Capacitance;//in pF
		Double_t RC_constant;
		double  trapezoidal_shaper_M ;
		double  trapezoidal_shaper_m2;// Gain of the Shaper Amplifier
		double  trapezoidal_shaper_m1; 
		//arrays for trapezoidal filters
		Double_t* array_u ;
		Double_t* array_v ;
		Double_t* array_d ;
		Double_t* array_p;
		Double_t* Rectangular;
		Double_t* Trapezoidal;
		Double_t signal(Int_t n, Int_t nStart, Int_t m, dssdData* const data);
		Double_t get_max_val_trapezoidal( Double_t signal_is, Double_t* v , UShort_t pos, Double_t max);

	public:
		digitalFilters();
		~digitalFilters();

		void assign_k_m_values();
		Double_t* get_array_u(){return array_u;}
		Double_t* get_array_v(){return array_v;}
		Double_t* get_array_d(){return array_d;}
		Double_t* get_array_p(){return array_p;}
		Double_t* get_array_Rectangular(){return Rectangular;}
		Double_t* get_array_Trapezoidal(){return Trapezoidal;}


		//--------------
		// Algoritm 1
		//---------------
		Double_t trapezoidal_filter_algorithm1( dssdData* const, UShort_t, UShort_t, TH1*);
		Double_t trapezoidal_filter_algorithm1( dssdData* const, TH1*) ; 
		//---------------
		//Algorithm 2
		//------------------
		Double_t trapezoidal_filter_algorithm2( dssdData* const, UShort_t, UShort_t, TH1*);
		Double_t trapezoidal_filter_algorithm2( dssdData* const, TH1*);

		//---------------
		//Algorithm 3
		//------------------
		Double_t trapezoidal_filter_algorithm3( dssdData* const, UShort_t, UShort_t, TH1*);
		Double_t trapezoidal_filter_algorithm3( dssdData* const, TH1*);
		//---------------
		//Algorithm 4
		//------------------
		Double_t trapezoidal_filter_algorithm4( dssdData* const, UShort_t, UShort_t, TH1*);
		Double_t trapezoidal_filter_algorithm4( dssdData* const, TH1*);
		//set gain
		void set_DSSD_gain(Double_t);
		void set_RC_constant(Double_t);

};


#endif
