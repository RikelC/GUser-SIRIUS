/*!
 * \file DssdPixel.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#include "DssdPixel.h"
//! Constructor
DssdPixel::DssdPixel(){
	fX = -100; 
	fY = -100; 
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
////! Constructor
DssdPixel::DssdPixel(int x, int y){
	fX = x; 
	fY = y; 
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
DssdPixel::~DssdPixel(){}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void DssdPixel::Clear(){
	fX = -100; fY = -100;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
