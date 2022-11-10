/*! \file tunnelMacroPixel.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 * */

#ifndef tunnelMacroPixel_h
#define tunnelMacroPixel_h 1
#include "tunnelPixel.h"
#include <vector>

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class tunnelMacroPixel
{
	private:

		char name;/*!< Name of the Macro pixel*/
		int N;/*!< Physical number from 0-23, 0-4 being nearest to the DSSD and 20-23 being the furthest from the DSSD.*/
		int detector;/*!< detector number from 1-4 */
	public:

		/*! Constructor
		*/
		tunnelMacroPixel(){N = -100; name = '?'; detector = -100;}
		/*! Destructor
		*/		
		~tunnelMacroPixel(){if(pixels.size()>0)pixels.clear();}
		/*! Set the name of the macro pixel.
		*/
		void set_name(char n){name = n;}
		/*! get the name of the macro pixel
		*/
		char get_name()const{return name;}
		/*! Set N of the macro pixel.
		*/
		void set_N(int n){N = n;}
/*! Set detector number
 */		
void set_detector(int n){detector = n;}
		/*! Get N of the macro pixel.
		*/
		char get_N()const{return N;}
	
	/*! Get detector number of the macro pixel.
		*/
		char get_detector()const{return detector;}

	/*! A vector containing the physical sub pixels associated to this macro pixel.
		*/
		vector<tunnelPixel> pixels; 
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
