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
		const int unFoundedStrip = -100;
		int maxTimeWindow;
		std::vector<DssdDataPoint>unCorrelatedDataSet;
		std::vector<DssdDataPoint>tempEvent;
		std::vector<DssdDataPoint>tempEventF;
		std::vector<DssdDataPoint>tempEventB;
		std::vector<DssdDataPoint>myEvent;
		std::vector<DssdDataPoint>myEvent_front;
		std::vector<DssdDataPoint>myEvent_back;
		void AddPreviousUncorrelatedDataPoints(std::vector<DssdDataPoint> &dataSet1, std::vector<DssdDataPoint> &dataSet2);
	public:

		MakeDssdEvents();
		~MakeDssdEvents();
		std::vector<DssdEvent> Construct(std::string mode, std::vector<DssdDataPoint> &dataSet,TGraph* gr1, TGraph* gr, TGraph *gr2);
		std::vector<DssdEvent> Construct(std::string mode, std::vector<DssdDataPoint> &dataSet, TH1 *h_ff, TH1 *h_fb, TH1 *h_bf, TH1 *h_bb);
		std::vector<DssdEvent> Construct(std::string mode, std::vector<DssdDataPoint> &dataSet);
		std::vector<DssdEvent> GetUnCorrelatedEvents(std::string mode);
		DssdEvent DetermineDssdEvent(std::vector<DssdDataPoint> &myEvent);
		DssdEvent DetermineDssdFrontEvent(std::vector<DssdDataPoint> &myEvent);
		DssdEvent DetermineDssdBackEvent(std::vector<DssdDataPoint> &myEvent);
		void SortInTime(std::vector<DssdDataPoint> &dataSet);
		void SortInTime(std::vector<DssdEvent> &dataSet);
};
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
#endif
