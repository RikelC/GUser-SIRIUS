/*! \file DssdDataPoint.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 */

#include "DssdDataPoint.h"
//! Constructor
DssdDataPoint::DssdDataPoint() : Numexo2DataPoint(-100, -100, 0, 0., 0.){
	fStrip = -100;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor
DssdDataPoint::DssdDataPoint(int s, int b, int c, ullint t1, double t2, double e) : Numexo2DataPoint (b, c, t1, t2, e){
	fStrip = s;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
DssdDataPoint::~DssdDataPoint(){}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void DssdDataPoint::Clear(){ fStrip = -100; fBoard =-100; fChannel =-110; fTimeStamp=0; fCFDTime = 0.; fEnergy =0.;}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Retrun true if the fStrip is a front fStrip else return false.
bool DssdDataPoint::IsAFrontStrip()const{if(fStrip >= 0 && fStrip < 128)return true; else return false;}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Return true if the fStrip is a back fStrip else return false.
bool DssdDataPoint::IsABackStrip()const{if(fStrip > 127 && fStrip <=255)return true; else return false;}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Check if the fStrip is a neighboring one with respect to the dssdDataPoint d.
bool DssdDataPoint::IsANeighboringStripOf(DssdDataPoint &d){
	if(fStrip == d.fStrip+1 || fStrip == d.fStrip-1)return true; else return false;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
