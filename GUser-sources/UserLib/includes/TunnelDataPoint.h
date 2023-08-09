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
		float fNeighborhoodness;
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
		float GetNeighborhoodness(TunnelMacroPixel *l);
		/*! get the current  macro pixel's fNeighborhoodness with respect to another macropixel.
		*/
		float GetNeighborhoodness(TunnelDataPoint *l);

		friend bool CompareEnergy(TunnelDataPoint const & i, TunnelDataPoint const & j);
		friend bool CompareTime(TunnelDataPoint const & i, TunnelDataPoint const & j);

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
