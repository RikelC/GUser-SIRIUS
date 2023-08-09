/*!
 * \file MakeDssdEvents.h
 * \author Rikel CHAKMA
 * \brief Building of DSSD events
 * \details
 *
 *
 */

#pragma once

#ifndef MakeDssdEvents_h
#define MakeDssdEvents_h 1

#include "UTypes.h"
#include "Global.h"
#include "TMath.h"
#include "DssdDataPoint.h"
#include "DssdEvent.h"
#include "DssdPixel.h"
#include "TH1.h"
#include "TGraph.h"
#include <vector>
#include <algorithm>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class MakeDssdEvents
{
	private:
		bool fStartNewEvent;
		MyGlobal *s1;
		llint fTimeDifferenceFrontFront;
		llint fTimeDifferenceFrontBack;
		llint fTimeDifferenceBackFront;
		llint fTimeDifferenceBackBack;
		DssdEvent fEvtPoint;
		int fPixelCounter;	
		std::vector<DssdDataPoint>tempEvent;
		std::vector<DssdDataPoint>tempEventF;
		std::vector<DssdDataPoint>tempEventB;
		std::vector<DssdDataPoint>myEvent;
		std::vector<DssdDataPoint>myEvent_front;
		std::vector<DssdDataPoint>myEvent_back;
	public:

		MakeDssdEvents();
		~MakeDssdEvents();
		std::vector<DssdEvent> Construct(std::string mode, std::vector<DssdDataPoint> &dataSet,TGraph* gr1, TGraph* gr, TGraph *gr2);
		std::vector<DssdEvent> Construct(std::string mode, std::vector<DssdDataPoint> &dataSet, TH1 *h_ff, TH1 *h_fb, TH1 *h_bf, TH1 *h_bb);
		std::vector<DssdEvent> Construct(std::string mode, std::vector<DssdDataPoint> &dataSet);
		DssdEvent DetermineDssdEvent(std::vector<DssdDataPoint> &myEvent);
		DssdEvent DetermineDssdFrontEvent(std::vector<DssdDataPoint> &myEvent);
		DssdEvent DetermineDssdBackEvent(std::vector<DssdDataPoint> &myEvent);
		void SortInTime(std::vector<DssdDataPoint> &dataSet);
		void SortInTime(std::vector<DssdEvent> &dataSet);
};
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
#endif
