/*! \file dssdDataPoint.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 */

#include "dssdDataPoint.h"
//! Constructor
dssdDataPoint::dssdDataPoint(){
	strip = -100;
	time = 0;
	energy =0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor
dssdDataPoint::dssdDataPoint(int s, ullint t, double e){
	strip = s;
	time = t;
	energy = e;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
dssdDataPoint::~dssdDataPoint(){}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void dssdDataPoint::clear(){strip =-100; time = 0; energy =0.;}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Retrun true if the strip is a front strip else return false.
bool dssdDataPoint::is_frontStrip()const{if(strip >= 0 && strip < 128)return true; else return false;}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Return true if the strip is a back strip else return false.
bool dssdDataPoint::is_backStrip()const{if(strip > 127 && strip <=255)return true; else return false;}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Check if the strip is a neighboring one with respect to the dssdDataPoint d.
bool dssdDataPoint::is_neighboringStripOf(dssdDataPoint &d){
	if(strip == d.strip+1 || strip == d.strip-1)return true; else return false;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
