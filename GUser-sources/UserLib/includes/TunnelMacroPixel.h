/*! \file TunnelMacroPixel.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 * */

#pragma once

#ifndef TunnelMacroPixel_h
#define TunnelMacroPixel_h 1
#include "TunnelPixel.h"
#include <vector>
#include <algorithm>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class TunnelMacroPixel
{
	private:
		char fName;/*!< Name of the Macro pixel*/
		int fN;/*!< Physical number from 0-23, 0-4 being nearest to the DSSD and 20-23 being the furthest from the DSSD.*/
		int fDetector;/*!< fDetector number from 1-4 */
		int fBoard;/*!< numexo fBoard number */
		int fChannel;/*!< fChannel number from 0-15 */
	public:

		/*! Constructor
		*/
		TunnelMacroPixel();
		/*! Destructor
		*/		
		~TunnelMacroPixel();
		/*! Set the Name of the macro pixel.
		*/
		void SetName(char n){fName = n;}
		/*! get the Name of the macro pixel
		*/
		char GetName()const{return fName;}
		/*! Set N of the macro pixel.
		*/
		void SetN(int n){fN = n;}
		void SetBoard(int n){fBoard = n;}
		void SetChannel(int n){fChannel = n;}
		/*! Set fDetector number
		*/		
		void SetDetector(int n){fDetector = n;}

		/*! Get N of the macro pixel.
		*/
		int GetN()const{return fN;}
		int GetBoard()const{return fBoard;}
		int GetChannel()const{return fChannel;}

		/*! Get fDetector number of the macro pixel.
		*/
		int GetDetector()const{return fDetector;}

		/*! a vector containing the physical sub pixels associated to this macro pixel.
		*/
		std::vector<TunnelPixel> pixels; 
		/*!  Reset values
		 * */
		void Clear();
		TunnelMacroPixel& operator=(const TunnelMacroPixel& other){

			if(this == &other) return *this;

			fName = other.GetName();
			fN = other.GetN();
			fDetector = other.GetDetector();
			fBoard = other.GetBoard();
			fChannel = other.GetChannel();
			pixels.clear();
			std::copy(other.pixels.begin(), other.pixels.end(),
					std::back_inserter(pixels));

			return *this;
		}

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
