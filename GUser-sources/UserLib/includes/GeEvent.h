/*!
 * \file GeEvent.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#pragma once

#ifndef GeEvent_h
#define GeEvent_h 1
#include "UTypes.h"

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class GeEvent
{
	private:

		ullint fTimeStamp;
		double fEnergy;
                int fDetector;
	public:
		GeEvent();
		~GeEvent();
		void SetTimeStamp(ullint t){fTimeStamp = t;}
		void SetEnergy(double e){fEnergy = e;}
		void SetDetector(int n){fDetector = n;}

		ullint GetTimeStamp() const {return fTimeStamp;}
		double GetEnergy() const {return fEnergy;}
		double GetDetector() const {return fDetector;}

		ullint* GetTimeStampAddress() {return &fTimeStamp;}
		double* GetEnergyAddress() {return &fEnergy;}


		void Clear();
		friend bool CompareTimeEvent(GeEvent const & i, GeEvent const & j);
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
