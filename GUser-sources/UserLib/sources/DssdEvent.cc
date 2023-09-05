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
	fTimeStampX = 0; 
	fTimeStampY = 0; 
	fCFDTimeX = 0.; 
	fCFDTimeY = 0.; 
	fEnergyX = 0.;
	fEnergyY = 0.;
	for(int i =0; i<992; i++){
		fTraceX[i]=0;
		fTraceY[i]=0;
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
////! Constructor
DssdEvent::DssdEvent(DssdPixel p, ullint t1, ullint t2, double t3,double t4, double ex, double ey){
	fPixel = p;
	fTimeStampX = t1;
	fTimeStampY = t2;
	fCFDTimeX = t3; 
	fCFDTimeY = t4; 
	fEnergyX = ex;
	fEnergyY = ey;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
DssdEvent::~DssdEvent(){}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void DssdEvent::Clear(){
	for(int i =0; i<992; i++){
		fTraceX[i]=0;
		fTraceY[i]=0;
	}
	fPixel.Clear(); 
	fTimeStampX =0;
	fTimeStampY =0;
	fCFDTimeX = 0.;
	fCFDTimeY = 0.;
	fEnergyX = 0.; 
	fEnergyY = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
