/*! \file Numexo2Data.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */

#include "Numexo2Data.h"
//! Constructor
Numexo2Data::Numexo2Data(){
	s1 = MyGlobal::GetInstance();
	fBoard = -1;
	fBoardIndex = -1;
	fChannel = -1;
	fEventNumber = 0;
	fTimeStamp = 0;
	fRawEnergy = 0.;
	fCalibratedEnergy = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
Numexo2Data::~Numexo2Data(){
}


