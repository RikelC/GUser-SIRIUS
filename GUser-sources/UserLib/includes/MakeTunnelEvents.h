/*!
 * \file MakeTunnelEvents.h
 * \author Rikel CHAKMA
 * \brief Building of tunnel events
 * \details
 *
 *
 */

#pragma once

#ifndef MakeTunnelEvents_h
#define MakeTunnelEvents_h 1

#include "UTypes.h"
#include "Global.h"
#include "TMath.h"
#include "TunnelDataPoint.h"
#include "TunnelMacroPixel.h"
#include "TH1.h"
#include "TH2.h"
#include <vector>
#include <algorithm>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class MakeTunnelEvents
{
	private:
		MyGlobal *s1;
		llint td;
	public:

		MakeTunnelEvents();
		~MakeTunnelEvents();
		void Construct(std::vector<TunnelDataPoint> &dataSet, TH2 *h1, TH2 *h2, TH2 *h3, TH2 *h4, TH1 * hdt1, TH1* hdt2, TH1* hdt3, TH1* hdt4);

};
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
#endif
