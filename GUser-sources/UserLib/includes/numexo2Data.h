/*! \file numexo2Data.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */

#ifndef numexo2Data_h
#define numexo2Data_h 1
#include "global.h"
#include "UTypes.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class numexo2Data
{
	protected:
		myGlobal *s1; /*!< pointer to the instance of the myGlobal singleton class. */ 
		int boardID;/*!< board ID from data */
		int boardIdx;/*!< index of the board from the list of the boards*/
		int channelID;/*!< Channel ID staring from 0-15 */
		uint eventnumber;/*!< event number from data */
		ullint timestamp;/*!< timestamp from data*/
		double raw_energy;/*!< raw energy value */
		double calibrated_energy;/*!< Calibrated Energy value*/
	public:
		numexo2Data();
		~numexo2Data();	
		//Setters
		void set_boardID(int i){boardID =i;}
		void set_boardIdx(int i){boardIdx =i;}
		void set_channelID(int i){channelID = i;}
		void set_eventnumber(uint i){eventnumber =i;}
		void set_timestamp(ullint i){timestamp =i;}
		void set_raw_energy(double x){raw_energy =x;}
		void set_calibrated_energy(double x){calibrated_energy =x;}
		//Getters
		int get_boardID()const {return boardID;}
		int get_boardIdx()const {return boardIdx;}
		int get_channelID()const{return channelID;}
		uint get_eventnumber()const {return eventnumber;}
		ullint get_timestamp()const {return timestamp;}
		double get_raw_energy() const {return raw_energy;}
		double get_calibrated_energy()const {return calibrated_energy;}


};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
