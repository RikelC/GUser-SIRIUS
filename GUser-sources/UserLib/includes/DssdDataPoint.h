/*! \file DssdDataPoint.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 */
#pragma once

#ifndef DssdDataPoint_h
#define DssdDataPoint_h 1
#include "UTypes.h"
#include "Numexo2DataPoint.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class DssdDataPoint : public Numexo2DataPoint
{
	private:
		int fStrip; /*!< Strip number*/
	public:
		DssdDataPoint();
		DssdDataPoint(int s, int b, int c, ullint t1, double t2, double e);
		~ DssdDataPoint();


		void SetStrip(int s){ fStrip = s;} 
		int GetStrip()const { return fStrip;}
		void Clear();
		bool IsAFrontStrip()const;
		bool IsABackStrip()const;
		bool IsANeighboringStripOf(DssdDataPoint &d);
		friend bool CompareEnergy(DssdDataPoint const & i, DssdDataPoint const & j);
		friend bool CompareTime(DssdDataPoint const & i, DssdDataPoint const & j);

		DssdDataPoint& operator=(const DssdDataPoint& other){
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

		inline bool operator==(const DssdDataPoint& other){
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
