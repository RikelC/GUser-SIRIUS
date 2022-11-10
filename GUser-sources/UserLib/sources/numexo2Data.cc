/*! \file numexo2Data.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */

#include "numexo2Data.h"
//! Constructor
numexo2Data::numexo2Data(){
	s1 = myGlobal::getInstance();
	boardID = -1;
	boardIdx = -1;
	channelID = -1;
	eventnumber = 0;
	timestamp = 0;
	raw_energy = 0.;
	calibrated_energy = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
numexo2Data::~numexo2Data(){
}


