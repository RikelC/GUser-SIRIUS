/*!
 * \file trackerData.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#pragma once

#ifndef TrackerCoBoEvent_h
#define traclerEvent_h 1
//#include "Global.h"
#include "UTypes.h"

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

class TrackerCoBoEvent
{

	private:
		//		MyGlobal *s1; 
		ullint fTimeStamp;
		double fBaryCenterXm, fBaryCenterYm, fBaryCenterZm;//barycenter position
		int fMultiplicityX,fMultiplicityY;// multiplicity
		double fSumX,fSumY;// charge integration

	public:
		TrackerCoBoEvent();
		TrackerCoBoEvent(double x, double y, double z, double Ex, double Ey, int Mx, int My, ullint time);
		~TrackerCoBoEvent();
		void Clear();
		//setters	
		void SetTimeStamp(ullint i){fTimeStamp =i;}
		void SetBaryCenterXm(double x){fBaryCenterXm = x;}
		void SetBaryCenterYm(double x){fBaryCenterYm = x;}
		void SetBaryCenterZm(double x){fBaryCenterZm = x;}
		void SetSumX(double x){fSumX = x;}
		void SetSumY(double x){fSumY = x;}
		void SetMultiplicityX(int x){fMultiplicityX = x;}
		void SetMultiplicityY(int x){fMultiplicityY = x;}
		//Getters
		ullint GetTimeStamp() const{return fTimeStamp;}
		double GetBaryCenterXm() const{return fBaryCenterXm;}
		double GetBaryCenterYm() const{return fBaryCenterYm;}
		double GetBaryCenterZm() const{return fBaryCenterZm;}
		double GetSumX() const{return fSumX;}
		double GetSumY() const{return fSumY;}
		int GetMultiplicityX() const{return fMultiplicityX;}
		int GetMultiplicityY() const{return fMultiplicityY;}


		ullint * GetAddressOfTimeStamp() {return &fTimeStamp;}
		double * GetAddressOfBaryCenterXm() {return &fBaryCenterXm;}
		double * GetAddressOfBaryCenterYm() {return &fBaryCenterYm;}
		double * GetAddressOfBaryCenterZm() {return &fBaryCenterZm;}
		int * GetAddressOfMultiplicityX() {return &fMultiplicityX;}
		int * GetAddressOfMultiplicityY() {return &fMultiplicityY;}
		double * GetAddressOfSumX() {return &fSumX;}
		double * GetAddressOfSumY() {return &fSumY;}






		friend bool CompareTimeTrackerCoBoEvent(TrackerCoBoEvent const & i, TrackerCoBoEvent const & j);

		TrackerCoBoEvent& operator=(const TrackerCoBoEvent& other){

			if(this == &other) return *this;

			fTimeStamp = other.GetTimeStamp();
			fBaryCenterXm = other.GetBaryCenterXm();
			fBaryCenterYm = other.GetBaryCenterYm();
			fBaryCenterZm = other.GetBaryCenterZm();
			fMultiplicityX = other.GetMultiplicityX();
			fMultiplicityY = other.GetMultiplicityY();
			fSumX = other.GetSumX();
			fSumY = other.GetSumY();

			return *this;
		}
		inline bool operator==(const TrackerCoBoEvent& other){

			if(this == &other) return true;

			if( fTimeStamp == other.GetTimeStamp()
					&& fBaryCenterXm == other.GetBaryCenterXm()
					&& fBaryCenterYm == other.GetBaryCenterYm()
					&& fMultiplicityX == other.GetMultiplicityX()
					&& fMultiplicityY == other.GetMultiplicityY()
					&& fSumX == other.GetSumX()
					&& fSumY == other.GetSumY())
				return true;
			else return false;
		}


};
#endif
