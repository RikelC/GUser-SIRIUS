/*! \file Numexo2DataPoint.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 */
#pragma once

#ifndef Numexo2DataPoint_h
#define Numexo2DataPoint_h 1
#include "UTypes.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class Numexo2DataPoint
{
	protected:
		int fBoard; /*!< Strip number*/
		int fChannel; /*!< Strip number*/
		ullint fTimeStamp;/*< Time of the event */
		double fCFDTime;/*< Time of the event */
		double fEnergy;/*< Energy recorded in this strip*/
	public:
		Numexo2DataPoint();
		Numexo2DataPoint(int b, int c, ullint t1, double t2, double e);
		~ Numexo2DataPoint();
		//setters	
		void SetBoard(int b){ fBoard = b;} 
		void SetChannel(int c){ fChannel = c;} 
		void SetTimeStamp(ullint t){fTimeStamp =t;}
		void SetCFDTime(double t){fCFDTime =t;}
		void SetEnergy(double e){fEnergy = e;}
		//getters	
		int GetBoard()const { return fBoard;}
		int GetChannel()const { return fChannel;}
		ullint GetTimeStamp() const {return fTimeStamp;}
		double GetCFDTime() const {return fCFDTime;}
		double GetEnergy() const {return fEnergy;}

                int* GetBoardAddress(){ return &fBoard;}
		int* GetChannelAddress(){ return &fChannel;}
		ullint* GetTimeStampAddress() {return &fTimeStamp;}
		double* GetCFDTimeAddress() {return &fCFDTime;}
		double* GetEnergyAddrress()  {return &fEnergy;}


		virtual	void Clear();

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
