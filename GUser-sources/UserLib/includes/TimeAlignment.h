/*! \file TimeAlignment.h
 *  \author Rikel CHAKMA
 *  \brief
 *  \details
 *
 */
#pragma once

#ifndef TimeAlignment_h
#define TimeAlignment_h 1
#include "Global.h"
#include "DssdData.h"
#include "UTypes.h"
#include <fstream>
#include <iostream>
class TimeAlignment{
	private:
		MyGlobal* s1;
		int **fTimeStampShift;
		double **fCFDTimeStampShift;

	public:

		TimeAlignment();
		~TimeAlignment();
		int GetTimeShift(int boardIdx, int channel) const { return fTimeStampShift[boardIdx][channel];}
		int GetTimeStampShift(DssdData* const data); 
		double GetCFDTimeShift(DssdData* const data); 
		ullint AlignTimeStamp(DssdData* const data);
		double AlignTime(DssdData* const data);
		ullint AlignTimeStampWithTrigger(DssdData * const data);
		ullint AlignTimeStampWithMaximumPosition(DssdData * const data);
		void AlignTime(double &time, int &board, int &channel);
		void AlignTimeStamp(ullint &timestamp, int &board, int &channel);
		void AlignTrigger(ushort &trigger, int &board, int &channel);
};
#endif

