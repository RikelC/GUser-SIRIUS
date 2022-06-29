
#ifndef Calibration_h
#define Calibration_h 1

#include "global.h"
#include <fstream>
#include <iostream>
#include  "TRandom3.h"
#include "dssdData.h"

class Calibration
{
	private:
		myGlobal* s1;
		Double_t **calib_m; //!
		Double_t **calib_c; //!
		TRandom3 * rand;//to add uniform random number
	public:
		Calibration();
		~Calibration();
		void assign_dssd_calibration_parameters();
		Double_t perform(dssdData* const);
};

#endif
