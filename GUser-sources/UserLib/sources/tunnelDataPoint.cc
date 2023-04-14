/*! \file tunnelDataPoint.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 */

#include "tunnelDataPoint.h"
//! Constructor
tunnelDataPoint::tunnelDataPoint(){
	macroPixel.clear();
	time = 0;
	energy =-100.;
neighborhoodness =0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor
tunnelDataPoint::tunnelDataPoint(tunnelMacroPixel s, ullint t, double e){
	macroPixel = s;
	time = t;
	energy = e;
neighborhoodness =0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
tunnelDataPoint::~tunnelDataPoint(){}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void tunnelDataPoint::clear(){macroPixel.clear(); time = 0; energy =-100.; neighborhoodness =0;}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

/*! get the current  macro pixel's neighborhoodness with respect to another macropixel.
*/
float tunnelDataPoint::get_neighborhoodness(tunnelMacroPixel *l){
	int dp = 0;
	int prevN = l->get_N();
	int currentN = macroPixel.get_N();
	if(currentN >=0 && currentN< 8){
		if(prevN >= 0 && prevN < 8){
			dp = std::abs(currentN - prevN);
			if(dp == 1) neighborhoodness =1.;
			else if (dp == 4) neighborhoodness = 2.;
			else if (dp == 5) neighborhoodness = 0.5;
			else if (dp == 3) neighborhoodness = 0.5;
			else neighborhoodness = 0.;
		}
		else if(prevN >7 && prevN < 16){
			if(currentN > 3 && currentN < 8){
				dp = prevN%currentN;
				if(dp == 0 || dp == 1) neighborhoodness = 1.;
				else if(dp == 2) neighborhoodness = 0.5;
				else if (dp >3 && dp < 7 ) neighborhoodness = 0.5;
				else neighborhoodness = 0.;
			}
			else neighborhoodness =0.;
		}
	}
	else if(currentN > 7 && currentN < 24){
		if(prevN > 7 && prevN < 24){
			dp = std::abs(currentN - prevN);
			if(dp == 1) neighborhoodness = 3.;
			else if(dp == 8) neighborhoodness = 1.;
			else if(dp == 9) neighborhoodness = 0.5;
			else if (dp == 7) neighborhoodness = 0.5;
			else neighborhoodness = 0.;
		}
		else if(prevN > 3 && prevN < 8){
			if(currentN > 7 && currentN < 16){
				dp = currentN%prevN;
				if(dp == 0 || dp == 1) neighborhoodness = 1.;
				else if(dp == 2) neighborhoodness = 0.5;
				else if (dp >3 && dp < 7 ) neighborhoodness = 0.5;
				else neighborhoodness = 0.;

			}
			else neighborhoodness = 0.;
		}
	}

	return neighborhoodness;
}

/*! get the current  macro pixel's neighborhoodness with respect to another macropixel.
*/
float tunnelDataPoint::get_neighborhoodness(tunnelDataPoint *l){
	int dp = 0;
	int prevN = l->get_macroPixel().get_N();
	int currentN = macroPixel.get_N();
	if(currentN >=0 && currentN< 8){
		if(prevN >= 0 && prevN < 8){
			dp = std::abs(currentN - prevN);
			if(dp == 1) neighborhoodness =1.;
			else if (dp == 4) neighborhoodness = 2.;
			else if (dp == 5) neighborhoodness = 0.5;
			else if (dp == 3) neighborhoodness = 0.5;
			else neighborhoodness = 0.;
		}
		else if(prevN >7 && prevN < 16){
			if(currentN > 3 && currentN < 8){
				dp = prevN%currentN;
				if(dp == 0 || dp == 1) neighborhoodness = 1.;
				else if(dp == 2) neighborhoodness = 0.5;
				else if (dp >3 && dp < 7 ) neighborhoodness = 0.5;
				else neighborhoodness = 0.;
			}
			else neighborhoodness =0.;
		}
	}
	else if(currentN > 7 && currentN < 24){
		if(prevN > 7 && prevN < 24){
			dp = std::abs(currentN - prevN);
			if(dp == 1) neighborhoodness = 3.;
			else if(dp == 8) neighborhoodness = 1.;
			else if(dp == 9) neighborhoodness = 0.5;
			else if (dp == 7) neighborhoodness = 0.5;
			else neighborhoodness = 0.;
		}
		else if(prevN > 3 && prevN < 8){
			if(currentN > 7 && currentN < 16){
				dp = currentN%prevN;
				if(dp == 0 || dp == 1) neighborhoodness = 1.;
				else if(dp == 2) neighborhoodness = 0.5;
				else if (dp >3 && dp < 7 ) neighborhoodness = 0.5;
				else neighborhoodness = 0.;

			}
			else neighborhoodness = 0.;
		}
	}

	return neighborhoodness;
}





//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
