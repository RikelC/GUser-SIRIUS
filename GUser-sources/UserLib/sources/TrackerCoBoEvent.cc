/*!
 * \file TrackerCoBoEvent.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */

#include "TrackerCoBoEvent.h"

TrackerCoBoEvent::TrackerCoBoEvent(){
	fBaryCenterXm = 0;
	fBaryCenterYm =0;
	fSumX =0;
	fSumY = 0;
	fMultiplicityX = 0;
	fMultiplicityY = 0;
	fTimeStamp = 0;
}

TrackerCoBoEvent::TrackerCoBoEvent(double x, double y, double z, double Ex, double Ey, int Mx, int My, ullint time){
	fBaryCenterXm = x;
	fBaryCenterYm =y;
	fBaryCenterZm =y;
	fSumX =Ex;
	fSumY = Ey;
	fMultiplicityX = Mx;
	fMultiplicityY = My;
	fTimeStamp = time;
}

TrackerCoBoEvent::~TrackerCoBoEvent(){}


void TrackerCoBoEvent::Clear(){
	fBaryCenterXm = 0;
	fBaryCenterYm =0;
	fSumX =0;
	fSumY = 0;
	fMultiplicityX = 0;
	fMultiplicityY = 0;
	fTimeStamp = 0;

}
