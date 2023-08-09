/*!
 * \file DssdEvent.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */
#pragma once

#ifndef DssdEvent_h
#define DssdEvent_h 1
#include "UTypes.h"
#include "DssdPixel.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class DssdEvent
{
	private:
		DssdPixel fPixel;
		double fCFDTime;
		ullint fTimeStamp;
		double fEnergyX;
		double fEnergyY;

	public:
		DssdEvent();
		DssdEvent(DssdPixel p, ullint t1, double t2, double ex, double ey);
		~DssdEvent();
		void SetPixel(DssdPixel p){fPixel = p;}
		void SetCFDTime(double t){fCFDTime = t;}
		void SetTimeStamp(ullint t){fTimeStamp = t;}
		void SetEnergyX(double ex){fEnergyX = ex;}
		void SetEnergyY(double ey){fEnergyY = ey;}

		DssdPixel GetPixel() const {return fPixel;}
		double GetCFDTime() const {return fCFDTime;}
		ullint GetTimeStamp() const {return fTimeStamp;}
		double GetEnergyX() const {return fEnergyX;}
		double GetEnergyY() const {return fEnergyY;}

		DssdPixel* GetPixelAddress() {return &fPixel;}
		double* GetCFDTimeAddress() {return &fCFDTime;}
		ullint* GetTimeStampAddress() {return &fTimeStamp;}
		double* GetEnergyXAddress() {return &fEnergyX;}
		double* GetEnergyYAddress() {return &fEnergyY;}


		void Clear();
		friend bool CompareTimeEvent(DssdEvent const & i, DssdEvent const & j);
		friend bool CompareTimeDssdEvent(DssdEvent const & i, DssdEvent const & j);

		DssdEvent& operator=(const DssdEvent& other){

			if(this == &other) return *this;

			fPixel = other.GetPixel();
			fTimeStamp = other.GetTimeStamp();
			fCFDTime = other.GetCFDTime();
			fEnergyX = other.GetEnergyX();
			fEnergyY = other.GetEnergyY();

			return *this;
		}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
