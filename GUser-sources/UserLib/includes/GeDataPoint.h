/*! \file GeDataPoint.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 */
#pragma once

#ifndef GeDataPoint_h
#define GeDataPoint_h 1
#include "UTypes.h"
#include "Numexo2DataPoint.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class GeDataPoint : public Numexo2DataPoint
{
	private:
	public:
		GeDataPoint();
		~ GeDataPoint();

		void Clear();
		friend bool CompareEnergy(GeDataPoint const & i, GeDataPoint const & j);
		friend bool CompareTime(GeDataPoint const & i, GeDataPoint const & j);

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
