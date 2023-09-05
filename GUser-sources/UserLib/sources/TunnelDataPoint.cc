/*! \file TunnelDataPoint.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 */

#include "TunnelDataPoint.h"
//! Constructor
TunnelDataPoint::TunnelDataPoint() : Numexo2DataPoint(-100, -100, 0, 0., 0.){
	fMacroPixel.Clear();
	fNeighborhoodness =0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor
TunnelDataPoint::TunnelDataPoint(TunnelMacroPixel s, ullint t, double e) : Numexo2DataPoint(s.GetBoard(), s.GetChannel(), t, (double)t, e){
	fMacroPixel = s;
	fNeighborhoodness =0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
TunnelDataPoint::~TunnelDataPoint(){}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void TunnelDataPoint::Clear(){fMacroPixel.Clear(); fBoard = -100, fChannel = -100; fTimeStamp =0; fCFDTime = 0.; fEnergy =-100.; fNeighborhoodness =0;}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

/*! get the current  macro pixel's fNeighborhoodness with respect to another macropixel.
*/
double TunnelDataPoint::GetNeighborhoodness(TunnelMacroPixel *l){
	int dp = 0;
	int prevN = l->GetN();
	int currentN = fMacroPixel.GetN();
	if(currentN >=0 && currentN< 8){
		if(prevN >= 0 && prevN < 8){
			dp = std::abs(currentN - prevN);
			if(dp == 1) fNeighborhoodness =1.;
			else if (dp == 4) fNeighborhoodness = 2.;
			else if (dp == 5) fNeighborhoodness = 0.5;
			else if (dp == 3) fNeighborhoodness = 0.5;
			else fNeighborhoodness = 0.;
		}
		else if(prevN >7 && prevN < 16){
			if(currentN > 3 && currentN < 8){
				dp = prevN%currentN;
				if(dp == 0 || dp == 1) fNeighborhoodness = 1.;
				else if(dp == 2) fNeighborhoodness = 0.5;
				else if (dp >3 && dp < 7 ) fNeighborhoodness = 0.5;
				else fNeighborhoodness = 0.;
			}
			else fNeighborhoodness =0.;
		}
	}
	else if(currentN > 7 && currentN < 24){
		if(prevN > 7 && prevN < 24){
			dp = std::abs(currentN - prevN);
			if(dp == 1) fNeighborhoodness = 3.;
			else if(dp == 8) fNeighborhoodness = 1.;
			else if(dp == 9) fNeighborhoodness = 0.5;
			else if (dp == 7) fNeighborhoodness = 0.5;
			else fNeighborhoodness = 0.;
		}
		else if(prevN > 3 && prevN < 8){
			if(currentN > 7 && currentN < 16){
				dp = currentN%prevN;
				if(dp == 0 || dp == 1) fNeighborhoodness = 1.;
				else if(dp == 2) fNeighborhoodness = 0.5;
				else if (dp >3 && dp < 7 ) fNeighborhoodness = 0.5;
				else fNeighborhoodness = 0.;

			}
			else fNeighborhoodness = 0.;
		}
	}

	return fNeighborhoodness;
}

/*! get the current  macro pixel's fNeighborhoodness with respect to another macropixel.
*/
double TunnelDataPoint::GetNeighborhoodness(TunnelDataPoint *l){
	int dp = 0;
	int prevN = l->GetMacroPixel().GetN();
	int currentN = fMacroPixel.GetN();
	if(currentN >=0 && currentN< 8){
		if(prevN >= 0 && prevN < 8){
			dp = std::abs(currentN - prevN);
			if(dp == 1) fNeighborhoodness =1.;
			else if (dp == 4) fNeighborhoodness = 2.;
			else if (dp == 5) fNeighborhoodness = 0.5;
			else if (dp == 3) fNeighborhoodness = 0.5;
			else fNeighborhoodness = 0.;
		}
		else if(prevN >7 && prevN < 16){
			if(currentN > 3 && currentN < 8){
				dp = prevN%currentN;
				if(dp == 0 || dp == 1) fNeighborhoodness = 1.;
				else if(dp == 2) fNeighborhoodness = 0.5;
				else if (dp >3 && dp < 7 ) fNeighborhoodness = 0.5;
				else fNeighborhoodness = 0.;
			}
			else fNeighborhoodness =0.;
		}
	}
	else if(currentN > 7 && currentN < 24){
		if(prevN > 7 && prevN < 24){
			dp = std::abs(currentN - prevN);
			if(dp == 1) fNeighborhoodness = 3.;
			else if(dp == 8) fNeighborhoodness = 1.;
			else if(dp == 9) fNeighborhoodness = 0.5;
			else if (dp == 7) fNeighborhoodness = 0.5;
			else fNeighborhoodness = 0.;
		}
		else if(prevN > 3 && prevN < 8){
			if(currentN > 7 && currentN < 16){
				dp = currentN%prevN;
				if(dp == 0 || dp == 1) fNeighborhoodness = 1.;
				else if(dp == 2) fNeighborhoodness = 0.5;
				else if (dp >3 && dp < 7 ) fNeighborhoodness = 0.5;
				else fNeighborhoodness = 0.;

			}
			else fNeighborhoodness = 0.;
		}
	}

	return fNeighborhoodness;
}





//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
