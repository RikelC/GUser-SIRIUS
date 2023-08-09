/*! \file Numexo2Data.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */
#pragma once

#ifndef Numexo2Data_h
#define Numexo2Data_h 1
#include "Global.h"
#include "UTypes.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class Numexo2Data
{
	protected:
		MyGlobal *s1; /*!< pointer to the instance of the myGlobal singleton class. */ 
		int fBoard;/*!< board ID from data */
		int fBoardIndex;/*!< index of the board from the list of the boards*/
		int fChannel;/*!< Channel ID staring from 0-15 */
		uint fEventNumber;/*!< event number from data */
		ullint fTimeStamp;/*!< fTimeStamp from data*/
		double fRawEnergy;/*!< raw energy value */
		double fCalibratedEnergy;/*!< Calibrated Energy value*/
	public:
		Numexo2Data();
		~Numexo2Data();	
		//Setters
		void SetBoard(int i){fBoard =i;}
		void SetBoardIndex(int i){fBoardIndex =i;}
		void SetChannel(int i){fChannel = i;}
		void SetEventNumber(uint i){fEventNumber =i;}
		void SetTimeStamp(ullint i){fTimeStamp =i;}
		void SetRawEnergy(double x){fRawEnergy =x;}
		void SetCalibratedEnergy(double x){fCalibratedEnergy =x;}
		//Getters
		int GetBoard()const {return fBoard;}
		int GetBoardIndex()const {return fBoardIndex;}
		int GetChannel()const{return fChannel;}
		uint GetEventNumber()const {return fEventNumber;}
		ullint GetTimeStamp()const {return fTimeStamp;}
		double GetRawEnergy() const {return fRawEnergy;}
		double GetCalibratedEnergy()const {return fCalibratedEnergy;}

		int* GetBoardAddress() {return &fBoard;}
		int* GetBoardIndexAddress() {return &fBoardIndex;}
		int* GetChannelAddress(){return &fChannel;}
		uint* GetEventNumberAddress() {return &fEventNumber;}
		ullint* GetTimeStampAddress() {return &fTimeStamp;}
		double* GetRawEnergyAddress() {return &fRawEnergy;}
		double* GetCalibratedEnergyAddress(){return &fCalibratedEnergy;}


};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
