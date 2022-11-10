/*! \file dssdDataPoint.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 */
#ifndef dssdDataPoint_h
#define dssdDataPoint_h 1
#include "UTypes.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class dssdDataPoint
{
	private:

		int strip; /*!< Strip number*/
		ullint time;/*< Time of the event */
		double energy;/*< Energy recorded in this strip*/
	public:
		dssdDataPoint();
		dssdDataPoint(int s, ullint t, double e);
		~ dssdDataPoint();
		//setters	
		void set_strip(int s){ strip = s;} 
		void set_time(ullint t){time =t;}
		void set_energy(double e){energy = e;}
		//getters	
		int get_strip()const { return strip;}
		ullint get_time() const {return time;}
		double get_energy() const {return energy;}

		void clear();
		bool is_frontStrip()const;
		bool is_backStrip()const;
		bool is_neighboringStripOf(dssdDataPoint &d);
		friend bool compareEnergy(dssdDataPoint const & i, dssdDataPoint const & j);
		friend bool compareTime(dssdDataPoint const & i, dssdDataPoint const & j);

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
