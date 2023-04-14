/*!
 * \file dssdEvent.h
 * \author Rikel CHAKMA
 * \brief Building of DSSD events
 * \details
 *
 *
 */


#ifndef dssdEvent_h
#define dssdEvent_h 1

#include "UTypes.h"
#include "global.h"
#include "TMath.h"
#include "dssdDataPoint.h"
#include "dssdPixel.h"
#include "TH1.h"
#include "TGraph.h"
#include <vector>
#include <algorithm>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class dssdEvent
{
	private:
		bool start_new_event;
		myGlobal *s1;
		llint td_ff;
		llint td_fb;
		llint td_bf;
		llint  td_bb;
		dssdPixel ePoint;
		int pixel_counter;	
		std::vector<dssdDataPoint>tempEvent;
		std::vector<dssdDataPoint>myEvent;
		std::vector<dssdDataPoint>myEvent_front;
		std::vector<dssdDataPoint>myEvent_back;
	public:

		dssdEvent();
		~dssdEvent();
		std::vector<dssdPixel> construct(std::vector<dssdDataPoint> &dataSet,TGraph* gr1, TGraph* gr, TGraph *gr2);
		std::vector<dssdPixel> construct(std::vector<dssdDataPoint> &dataSet, TH1 *h_ff, TH1 *h_fb, TH1 *h_bf, TH1 *h_bb);
		std::vector<dssdPixel> construct(std::vector<dssdDataPoint> &dataSet);
		dssdPixel determine_dssd_pixel(std::vector<dssdDataPoint> &myEvent);
void SortInTime(std::vector<dssdDataPoint> &dataSet);

};
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
#endif
