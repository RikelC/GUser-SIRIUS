#ifndef digitalCFD_h
#define digitalCFD_h 1
#include <fstream>
#include <iostream>
#include "global.h"
#include <Math/Interpolator.h>
#include "dssdData.h"

class digitalCFD
{
	private:
		//CFD
		myGlobal* s1;
		UShort_t **delay; 
		Double_t **fraction; 

		Double_t* vout; 
		Double_t* vin; 
		Double_t* vindelayed; 
	public:
		digitalCFD();
		~digitalCFD();
		Double_t getCFD_Time1( dssdData* const ,  Double_t );
		Double_t getCFD_Time2( dssdData* const);
		Double_t getCFD_Time2( dssdData* const,  Int_t, Double_t);
		Double_t getCFD_Time3( dssdData* const,  Double_t);
		void assign_cfd_parameters();
};

#endif
