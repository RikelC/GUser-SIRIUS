/*!
 * \file tunnelEvent.h
 * \author Rikel CHAKMA
 * \brief Building of tunnel events
 * \details
 *
 *
 */


#ifndef tunnelEvent_h
#define tunnelEvent_h 1

#include "UTypes.h"
#include "global.h"
#include "TMath.h"
#include "tunnelDataPoint.h"
#include "tunnelMacroPixel.h"
#include "TH1.h"
#include "TH2.h"
#include <vector>
#include <algorithm>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class tunnelEvent
{
	private:
		myGlobal *s1;
		llint td;
	public:

		tunnelEvent();
		~tunnelEvent();
		void construct(std::vector<tunnelDataPoint> &dataSet, TH2 *h1, TH2 *h2, TH2 *h3, TH2 *h4, TH1 * hdt1, TH1* hdt2, TH1* hdt3, TH1* hdt4);

};
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
#endif
