/*!
 * \file trackerData.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#pragma once

#ifndef TrackerEvent_h
#define traclerEvent_h 1
//#include "Global.h"
#include "UTypes.h"

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

class TrackerEvent
{

	private:
		//		MyGlobal *s1; 
		ullint fTimeStamp;
		float fBaryCenterXm, fBaryCenterYm, fBaryCenterZm;//barycenter position
		int fMultiplicityX,fMultiplicityY;// multiplicity
		float fSumX,fSumY;// charge integration

	public:
		TrackerEvent();
		TrackerEvent(float x, float y, float z, float Ex, float Ey, int Mx, int My, ullint time);
		~TrackerEvent();
		//setters	
		void SetTimeStamp(ullint i){fTimeStamp =i;}
		void SetBaryCenterXm(float x){fBaryCenterXm = x;}
		void SetBaryCenterYm(float x){fBaryCenterYm = x;}
		void SetBaryCenterZm(float x){fBaryCenterZm = x;}
		void SetSumX(float x){fSumX = x;}
		void SetSumY(float x){fSumY = x;}
		void SetMultiplicityX(int x){fMultiplicityX = x;}
		void SetMultiplicityY(int x){fMultiplicityY = x;}
		//Getters
		ullint GetTimeStamp() const{return fTimeStamp;}
		float GetBaryCenterXm() const{return fBaryCenterXm;}
		float GetBaryCenterYm() const{return fBaryCenterYm;}
		float GetBaryCenterZm() const{return fBaryCenterZm;}
		float GetSumX() const{return fSumX;}
		float GetSumY() const{return fSumY;}
		int GetMultiplicityX() const{return fMultiplicityX;}
		int GetMultiplicityY() const{return fMultiplicityY;}

		friend bool CompareTimeTrackerEvent(TrackerEvent const & i, TrackerEvent const & j);
		TrackerEvent& operator=(const TrackerEvent& other){

			if(this == &other) return *this;

			fTimeStamp = other.GetTimeStamp();
			fBaryCenterXm = other.GetBaryCenterXm();
			fBaryCenterYm = other.GetBaryCenterYm();
			fMultiplicityX = other.GetMultiplicityX();
			fMultiplicityY = other.GetMultiplicityY();
			fSumX = other.GetSumX();
			fSumY = other.GetSumY();

			return *this;
		}

};
#endif
