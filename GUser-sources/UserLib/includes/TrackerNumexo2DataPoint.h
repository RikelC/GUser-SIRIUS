/*! \file TrackerNumexo2DataPoint.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 */

#pragma once

#ifndef TrackerNumexo2DataPoint_h
#define TrackerNumexo2DataPoint_h 1
#include "UTypes.h"
#include "Numexo2DataPoint.h"
#include <vector>
#include <algorithm>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class TrackerNumexo2DataPoint : public Numexo2DataPoint
{
	private:
		int fStrip; /*!< Strip number*/
	public:
		TrackerNumexo2DataPoint();
		TrackerNumexo2DataPoint(int s, int b, int c, ullint t1, double t2, double e);
		~ TrackerNumexo2DataPoint();
                void Clear();
                void SetStrip(int s){ fStrip = s;} 
		int GetStrip()const { return fStrip;}
		friend bool CompareTime(TrackerNumexo2DataPoint const & i, TrackerNumexo2DataPoint const & j);
		void SortInTime(std::vector<TrackerNumexo2DataPoint> &dataSet);
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
