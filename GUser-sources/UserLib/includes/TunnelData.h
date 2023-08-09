/*! \file TunnelData.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */
#pragma once

#ifndef TunnelData_h
#define TunnelData_h 1
#include "Numexo2Data.h"
#include "TunnelPixel.h"
#include "TunnelMacroPixel.h"
#include "TMath.h"
#include <vector>
#include <map>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class TunnelData : public Numexo2Data
{
	private:
		std::map<std::pair<int, int >, TunnelMacroPixel > fMacroPixelMap;
		void MapDetectorMacroPixels();
	public:
		TunnelData();
		~TunnelData();	
		TunnelMacroPixel GetMacroPixel();	
		TunnelMacroPixel GetMacroPixel(int * const, int * const);	

		char GetMacroPixelName(int * const p_board, int * const p_channel);
		int GetMacroPixelPhysicalNumber(int * const p_board, int * const p_channel);
		int GetTunnelDetectorNumber(int * const p_board, int * const p_channel);
		int GetTunnelBoardNumber(int * const p_board);

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
