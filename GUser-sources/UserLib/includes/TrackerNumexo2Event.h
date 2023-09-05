/*!
 * \file TrackerNumexo2Event.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */
#pragma once

#ifndef TrackerNumexo2Event_h
#define TrackerNumexo2Event_h 1
#include "UTypes.h"
#include <vector>
#include <algorithm>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class TrackerNumexo2Event
{
	private:
		double fCFDTime;
		ullint fTimeStamp;
		double fEnergy;
		ushort fTrace[992];

	public:
		TrackerNumexo2Event();
		TrackerNumexo2Event(ullint t1, double t2, double e);
		~TrackerNumexo2Event();
		void SetCFDTime(double t){fCFDTime = t;}
		void SetTimeStamp(ullint t){fTimeStamp = t;}
		void SetEnergy(double e){fEnergy = e;}
		double GetCFDTime() const {return fCFDTime;}
		ullint GetTimeStamp() const {return fTimeStamp;}
		double GetEnergy() const {return fEnergy;}
		ushort GetTraceValue(int i)const {return fTrace[i];}
		void SetTrace(const ushort *v, int size){std::copy(v, v+size, fTrace);}
		void SetTraceValue(int i, ushort v){fTrace[i] = v;}
		double * GetAddressOfCFDTime() {return &fCFDTime;}
		ullint * GetAddressOfTimeStamp() {return &fTimeStamp;}
		double * GetAddressOfEnergy() {return &fEnergy;}
		ushort * GetTrace() {return fTrace;}
		ushort * GetAddressOfTrace() {return fTrace;}


		void Clear();

		friend bool CompareTime(TrackerNumexo2Event const & i, TrackerNumexo2Event const & j);
		friend bool CompareTimeTrackerNumexo2Event(TrackerNumexo2Event const & i, TrackerNumexo2Event const & j);

		void SortInTime(std::vector<TrackerNumexo2Event> &dataSet);

		TrackerNumexo2Event& operator=(const TrackerNumexo2Event& other){

			if(this == &other) return *this;

			fTimeStamp = other.GetTimeStamp();
			fCFDTime = other.GetCFDTime();
			fEnergy = other.GetEnergy();
			for(int i =0; i < 992; i++)fTrace[i]=other.GetTraceValue(i);
			return *this;
		}
		inline bool operator==(const TrackerNumexo2Event& other){

			if(this == &other) return true;

			if( fTimeStamp == other.GetTimeStamp()
					&& fCFDTime == other.GetCFDTime()
					&& fEnergy == other.GetEnergy())
				return true;
			else return false;
		}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
