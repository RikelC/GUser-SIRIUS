/*!
 * \file geData.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#ifndef geData_h
#define geData_h 1
#include "numexo2Data.h"
#include "TMath.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
/*! 
 *
 */

class geData : public numexo2Data
{

	private:
		int detectorNumber;
	public:
		geData();
		~geData();

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
