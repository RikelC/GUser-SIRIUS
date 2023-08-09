/*!
 * \file TrackerEvent.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */

#include "TrackerEvent.h"

TrackerEvent::TrackerEvent(){
	fBaryCenterXm = 0;
	fBaryCenterYm =0;
	fSumX =0;
	fSumY = 0;
	fMultiplicityX = 0;
	fMultiplicityY = 0;
	fTimeStamp = 0;
}

TrackerEvent::TrackerEvent(float x, float y, float z, float Ex, float Ey, int Mx, int My, ullint time){
	fBaryCenterXm = x;
	fBaryCenterYm =y;
	fBaryCenterZm =y;
	fSumX =Ex;
	fSumY = Ey;
	fMultiplicityX = Mx;
	fMultiplicityY = My;
	fTimeStamp = time;
}

TrackerEvent::~TrackerEvent(){}


