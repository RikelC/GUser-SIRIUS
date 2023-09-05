/*!
 * \file TrackerNumexo2Data.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */

#pragma once

#ifndef TrackerNumexo2Data_h
#define TrackerNumexo2Data_h 1
#include "Numexo2Data.h"
#include "TMath.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class TrackerNumexo2Data : public Numexo2Data
{
	private:
		ushort trace[992];/*!< 1D array for storing the trace. */
		//extracted data
		int stripnumber;/*!< strip number obtained using get_DSSD_strip_number(int *p_board, int *p_channel) */
		double Baseline;/*!< Baseline of the signal calculated for 200 points in GetSignalInfo()  */
		double signal_is;/*!< variable for the polarity of the signal. */
		double Noise;/*!< Noise of the signal. Noise = standard deviation calculated over 200 points in the begining of the trace.  */
		double signalHeight;/*!< Height of the signal is calculated by subtarcting the Baseline frim the maximum value. */
		ushort RiseTime;/*!< Risetime of the siganl */
		ushort DecayTime;/*!< Decaytime of the signal */
		ushort SaturationTime;/*!< Saturation time */		
		ushort Min_pos;
		ushort Max_pos;/*!< Sample number of the occurance of the Maximum value. */
		ushort Trigger;/*!< Sample number of the occurance of the trigger. */
		double cfd_time;/*!< Time obtained by using the digital constant fraction discriminator in digitalCFD class. */
		double RC_constant;/*!< */
		bool saturated;/*!< */
		bool noisy_signal;/*!< */
		std::map<std::pair<int, int >, int > stripMap;
		//private methods		
		double CalculateRCConstant();
		void MapStripNumber();
		void Reset(){
			stripnumber = -1;
			Baseline = 0.;
			signal_is = 0.;
			Noise = 0.;//sigma
			signalHeight = 0.;//max val - baseline
			RiseTime = 0;
			DecayTime = 0;
			SaturationTime = 0;		
			Max_pos = 0;
			Min_pos = 0;
			Trigger = 0;
			cfd_time = 0;
			RC_constant = 0.;
			saturated = 0;
			noisy_signal = 0;
		}
	public:
		TrackerNumexo2Data();
		~TrackerNumexo2Data();
		//Setters	
		void SetTraceValue(int i, ushort v){trace[i] = v;}
		void SetBaseline(double x){Baseline = x;}
		void SetNoise(double x){ Noise =x;}//sigma
		void SetSignalHeight(double x) { signalHeight =x;}//max val - baseline
		void SetRiseTime(ushort x){ RiseTime =x;}
		void SetMaximumPosition( ushort x) { Max_pos =x;}
		void SetTrigger (ushort x){ Trigger =x;}
		void SetCFDTime(double x){ cfd_time =x ;}

		// Getters
		ushort GetTraceValue(int i)const {return trace[i];}
		ushort * GetTrace() {return trace;}
		double SignalIs()const{return signal_is;}
		double GetBaseline()const{return Baseline;}
		double GetNoise()const{return Noise;}//sigma
		ushort GetRiseTime()const{return RiseTime;}
		ushort GetMaximumPosition()const{return Max_pos;}
		ushort GetTrigger()const{return Trigger;}
		double GetSignalHeight()const{return signalHeight;}//max val - baseline
		double GetCFDTime()const{return cfd_time;}
		double GetRCConstant() const {return RC_constant;}
		bool IsSaturated() const {return saturated;}
		bool IsNoisy() const {return noisy_signal;}
		void GetSignalInfo();
		int GetStripNumber(int * const, int * const); 
		int GetStripNumber() const {return stripnumber;} 
		//Get Addesses
		double* GetBaselineAddress() {return &Baseline;}
		ushort* GetTriggerAddress() {return &Trigger;}
		ushort* GetMaximumPositionAddress() {return &Max_pos;}
		double* GetNoiseAddress(){return &Noise;}
		ushort* GetRiseTimeAddress(){return &RiseTime;} 
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
