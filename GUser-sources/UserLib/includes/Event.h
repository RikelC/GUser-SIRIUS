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
class Event
{
	protected:
		DssdEvent fDssdEvent;
	public:

		Event();
		~Event();	
		void SetDssdEvent(DssdEvent evt){fDssdEvent = evt;}
		DssdEvent GetDssdEvent() {return fDssdEvent;}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
