/*! \file tunnelDataPoint.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 */
#ifndef tunnelDataPoint_h
#define tunnelDataPoint_h 1
#include "UTypes.h"
#include "tunnelMacroPixel.h"
#include "tunnelData.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class tunnelDataPoint
{
	private:
		tunnelMacroPixel macroPixel; /*!< Macro pixel*/
		ullint time;/*< Time of the event */
		double energy;/*< Energy recorded in this strip*/
		float neighborhoodness;
	public:
		tunnelDataPoint();
		tunnelDataPoint(tunnelMacroPixel s, ullint t, double e);
		~ tunnelDataPoint();
		//setters	
		void set_macroPixel(tunnelMacroPixel s){ macroPixel = s;} 
		void set_time(ullint t){time =t;}
		void set_energy(double e){energy = e;}
		void set_values(tunnelData * const data){
			macroPixel = data->get_macroPixel();
			time = data->get_timestamp();
			energy = data->get_calibrated_energy();
		}
		//getters	
		tunnelMacroPixel get_macroPixel()const { return macroPixel;}
		ullint get_time() const {return time;}
		double get_energy() const {return energy;}

		void clear();
		/*! get the current  macro pixel's neighborhoodness with respect to another macropixel.
		*/
		float get_neighborhoodness(tunnelMacroPixel *l);
		/*! get the current  macro pixel's neighborhoodness with respect to another macropixel.
		*/
		float get_neighborhoodness(tunnelDataPoint *l);

		friend bool compareEnergy(tunnelDataPoint const & i, tunnelDataPoint const & j);
		friend bool compareTime(tunnelDataPoint const & i, tunnelDataPoint const & j);

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
