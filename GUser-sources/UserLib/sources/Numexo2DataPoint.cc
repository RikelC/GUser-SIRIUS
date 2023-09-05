/*! \file Numexo2DataPoint.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 */

#include "Numexo2DataPoint.h"
//! Constructor
Numexo2DataPoint::Numexo2DataPoint(){
	fBoard = -100;
	fChannel = -100;
	fTimeStamp = 0;
	fCFDTime = 0;
	fEnergy =0.;
	for(int i =0; i< 992;i++)fTrace[i]=0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor
Numexo2DataPoint::Numexo2DataPoint(int b, int c, ullint t1, double t2, double e){
	fBoard = b;
	fChannel = c;
	fTimeStamp = t1;
	fCFDTime = t2;
	fEnergy =e;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
Numexo2DataPoint::~Numexo2DataPoint(){}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void Numexo2DataPoint::Clear(){

	for(int i =0; i< 992;i++)fTrace[i]=0;
	fBoard =-100; fChannel =-110; fTimeStamp=0; fCFDTime = 0.; fEnergy =0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
