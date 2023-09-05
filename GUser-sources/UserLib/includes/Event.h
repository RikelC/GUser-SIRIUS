/*! \file Event.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */

#pragma once

#ifndef Event_h
#define Event_h 1
#include "UTypes.h"
#include "DssdEvent.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class DssdEvent;
class Event
{
	protected:
		DssdEvent fDssdEvent;
	public:

		Event();
		~Event();	
		virtual void Clear();
		void SetDssdEvent(DssdEvent evt){fDssdEvent = evt;}
		DssdEvent GetDssdEvent()const {return fDssdEvent;}
		DssdEvent * GetAddressOfDssdEvent() {return &fDssdEvent;}

		Event& operator=(const Event& rhs){
			if(this == &rhs) return *this;
			fDssdEvent = rhs.GetDssdEvent();
			return *this;
		}
		inline bool operator==(const Event& rhs){
			if(this == &rhs) return true;
			if(fDssdEvent == rhs.GetDssdEvent())
				return true;
			else return false;
		}

};
#endif//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
