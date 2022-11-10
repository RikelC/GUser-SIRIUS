/*!
 * \file dssdPixel.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */
#ifndef dssdPixel_h
#define dssdPixel_h 1
#include "UTypes.h"

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class dssdPixel
{
	private:
		int X;
		int Y;
		ullint time;
		double energyX;
		double energyY;
		//int type;//RECOIL,DECAY

	public:
		dssdPixel();
		dssdPixel(int x, int y, ullint t, double ex, double ey);
		~dssdPixel();
		void set_X(int x){X = x;}
		void set_Y(int y){Y = y;}
		void set_time(ullint t){time = t;}
		void set_energyX(double ex){energyX = ex;}
		void set_energyY(double ey){energyY = ey;}

		int get_X() const {return X;}
		int get_Y() const {return Y;}
		ullint get_time() const {return time;}
		double get_energyX() const {return energyX;}
		double get_energyY() const {return energyY;}

		void clear();
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
