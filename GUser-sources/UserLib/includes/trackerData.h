/*!
 * \file trackerData.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#ifndef trackerData_h
#define traclerData_h 1
#include "global.h"
#include "UTypes.h"
#include "TMath.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class trackerData
{

	private:
		myGlobal *s1; 
		uint eventnumber;
		ullint timestamp;
	public:
		trackerData();
		~trackerData();
		void set_eventnumber(uint i){eventnumber =i;}
		void set_timestamp(ullint i){timestamp =i;}
		uint get_eventnumber()const {return eventnumber;}
		ullint get_timestamp()const {return timestamp;}

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
