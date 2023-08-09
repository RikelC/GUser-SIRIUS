/*! \file Correlation.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */
#pragma once

#ifndef Correlation_h
#define Correlation_h 1
#include "Global.h"
#include "RecoilEvent.h"
#include "DecayEvent.h"
#include "UTypes.h"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class Correlation
{
	private:
		MyGlobal *s1; /*!< pointer to the instance of the myGlobal singleton class. */ 

		std::vector<RecoilEvent> RecoilEventVec;
		std::vector<DecayEvent> DecayEventVec;
	public:
		Correlation();
		~Correlation();	
		void Find(std::vector<DssdEvent> &dssd, std::vector<TrackerEvent> &tracker);
		std::vector<RecoilEvent> GetRecoils() const {return RecoilEventVec;}
		std::vector<DecayEvent> GetDecays() const {return DecayEventVec;}
		void SortInTime(std::vector<DssdEvent> &dataSet);
		void SortInTime(std::vector<TrackerEvent> &dataSet);
};
#endif
