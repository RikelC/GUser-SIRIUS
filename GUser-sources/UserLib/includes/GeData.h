/*!
 * \file GeData.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */
#pragma once

#ifndef GeData_h
#define GeData_h 1
#include "Numexo2Data.h"
#include "TMath.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
/*! 
 *
 */

class GeData : public Numexo2Data
{

	private:
		int fDetectorNumber;
	public:
		GeData();
		~GeData();

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
