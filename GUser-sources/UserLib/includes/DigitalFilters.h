/*!
 * \file DigitalFilters.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */
#pragma once

#ifndef DigitalFilters_h
#define DigitalFilters_h 1
#include "Global.h"
#include "DssdData.h"
#include "UTypes.h"
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>
#include "TH1.h"
#include "TMath.h"

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class DigitalFilters
{
	private:
		MyGlobal* s1;/*!< A instance to the myGlobal class */
		ushort **fKParameter; /*!< A 2D array for holding the optimized trapezoidal parameters k[board][channel].*/
		ushort **fMParameter; /*!< A 2D array for holding optimized trapzoidal parameters m[board][channel]. */
		double fCapacitance;/*!< fCapacitance of a charge sensitive preamplifier */ //in pF
		double fRCConstant;/*!< RC decay constant of a signal */
		double  fTrapezoidalShaperM ;
		double  fTrapezoidalShaperM2;// Gain of the Shaper Amplifier
		double  fTrapezoidalShaperM1; 
		//arrays for trapezoidal filters
		double* array_u; /*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* array_v;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* array_d;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* array_p;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* Rectangular;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* Trapezoidal;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double Signal(int n, int nStart, int m, DssdData* const data);
		double GetMaxValueOfTrapezoidal1( double signal_is, double* const v , ushort pos, double max);
		double GetMaxValueOfTrapezoidal2( DssdData* const data, double* const v , ushort k,ushort l );
		double GetMaxValueOfTrapezoidal3( DssdData* const data, double* const v , ushort k,ushort l );
		double *diff_arr;

		double *diff_arr2;
		template<class T>
			int * GetStartStopRange(T* const v, double signal_is, int m);
		template<class T>
			void GetMovingAverageOfSignal(T* const v, int N, int nStart, int nEnd);
		double *MA;
		double GetAutoMaxValueOfTrapezoidal(double * const v);

		void GetRCConstant(DssdData* const data);
	public:
		DigitalFilters();
		~DigitalFilters();

		void AssignKMValues();
		double* Get_array_u(){return array_u;}
		double* Get_array_v(){return array_v;}
		double* Get_array_d(){return array_d;}
		double* Get_array_p(){return array_p;}
		double* Get_array_Rectangular(){return Rectangular;}
		double* Get_array_Trapezoidal(){return Trapezoidal;}


		//--------------
		// Algoritm 1
		//---------------
		double TrapezoidalFilterAlgorithm1( DssdData* const, ushort, ushort, TH1* const);
		double TrapezoidalFilterAlgorithm1( DssdData* const, TH1* const) ; 
		//---------------
		//Algorithm 2
		//------------------
		double TrapezoidalFilterAlgorithm2( DssdData* const, ushort, ushort, TH1* const);
		double TrapezoidalFilterAlgorithm2( DssdData* const, TH1* const);

		//---------------
		//Algorithm 3
		//------------------
		double TrapezoidalFilterAlgorithm3( DssdData* const, ushort, ushort, TH1* const);
		double TrapezoidalFilterAlgorithm3( DssdData* const, TH1* const);
		//---------------
		//Algorithm 4
		//------------------
		double TrapezoidalFilterAlgorithm4( DssdData* const, ushort, ushort, TH1* const);
		double TrapezoidalFilterAlgorithm4( DssdData* const, TH1* const);
		//set gain
		void SetDssdGain(double);
		void SetRCConstant(double);
                double Perform(DssdData* const, TH1* const);
                double Perform(DssdData* const);

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
