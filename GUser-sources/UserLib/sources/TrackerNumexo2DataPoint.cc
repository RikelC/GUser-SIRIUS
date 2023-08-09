/*! \file TrackerNumexo2DataPoint.cc
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 */

#include "TrackerNumexo2DataPoint.h"
//! Constructor
TrackerNumexo2DataPoint::TrackerNumexo2DataPoint() : Numexo2DataPoint(-100, -100, 0, 0., 0.) {
	fStrip = -100;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor
TrackerNumexo2DataPoint::TrackerNumexo2DataPoint(int s, int b, int c, ullint t1, double t2, double e) :  Numexo2DataPoint(b, c, t1, t2, e){
	fStrip = s;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//! Destructor
TrackerNumexo2DataPoint::~TrackerNumexo2DataPoint(){}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void TrackerNumexo2DataPoint::Clear(){ fStrip = -100; fBoard =-100; fChannel =-110; fTimeStamp=0; fCFDTime = 0.; fEnergy =0.;}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in time
bool CompareTime(TrackerNumexo2DataPoint const & i, TrackerNumexo2DataPoint const & j)
{
	return std::tie(i.fTimeStamp,  i.fStrip, i.fEnergy) < std::tie(j.fTimeStamp, j.fStrip, j.fEnergy);
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

void TrackerNumexo2DataPoint::SortInTime(std::vector<TrackerNumexo2DataPoint> &dataSet){
	std::sort(dataSet.begin(), dataSet.end(),CompareTime);
}

