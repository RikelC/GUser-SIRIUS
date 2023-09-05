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
#include <algorithm>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class DssdEvent
{
	private:
		DssdPixel fPixel;
		double fCFDTimeX;
		double fCFDTimeY;
		ullint fTimeStampX;
		ullint fTimeStampY;
		double fEnergyX;
		double fEnergyY;
		ushort fTraceX[992];
		ushort fTraceY[992];

	public:
		DssdEvent();
		DssdEvent(DssdPixel p, ullint t1, ullint t2, double t3, double t4, double ex, double ey);
		~DssdEvent();
		void Reset();
		void SetPixel(DssdPixel p){fPixel = p;}
		void SetCFDTime(double t){fCFDTimeX = t;}
		void SetCFDTimeX(double t){fCFDTimeX = t;}
		void SetCFDTimeY(double t){fCFDTimeY = t;}
		void SetTimeStamp(ullint t){fTimeStampX = t;}
		void SetTimeStampX(ullint t){fTimeStampX = t;}
		void SetTimeStampY(ullint t){fTimeStampY = t;}
		void SetEnergyX(double ex){fEnergyX = ex;}
		void SetEnergyY(double ey){fEnergyY = ey;}
		void SetTraceX(ushort *v, int size){std::copy(v, v+size, fTraceX);}
		void SetTraceY(ushort *v, int size){std::copy(v, v+size, fTraceY);}
		DssdPixel GetPixel() const {return fPixel;}
		double GetCFDTime() const {return fCFDTimeX;}
		double GetCFDTimeX() const {return fCFDTimeX;}
		double GetCFDTimeY() const {return fCFDTimeY;}
		ullint GetTimeStamp() const {return fTimeStampX;}
		ullint GetTimeStampX() const {return fTimeStampX;}
		ullint GetTimeStampY() const {return fTimeStampY;}
		double GetEnergy() const {return fEnergyX;}
		double GetEnergyX() const {return fEnergyX;}
		double GetEnergyY() const {return fEnergyY;}
		ushort *GetTrace() {return fTraceX;}
		ushort *GetTraceX() {return fTraceX;}
		ushort *GetTraceY() {return fTraceY;}
		ushort *GetAddressOfTrace() {return fTraceX;}
		ushort *GetAddressOfTraceX() {return fTraceX;}
		ushort *GetAddressOfTraceY() {return fTraceY;}
		ushort GetTraceValue(int i) const {return fTraceX[i];}
		ushort GetTraceXValue(int i) const {return fTraceX[i];}
		ushort GetTraceYValue(int i) const {return fTraceY[i];}
		DssdPixel* GetPixelAddress() {return &fPixel;}
		double* GetCFDTimeAddress() {return &fCFDTimeX;}
		double* GetCFDTimeXAddress() {return &fCFDTimeX;}
		double* GetCFDTimeYAddress() {return &fCFDTimeY;}
		ullint* GetTimeStampAddress() {return &fTimeStampX;}
		ullint* GetTimeStampXAddress() {return &fTimeStampX;}
		ullint* GetTimeStampYAddress() {return &fTimeStampY;}
		double* GetEnergyAddress() {return &fEnergyX;}
		double* GetEnergyXAddress() {return &fEnergyX;}
		double* GetEnergyYAddress() {return &fEnergyY;}


		void Clear();
		friend bool CompareTimeEvent(DssdEvent const & i, DssdEvent const & j);
		friend bool CompareTimeDssdEvent(DssdEvent const & i, DssdEvent const & j);

		DssdEvent& operator=(const DssdEvent& other){

			if(this == &other) return *this;

			fPixel = other.GetPixel();
			fTimeStampX = other.GetTimeStampX();
			fTimeStampY = other.GetTimeStampY();
			fCFDTimeX = other.GetCFDTimeX();
			fCFDTimeY = other.GetCFDTimeY();
			fEnergyX = other.GetEnergyX();
			fEnergyY = other.GetEnergyY();
			for(int i =0; i <992;i++){
				fTraceX[i]=other.GetTraceXValue(i);
				fTraceY[i]= other.GetTraceYValue(i);
			}

			return *this;
		}


		inline bool operator==(const DssdEvent& other){
			if(this == &other) return true;
			if(fPixel == other.GetPixel() 
					&& fTimeStampX == other.GetTimeStampX()
					&& fTimeStampY == other.GetTimeStampY()
					&& fCFDTimeX == other.GetCFDTimeX()
					&& fCFDTimeY == other.GetCFDTimeY()
					&& fEnergyX == other.GetEnergyX()
					&& fEnergyY == other.GetEnergyY()
			  )
				return true;
			else return false;

		}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
