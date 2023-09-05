/*!
 * \file TunnelEvent.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#pragma once

#ifndef TunnelEvent_h
#define TunnelEvent_h 1
#include "UTypes.h"
#include "TunnelMacroPixel.h"

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class TunnelEvent
{
	private:

		TunnelMacroPixel fPixel;
		ullint fTimeStamp;
		double fEnergy;

	public:
		TunnelEvent();
		TunnelEvent(TunnelMacroPixel p, ullint t, double e);
		~TunnelEvent();
		void SetMacroPixel(TunnelMacroPixel p){fPixel = p;}
		void SetTimeStamp(ullint t){fTimeStamp = t;}
		void SetEnergy(double e){fEnergy = e;}

		TunnelMacroPixel GetMacroPixel() const {return fPixel;}
		ullint GetTimeStamp() const {return fTimeStamp;}
		double GetEnergy() const {return fEnergy;}

		TunnelMacroPixel* GetMacroPixel() {return &fPixel;}
		ullint* GetTimeStampAddress() {return &fTimeStamp;}
		double* GetEnergyAddress() {return &fEnergy;}


		void Clear();
		friend bool CompareTimeEvent(TunnelEvent const & i, TunnelEvent const & j);

		TunnelEvent& operator=(const TunnelEvent& other){
			if(this ==&other) return *this;
			fPixel = other.GetMacroPixel();
			fTimeStamp = other.GetTimeStamp();
			fEnergy = other.GetEnergy();
			return *this;
		} 
		inline bool operator==(const TunnelEvent& other){
			if(this ==&other) return true;
			if( fPixel == other.GetMacroPixel()
					&& fTimeStamp == other.GetTimeStamp()
					&& fEnergy == other.GetEnergy())
				return true;
			else return false;
		} 
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
