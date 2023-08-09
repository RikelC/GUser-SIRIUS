/*! \file TunnelMacroPixel.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 * */

#include "TunnelMacroPixel.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

TunnelMacroPixel::TunnelMacroPixel(){fN = -100; fName = '?'; fDetector = -100; fBoard = -100; fChannel = -100;}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
/*! Destructor
*/		
TunnelMacroPixel::~TunnelMacroPixel(){if(pixels.size()>0)pixels.clear();}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void TunnelMacroPixel::Clear(){
	fN = -100;
	fDetector = -100;
	fBoard = -100;
	fChannel = -100;
	fName = '?';
	pixels.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
