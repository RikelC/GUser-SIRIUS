/*!
 * \file TunnelEvent.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#include "TunnelEvent.h"
//! Constructor
TunnelEvent::TunnelEvent(){
	fPixel.Clear(); 
	fTimeStamp = 0; 
	fEnergy = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
////! Constructor
TunnelEvent::TunnelEvent(TunnelMacroPixel p, ullint t,double e){
	fPixel = p;
        fTimeStamp = t;
	fEnergy = e;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
TunnelEvent::~TunnelEvent(){}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void TunnelEvent::Clear(){
	fPixel.Clear(); fTimeStamp =0; fEnergy = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
