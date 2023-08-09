/*! \file RecoilEvent.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */

#pragma once

#ifndef RecoilEvent_h
#define RecoilEvent_h 1
#include "Event.h"
#include "UTypes.h"
#include "TrackerEvent.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class RecoilEvent : public Event
{
	protected:
		TrackerEvent fTrackerEvent;
		llint fToF;
	public:

		RecoilEvent();
		~RecoilEvent();	
		void SetTrackerEvent(TrackerEvent evt){fTrackerEvent = evt;}
		TrackerEvent GetTrackerEvent() { return fTrackerEvent;}
		void SetToF(llint t){fToF =t;}
		llint GetToF() const {return fToF;}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
