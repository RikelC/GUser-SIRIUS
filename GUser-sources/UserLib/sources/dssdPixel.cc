/*!
 * \file dssdPixel.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#include "dssdPixel.h"
//! Constructor
dssdPixel::dssdPixel(){
	X = -100; 
	Y = -100; 
	time = 0; 
	energyX = 0.;
	energyY = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
////! Constructor
dssdPixel::dssdPixel(int x, int y, ullint t, double ex, double ey){
	X = x; 
	Y = y; 
	time = t; 
	energyX = ex;
	energyY = ey;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
dssdPixel::~dssdPixel(){}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void dssdPixel::clear(){
	X = -100; Y = -100; time = 0; energyX = 0.; energyY = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
