/*!
 * \file digitalFilters.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#ifndef digitalFilters_h
#define digitalFilters_h 1
#include "global.h"
#include "dssdData.h"
#include "UTypes.h"
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>
#include "TH1.h"
#include "TMath.h"

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class digitalFilters
{
	private:
		myGlobal* s1;/*!< A instance to the myGlobal class */
		ushort **kPar; /*!< A 2D array for holding the optimized trapezoidal parameters k[board][channel].*/
		ushort **mPar; /*!< A 2D array for holding optimized trapzoidal parameters m[board][channel]. */
		double Capacitance;/*!< Capacitance of a charge sensitive preamplifier */ //in pF
		double RC_constant;/*!< RC decay constant of a signal */
		double  trapezoidal_shaper_M ;
		double  trapezoidal_shaper_m2;// Gain of the Shaper Amplifier
		double  trapezoidal_shaper_m1; 
		//arrays for trapezoidal filters
		double* array_u; /*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* array_v;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* array_d;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* array_p;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* Rectangular;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double* Trapezoidal;/*!< An array of size of the trace used in trapezoidal filter algorithm. */
		double signal(int n, int nStart, int m, dssdData* const data);
		double get_max_val_trapezoidal1( double signal_is, double* const v , ushort pos, double max);
		double get_max_val_trapezoidal2( dssdData* const data, double* const v , ushort k,ushort l );
		double get_max_val_trapezoidal3( dssdData* const data, double* const v , ushort k,ushort l );
		double *diff_arr;

		double *diff_arr2;
		template<class T>
			int * get_start_stop_range(T* const v, double signal_is, int m);
		template<class T>
			void get_moving_average_signal(T* const v, int N, int nStart, int nEnd);
		double *MA;
		double get_auto_max_val_trapezoidal(double * const v);

		void get_RC_constant(dssdData* const data);
	public:
		digitalFilters();
		~digitalFilters();

		void assign_k_m_values();
		double* get_array_u(){return array_u;}
		double* get_array_v(){return array_v;}
		double* get_array_d(){return array_d;}
		double* get_array_p(){return array_p;}
		double* get_array_Rectangular(){return Rectangular;}
		double* get_array_Trapezoidal(){return Trapezoidal;}


		//--------------
		// Algoritm 1
		//---------------
		double trapezoidal_filter_algorithm1( dssdData* const, ushort, ushort, TH1* const);
		double trapezoidal_filter_algorithm1( dssdData* const, TH1* const) ; 
		//---------------
		//Algorithm 2
		//------------------
		double trapezoidal_filter_algorithm2( dssdData* const, ushort, ushort, TH1* const);
		double trapezoidal_filter_algorithm2( dssdData* const, TH1* const);

		//---------------
		//Algorithm 3
		//------------------
		double trapezoidal_filter_algorithm3( dssdData* const, ushort, ushort, TH1* const);
		double trapezoidal_filter_algorithm3( dssdData* const, TH1* const);
		//---------------
		//Algorithm 4
		//------------------
		double trapezoidal_filter_algorithm4( dssdData* const, ushort, ushort, TH1* const);
		double trapezoidal_filter_algorithm4( dssdData* const, TH1* const);
		//set gain
		void set_DSSD_gain(double);
		void set_RC_constant(double);
                double perform(dssdData* const, TH1* const);

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
