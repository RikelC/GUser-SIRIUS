/*! \file DecayEvent.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */

#pragma once

#ifndef DecayEvent_h
#define DecayEvent_h 1
#include "UTypes.h"
#include "Event.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class DecayEvent : public Event
{
	protected:
	public:
		DecayEvent();
		~DecayEvent();	
		void Clear();
		DecayEvent& operator=(const DecayEvent& rhs){
			if(this == &rhs) return *this;
			else{
				fDssdEvent = rhs.GetDssdEvent();
			}
			return *this;
		}

		inline bool operator==(const DecayEvent& rhs){
			if(this == &rhs) return true;
			else{ 
				return (fDssdEvent == rhs.GetDssdEvent());
			}
		}

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
