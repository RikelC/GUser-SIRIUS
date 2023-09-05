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
#include "TrackerCoBoEvent.h"
#include "TrackerNumexo2Event.h"
#include <iostream>
#define NPOINTS 101
#define DISTANCE 100. // cm
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class TrackerCoBoEvent;
class TrackerNumexo2Event;

class RecoilEvent : public Event
{
	private:
		TrackerCoBoEvent fTrackerCoBoEvent;
		TrackerNumexo2Event fTrackerNumexo2Event;
		llint fToFCoBo;
		llint fToFNumexo2X;
		llint fToFNumexo2Y;
		double fCFDToFNumexo2X;
		double fCFDToFNumexo2Y;
		bool flagCoBo;
		bool flagSedNumexo2;
		double fTrackLength;
		double fTrackSlopeXZ;
		double fTrackSlopeYZ;
		double fTrackX[NPOINTS];
		double fTrackY[NPOINTS];
	public:

		RecoilEvent();
		~RecoilEvent();	
		void ReconstructTrajectory();
		void Clear();
		void SetTrackerCoBoEvent(TrackerCoBoEvent evt){fTrackerCoBoEvent = evt;}
		void SetTrackerNumexo2Event(TrackerNumexo2Event evt){fTrackerNumexo2Event = evt;}
		void SetTrackXValue(int i, double x){fTrackX[i] =x;}
		void SetTrackYValue(int i, double x){fTrackY[i] =x;}
		double GetTrackXValue(int i) const {return fTrackX[i];}
		double GetTrackYValue(int i) const {return fTrackY[i];}
		double * GetTrackXAddress(){return fTrackX;}
		double * GetTrackYAddress(){return fTrackY;}
		double GetTrackLength() const {return fTrackLength;}
		double * GetTrackLengthAddress() {return &fTrackLength;}

		double GetTrackSlopeXZ() const {return fTrackSlopeXZ;}
		double GetTrackSlopeYZ() const {return fTrackSlopeYZ;}
		double * GetTrackSlopeXZAddress(){return &fTrackSlopeXZ;}
		double * GetTrackSlopeYZAddress(){return &fTrackSlopeYZ;}
		TrackerCoBoEvent GetTrackerCoBoEvent() const { return fTrackerCoBoEvent;}
		TrackerNumexo2Event GetTrackerNumexo2Event() const { return fTrackerNumexo2Event;}
		void SetToFCoBo(llint t){fToFCoBo =t;}
		void SetToFNumexo2(llint t){fToFNumexo2X =t;}
		void SetToFNumexo2X(llint t){fToFNumexo2X =t;}
		void SetToFNumexo2Y(llint t){fToFNumexo2Y =t;}
		void SetCFDToFNumexo2(double t){fCFDToFNumexo2X =t;}
		void SetCFDToFNumexo2X(double t){fCFDToFNumexo2X =t;}
		void SetCFDToFNumexo2Y(double t){fCFDToFNumexo2Y =t;}
		void SetCoBoFlag(bool n){flagCoBo =n;}
		void SetTrackerNumexo2Flag(bool n){flagSedNumexo2 =n;}
		llint GetToFCoBo() const {return fToFCoBo;}
		llint GetToFNumexo2() const {return fToFNumexo2X;}
		llint GetToFNumexo2X() const {return fToFNumexo2X;}
		llint GetToFNumexo2Y() const {return fToFNumexo2Y;}
		double GetCFDToFNumexo2() const {return fCFDToFNumexo2X;}
		double GetCFDToFNumexo2X() const {return fCFDToFNumexo2X;}
		double GetCFDToFNumexo2Y() const {return fCFDToFNumexo2Y;}
		bool HasACoBoEvent() const {return flagCoBo;}
		bool HasATrackerNumexo2Event() const {return flagSedNumexo2;}
		llint * GetAddressOfToFCoBo() {return &fToFCoBo;}
		llint * GetAddressOfToFNumexo2() {return &fToFNumexo2X;}
		llint * GetAddressOfToFNumexo2X() {return &fToFNumexo2X;}
		llint * GetAddressOfToFNumexo2Y() {return &fToFNumexo2Y;}
		double * GetAddressOfCFDToFNumexo2() {return &fCFDToFNumexo2X;}
		double * GetAddressOfCFDToFNumexo2X() {return &fCFDToFNumexo2X;}
		double * GetAddressOfCFDToFNumexo2Y() {return &fCFDToFNumexo2Y;}
		bool * GetAddressOfFlagCoBo() {return &flagCoBo;}
		bool * GetAddressOfFlagSedNumexo2() {return &flagSedNumexo2;}
		TrackerCoBoEvent * GetAddressOfTrackerCoBoEvent() {return &fTrackerCoBoEvent;}
		TrackerNumexo2Event * GetAddressOfTrackerNumexo2Event() {return &fTrackerNumexo2Event;}

