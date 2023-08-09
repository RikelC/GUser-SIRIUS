/*!
 * \file DssdEvent.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#include "DssdEvent.h"
//! Constructor
DssdEvent::DssdEvent(){
	fPixel.SetX(-100); 
	fPixel.SetY(-100); 
	fTimeStamp = 0; 
	fCFDTime = 0.; 
	fEnergyX = 0.;
	fEnergyY = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
////! Constructor
DssdEvent::DssdEvent(DssdPixel p, ullint t1, double t2, double ex, double ey){
	fPixel = p;
        fTimeStamp = t1;
	fCFDTime = t2; 
	fEnergyX = ex;
	fEnergyY = ey;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
DssdEvent::~DssdEvent(){}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void DssdEvent::Clear(){
	fPixel.Clear(); fTimeStamp =0; fCFDTime = 0.; fEnergyX = 0.; fEnergyY = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
