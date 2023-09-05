/*! \file Numexo2DataPoint.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 */
#pragma once

#ifndef Numexo2DataPoint_h
#define Numexo2DataPoint_h 1
#include "UTypes.h"
#include <algorithm>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class Numexo2DataPoint
{
	protected:
		int fBoard; /*!< Strip number*/
		int fBoardIndex; /*!< Strip number*/
		int fChannel; /*!< Strip number*/
		ullint fTimeStamp;/*< Time of the event */
		double fCFDTime;/*< Time of the event */
		double fEnergy;/*< Energy recorded in this strip*/
		ushort fTrace[992];
	public:
		Numexo2DataPoint();
		Numexo2DataPoint(int b, int c, ullint t1, double t2, double e);
		~ Numexo2DataPoint();
		//setters	
		void SetBoard(int b){ fBoard = b;} 
		void SetBoardIndex(int i){ fBoardIndex = i;} 
		void SetChannel(int c){ fChannel = c;} 
		void SetTimeStamp(ullint t){fTimeStamp =t;}
		void SetCFDTime(double t){fCFDTime =t;}
		void SetEnergy(double e){fEnergy = e;}
		//void SetTrace(ushort *v){for(int i=0; i<s1->TRACE_SIZE;i++)fTrace[i]=v[i];}
		void SetTrace(ushort *v, int size){std::copy(v, v+size, fTrace);}
		void SetTraceValue(int i, ushort v){fTrace[i] = v;}
		ushort GetTraceValue(int i) const {return fTrace[i];}
		//getters	
		int GetBoard()const { return fBoard;}
		int GetBoardIndex()const { return fBoardIndex;}
		int GetChannel()const { return fChannel;}
		ullint GetTimeStamp() const {return fTimeStamp;}
		double GetCFDTime() const {return fCFDTime;}
		double GetEnergy() const {return fEnergy;}

		ushort *GetTrace() {return fTrace;}
		ushort GetTraceValue(int i) {return fTrace[i];}
		int* GetBoardAddress(){ return &fBoard;}
		int* GetChannelAddress(){ return &fChannel;}
		ullint* GetTimeStampAddress() {return &fTimeStamp;}
		double* GetCFDTimeAddress() {return &fCFDTime;}
		double* GetEnergyAddrress()  {return &fEnergy;}


		virtual	void Clear();

		Numexo2DataPoint& operator=(const Numexo2DataPoint& other){
			if(this == &other) return *this;
			fBoard = other.GetBoard(); 
			fBoardIndex = other.GetBoardIndex();
			fChannel = other.GetChannel(); 
			fTimeStamp = other.GetTimeStamp();
			fCFDTime = other.GetCFDTime();
			fEnergy = other.GetEnergy();
			for(int i =0; i <992;i++) fTrace[i] = other.GetTraceValue(i);

			return *this;
		}

		inline bool operator==(const Numexo2DataPoint& other){
			if(this == &other) return true;
			if( fBoard == other.GetBoard()
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
