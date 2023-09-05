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

		TrackerNumexo2DataPoint& operator=(const TrackerNumexo2DataPoint& other){
			if(this == &other) return *this;
			fStrip = other.GetStrip();
			fBoard = other.GetBoard(); 
			fBoardIndex = other.GetBoardIndex();
			fChannel = other.GetChannel(); 
			fTimeStamp = other.GetTimeStamp();
			fCFDTime = other.GetCFDTime();
			fEnergy = other.GetEnergy();
			for(int i =0; i <992;i++) fTrace[i] = other.GetTraceValue(i);

			return *this;
		}
		inline bool operator==(const TrackerNumexo2DataPoint& other){
			if(this == &other) return true;
			if( fStrip == other.GetStrip()
					&& fBoard == other.GetBoard()
					&& fBoardIndex == other.GetBoardIndex()
					&& fChannel == other.GetChannel()
					&& fTimeStamp == other.GetTimeStamp()
					&& fCFDTime == other.GetCFDTime()
					&& fEnergy == other.GetEnergy())
				return true;
			else return false;
		}


};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
