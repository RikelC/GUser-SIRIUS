/*! \file tunnelPixel.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details
 *
 * */

#ifndef tunnelPixel_h
#define tunnelPixel_h 1
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class tunnelPixel
{
	private:
		int N;/*!< Physical number of the pixel from 0-63, 0-7 being the nearest to the DSSD and 56-63 being the furthest from the DSSD. */
		int X; /*!< Horizontal position of the pixel.*/
		int Y;/*!< Vertical position of the pixel.*/
	public:
		//! Constructor
		/*! Here, one initializes the variables with negative values.
		*/
		tunnelPixel(){N = -100; Y = -100, X = -100;}
		//! Constructor
		/*! Here, one initializes the variable X and Y according to the parameter passed. X is computed as X = N/8 and Y is computed as Y = N%8.
		*/
		tunnelPixel(int n){N = n; Y = int(n/8), X = (n%8);}
		/*! Destructor
		*/	
		~tunnelPixel(){}
		/*! Set the value of N.
		*/		
		void set_N(int n){N = n;}
		/*! The the value of X.
		*/
		void set_X(int x){X = x;}
		/*! Set the value of Y.
		*/
		void set_Y(int y){Y = y;}
		/*! Get the value of N. 
		*/
		int get_N()const{return N;}
		/*! Get the value of X.
		*/
		int get_X()const{return X;}
		/*! Get the value of Y.
		*/
		int get_Y()const{return Y;}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
