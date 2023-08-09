/*!
 * \file DssdPixel.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */
#pragma once

#ifndef DssdPixel_h
#define DssdPixel_h 1
#include "UTypes.h"

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class DssdPixel
{
	private:
		int fX;
		int fY;

	public:
		DssdPixel();
		DssdPixel(int x, int y);
		~DssdPixel();

		void SetX(int x){fX = x;}
		void SetY(int y){fY = y;}

		int GetX() const {return fX;}
		int GetY() const {return fY;}

		int* GetXAddress() {return &fX;}
		int* GetYAddress() {return &fY;}

		void Clear();

		DssdPixel& operator=(const DssdPixel& other){
			if(this == &other) return *this;
			fX = other.GetX();
			fY = other.GetY();
			return *this;
		}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
