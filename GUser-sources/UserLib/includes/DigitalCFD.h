/*!
 * \file DigitalCFD.h
 * \author Rikel CHAKMA
 * \brief Digital Constant fFraction discriminator class
 * \details
 *
 *
 *
 */

#pragma once

#ifndef DigitalCFD_h
#define DigitalCFD_h 1
#include "DssdData.h"
#include "Global.h"
#include <fstream>
#include <iostream>
#include <string>
#include "TGraph.h"
#include "TH2.h"
#include <Math/Interpolator.h>
#include "TRandom3.h"
#define fFixedTimeSample 512
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class DigitalCFD
{
	private:
		//CFD
		MyGlobal* s1;
		ushort **fDelay; 
		double **fFraction; 
		TRandom3 * rand;
		double* fVOut; 
		double* fVIn; 
		double* fVInDelayed; 
		double fZeroCrossingTime;
		double fZeroCrossingSample;
		double fZeroCrossingSampleHeight;
		int fDisplacement;
	public:
		DigitalCFD();
		~DigitalCFD();
		double GetFractionalTime( DssdData* const ,  double );
		double GetCFDTime( DssdData* const);
		double GetCFDTime( DssdData* const,  Int_t, double, TH2F*);
		double GetCFDTime( DssdData* const data, int del,  double fract, TGraph* h1, TGraph *h2, TGraph *h3, TGraph* h4 );
		void AssignCFDParameters();
		double Perform(DssdData* const, std::string, TH2F*);
		double GetZeroCrossingTime() const{return fZeroCrossingTime;}
		double GetZeroCrossingSample() const{return fZeroCrossingSample;}
		double GetZeroCrossingSampleHeight() const{return fZeroCrossingSampleHeight;}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
