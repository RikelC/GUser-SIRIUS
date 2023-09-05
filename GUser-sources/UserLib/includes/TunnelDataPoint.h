/*! \file TunnelDataPoint.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 */
#pragma once

#ifndef TunnelDataPoint_h
#define TunnelDataPoint_h 1
#include "UTypes.h"
#include "Numexo2DataPoint.h"
#include "TunnelMacroPixel.h"
#include "TunnelData.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class TunnelDataPoint : public Numexo2DataPoint
{
	private:
		TunnelMacroPixel fMacroPixel; /*!< Macro pixel*/
		double fNeighborhoodness;
	public:
		TunnelDataPoint();
		TunnelDataPoint(TunnelMacroPixel s, ullint t, double e);
		~ TunnelDataPoint();
		//setters	
		void SetMacroPixel(TunnelMacroPixel s){ fMacroPixel = s;} 
		void SetValues(TunnelData * const data){
			fMacroPixel = data->GetMacroPixel();
			fTimeStamp = data->GetTimeStamp();
			fEnergy = data->GetCalibratedEnergy();
		}
		//getters	
		TunnelMacroPixel GetMacroPixel()const { return fMacroPixel;}

		void Clear();
		/*! get the current  macro pixel's fNeighborhoodness with respect to another macropixel.
		*/
		double GetNeighborhoodness(TunnelMacroPixel *l);
		/*! get the current  macro pixel's fNeighborhoodness with respect to another macropixel.
		*/
		double GetNeighborhoodness(TunnelDataPoint *l);
		double GetNeighborhoodness() const {return fNeighborhoodness;}

		friend bool CompareEnergy(TunnelDataPoint const & i, TunnelDataPoint const & j);
		friend bool CompareTime(TunnelDataPoint const & i, TunnelDataPoint const & j);

		TunnelDataPoint& operator=(const TunnelDataPoint& other){
			if(this==&other)return *this;
			fMacroPixel = other.GetMacroPixel();
			fNeighborhoodness = other.GetNeighborhoodness();
			return *this;
		}
		inline bool operator==(const TunnelDataPoint& other){
			if(this==&other)return true;
			if( fMacroPixel == other.GetMacroPixel()
					&& fNeighborhoodness == other.GetNeighborhoodness())
				return true;
			else return false;
		}

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
