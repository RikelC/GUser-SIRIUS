/*! \file TunnelPixel.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 * */

#include "TunnelPixel.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor
/*! Here, one initializes the variables with negative values.
*/
TunnelPixel::TunnelPixel(){fN = -100; fY = -100, fX = -100;}
//! Constructor
/*! Here, one initializes the variable fX and fY according to the parameter passed. fX is computed as fX = fN/8 and fY is computed as fY = fN%8.
*/
TunnelPixel::TunnelPixel(int n){fN = n; fY = int(n/8), fX = (n%8);}
/*! Destructor
*/	
TunnelPixel::~TunnelPixel(){}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
