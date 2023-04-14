/*! \file tunnelData.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */

#ifndef tunnelData_h
#define tunnelData_h 1
#include "numexo2Data.h"
#include "tunnelPixel.h"
#include "tunnelMacroPixel.h"
#include "TMath.h"
#include <vector>
#include <map>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class tunnelData : public numexo2Data
{
	private:
		std::map<std::pair<int, int >, tunnelMacroPixel > macroPixelMap;
		void map_detectorMacroPixels();
	public:
		tunnelData();
		~tunnelData();	
		tunnelMacroPixel get_macroPixel();	
		tunnelMacroPixel get_macroPixel(int * const, int * const);	

		char get_macroPixelName(int * const p_board, int * const p_channel);
		int get_macroPixelPhysicalNumber(int * const p_board, int * const p_channel);
		int get_tunnelDetectorNumber(int * const p_board, int * const p_channel);
		int get_tunnelBoardNumber(int * const p_board);

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