		void Print(){
			std::cout<<"ToFCoBo: "<<fToFCoBo<<"  ToFNumexo2X: "<<fToFNumexo2X<<" ToFNumexo2Y:  "<<fToFNumexo2Y <<"  CFDToFNumexo2Front:  "<<fCFDToFNumexo2X<<"  CFDToFNumexo2Back: "<<fCFDToFNumexo2Y<<std::endl;
			std::cout<<"EX: "<<fDssdEvent.GetEnergyX()<<"  EY: "<<fDssdEvent.GetEnergyY()<<"  X: "<<fDssdEvent.GetPixel().GetX()<<"  Y: "<<fDssdEvent.GetPixel().GetY()<<std::endl;
			std::cout<<"SedE: "<<fTrackerNumexo2Event.GetEnergy()<<std::endl;
			std::cout<<"BarXm: "<<fTrackerCoBoEvent.GetBaryCenterXm()<<" Ym: "<<fTrackerCoBoEvent.GetBaryCenterYm()<<"  Zm: "<<fTrackerCoBoEvent.GetBaryCenterZm()<<std::endl;

		}
		RecoilEvent& operator=(const RecoilEvent& rhs){

			if(this == &rhs) return *this;

			fDssdEvent = rhs.GetDssdEvent();
			fTrackerCoBoEvent = rhs.GetTrackerCoBoEvent();
			fTrackerNumexo2Event = rhs.GetTrackerNumexo2Event();
			fToFCoBo = rhs.GetToFCoBo();
			fToFNumexo2X = rhs.GetToFNumexo2X();
			fToFNumexo2Y = rhs.GetToFNumexo2Y();
			fCFDToFNumexo2X = rhs.GetCFDToFNumexo2X();
			fCFDToFNumexo2Y = rhs.GetCFDToFNumexo2Y();
			flagCoBo = rhs.HasACoBoEvent();
			flagSedNumexo2 = rhs.HasATrackerNumexo2Event();
			fTrackLength = rhs.GetTrackLength();
			fTrackSlopeXZ = rhs.GetTrackSlopeXZ();
			fTrackSlopeYZ = rhs.GetTrackSlopeYZ();
			for(int i =0; i < NPOINTS;i++){
				fTrackX[i] = rhs.GetTrackXValue(i);
				fTrackY[i] = rhs.GetTrackYValue(i);
			}
			return *this;
		}
		inline bool operator==(const RecoilEvent& rhs){

			if(this == &rhs) return true;

			if( fDssdEvent == rhs.GetDssdEvent()
					&& fTrackerCoBoEvent == rhs.GetTrackerCoBoEvent()
					&& fTrackerNumexo2Event == rhs.GetTrackerNumexo2Event()
					&& fToFCoBo == rhs.GetToFCoBo()
					&& fToFNumexo2X == rhs.GetToFNumexo2X()
					&& fToFNumexo2Y == rhs.GetToFNumexo2Y()
					&& fCFDToFNumexo2X == rhs.GetCFDToFNumexo2X()
					&& fCFDToFNumexo2Y == rhs.GetCFDToFNumexo2Y()
					&& flagCoBo == rhs.HasACoBoEvent()
					&& flagSedNumexo2 == rhs.HasATrackerNumexo2Event()
					&& fTrackLength == rhs.GetTrackLength()
					&& fTrackSlopeXZ == rhs.GetTrackSlopeXZ()
					&& fTrackSlopeYZ == rhs.GetTrackSlopeYZ())
				return true;
			else return false;
		}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
