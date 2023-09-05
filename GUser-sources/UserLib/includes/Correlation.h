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
#include "TrackerNumexo2Event.h"
#define MAX_TIME_WINDOW 500 //5 us

using namespace std;
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class Correlation
{
	private:
		MyGlobal *s1; /*!< pointer to the instance of the myGlobal singleton class. */ 

		std::vector<RecoilEvent> RecoilEventVec;
		std::vector<DecayEvent> DecayEventVec;
		std::vector<DssdEvent> unCorrelatedDssd;
		std::vector<TrackerNumexo2Event> unCorrelatedTrackerNumexo2;
		std::vector<TrackerCoBoEvent> unCorrelatedTrackerCobo;
		std::vector<DssdEvent> uDssd;
		std::vector<TrackerNumexo2Event> uTrackerNumexo2;
		std::vector<TrackerCoBoEvent> uTrackerCobo;
	public:
		Correlation();
		~Correlation();	
		void Find(std::vector<DssdEvent> &dssd, std::vector<TrackerNumexo2Event> &tracker);
		void Find(std::vector<DssdEvent> &dssd, std::vector<TrackerCoBoEvent> &tracker);
		void Find(std::vector<DssdEvent> &dssd, std::vector<TrackerNumexo2Event> &trackerNumexo, std::vector<TrackerCoBoEvent> &trackerCobo, bool reached_eof);
		std::vector<DssdEvent> GetUnCorrelatedDssdEvents();

		std::vector<TrackerNumexo2Event> GetUnCorrelatedTackerNumexo2Events();
		std::vector<TrackerCoBoEvent> GetUnCorrelatedTackerCoBoEvents();

		void Clear();
		//void AssociateWithRecoils(std::vector<TrackerCoBoEvent> &tracker);
		//void AssociateWithRecoils(std::vector<TrackerNumexo2Event> &tracker);
		std::vector<RecoilEvent> GetRecoils() {return RecoilEventVec;}
		std::vector<DecayEvent> GetDecays() {return DecayEventVec;}
		void SortInTime(std::vector<DssdEvent> &dataSet);
		void SortInTime(std::vector<TrackerCoBoEvent> &dataSet);
		void SortInTime(std::vector<TrackerNumexo2Event> &dataSet);
};
#endif
