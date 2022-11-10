/*!
 * \file digitalCFD.h
 * \author Rikel CHAKMA
 * \brief Digital Constant fraction discriminator class
 * \details
 *
 *
 *
 */


#ifndef digitalCFD_h
#define digitalCFD_h 1
#include "dssdData.h"
#include "global.h"
#include <fstream>
#include <iostream>
#include <string>
#include <Math/Interpolator.h>

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class digitalCFD
{
	private:
		//CFD
		myGlobal* s1;
		ushort **delay; 
		double **fraction; 

		double* vout; 
		double* vin; 
		double* vindelayed; 
	public:
		digitalCFD();
		~digitalCFD();
		double getFractional_Time( dssdData* const ,  double );
		double getCFD_Time( dssdData* const);
		double getCFD_Time( dssdData* const,  Int_t, double);
		double getFractional_Time( dssdData* const,  double, std::string);
		void assign_cfd_parameters();
		ullint perform(dssdData* const);
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
