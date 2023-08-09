/*! \file TunnelPixel.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 * */

#pragma once

#ifndef TunnelPixel_h
#define TunnelPixel_h 1
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class TunnelPixel
{
	private:
		int fN;/*!< Physical number of the pixel from 0-63, 0-7 being the nearest to the DSSD and 56-63 being the furthest from the DSSD. */
		int fX; /*!< Horizontal position of the pixel.*/
		int fY;/*!< Vertical position of the pixel.*/
	public:
		//! Constructor
		/*! Here, one initializes the variables with negative values.
		*/
		TunnelPixel();
		//! Constructor
		/*! Here, one initializes the variable fX and fY according to the parameter passed. fX is computed as fX = fN/8 and fY is computed as fY = fN%8.
		*/
		TunnelPixel(int n);
		/*! Destructor
		*/	
		~TunnelPixel();
		/*! Set the value of fN.
		*/		
		void SetN(int n){fN = n;}
		/*! The the value of fX.
		*/
		void SetX(int x){fX = x;}
		/*! Set the value of fY.
		*/
		void SetY(int y){fY = y;}
		/*! Get the value of fN. 
		*/
		int GetN()const{return fN;}
		/*! Get the value of fX.
		*/
		int GetX()const{return fX;}
		/*! Get the value of fY.
		*/
		int GetY()const{return fY;}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
