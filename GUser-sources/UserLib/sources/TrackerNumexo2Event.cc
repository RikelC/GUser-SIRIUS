/*!
 * \file TrackerNumexo2Event.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#include "TrackerNumexo2Event.h"
//! Constructor
TrackerNumexo2Event::TrackerNumexo2Event(){
	fTimeStamp = 0; 
	fCFDTime = 0.; 
	fEnergy = 0.;
	for(int i=0;i<992;i++)fTrace[i]=0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
////! Constructor
TrackerNumexo2Event::TrackerNumexo2Event( ullint t1, double t2, double e){
	fTimeStamp = t1;
	fCFDTime = t2; 
	fEnergy = e;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------


//! Destructor
TrackerNumexo2Event::~TrackerNumexo2Event(){}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void TrackerNumexo2Event::Clear(){
	fTimeStamp =0; fCFDTime = 0.; fEnergy = 0.; 
	for(int i=0;i<992;i++)fTrace[i]=0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in time
bool CompareTime(TrackerNumexo2Event const & i, TrackerNumexo2Event const & j)
{
	return std::tie(i.fTimeStamp, i.fEnergy) < std::tie(j.fTimeStamp, j.fEnergy);
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

void TrackerNumexo2Event::SortInTime(std::vector<TrackerNumexo2Event> &dataSet){
	std::sort(dataSet.begin(), dataSet.end(),CompareTime);
}

