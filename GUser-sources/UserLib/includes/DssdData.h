/*!
 * \file DssdData.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */
#pragma once

#ifndef DssdData_h
#define DssdData_h 1
#include "Numexo2Data.h"
#include "TMath.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class DssdData : public Numexo2Data
{
	private:
		int fDifferentialSignalAmplitude;
		ushort fGain; /*!< Gain from the data. */
		//ushort *fTrace;/*!< 1D array for storing the fTrace. */
		ushort fTrace[992];/*!< 1D array for storing the fTrace. */
		//ushort autoGainTraceEnergy[992];/*!< 1D array for storing the fTrace. */
		ushort fAutoGainTraceTime[992];/*!< 1D array for storing the fTrace. */
		//extracted data
		int fStripNumber;/*!< strip number obtained using get_DSSD_strip_number(int *p_board, int *p_channel) */
		double fBaseline;/*!< fBaseline of the signal calculated for 200 points in GetSignalInfo()  */
		double fSignalIs;/*!< variable for the polarity of the signal. */
		double fNoise;/*!< fNoise of the signal. fNoise = standard deviation calculated over 200 points in the begining of the fTrace.  */
		double fSignalHeight;/*!< Height of the signal is calculated by subtarcting the fBaseline frim the maximum value. */
		bool fGain_switch_flag;/*!< For checking the fGain switching of the signal. Its value true if the signal switches fGain else it is false.  */
		ushort fRiseTime;/*!< Risetime of the siganl */
		ushort fDecayTime;/*!< Decaytime of the signal */
		ushort fSaturationTime;/*!< Saturation time */		
		ushort fMinimumPosition;
		ushort fMinimumPosition2;
		ushort fMaximumPosition;/*!< Sample number of the occurance of the Maximum value. */
		ushort Trace_Max_Val;/*!< the Maximum value of the current fTrace*/
		ushort fTrigger;/*!< Sample number of the occurance of the trigger. */
		double fCFDTime;/*!< Time obtained by using the digital constant fraction discriminator in digitalCFD class. */
		int fInflectionPoint;/*!< */
		int fReflectionPoint;/*!< */
		double fRCConstant;/*!< */
		bool fSaturated;/*!< */
		bool fNoisySignal;/*!< */
		int fFluctuationDistance;/*!< */
		std::map<std::pair<int, int >, int > fStripMap;
		std::map<int, std::pair<int, int > > fReverseStripMap;
		int* fDifferentialTrace;
		//private methods		
		double CalculateRCConstant();
		inline int CheckGainSwitching();
		void MapStripNumber();
		void Reset(){
			fDifferentialSignalAmplitude = 0;
			fGain = 0;
			fStripNumber = -1;
			fBaseline = 0.;
			fSignalIs = 0.;
			fNoise = 0.;//sigma
			fSignalHeight = 0.;//max val - baseline
			fRiseTime = 0;
			fDecayTime = 0;
			fSaturationTime = 0;		
			fMaximumPosition = 0;
			fMinimumPosition = 0;
			fMinimumPosition2 = 0;
			fTrigger = 0;
			fCFDTime = 0;
			fGain_switch_flag = 0;
			fInflectionPoint = 0;
			fReflectionPoint = 0;
			fRCConstant = 0.;
			fSaturated = 0;
			fNoisySignal = 0;
			fFluctuationDistance = 0;
		}
	public:
		DssdData();
		~DssdData();
		void PrintMapping();
		int GetBoardNumberOfStrip(int i);
		int GetChannelNumberOfStrip(int i);
		//Setters	
		void SetGain( ushort i){fGain = i;}
		void SetTraceValue(int i, ushort v){fTrace[i] = v;}
		void SetBaseline(double x){fBaseline = x;}
		void SetNoise(double x){ fNoise =x;}//sigma
		void SetSignalHeight(double x) { fSignalHeight =x;}//max val - baseline
		void SetRiseTime(ushort x){ fRiseTime =x;}
		void SetMaximumPosition( ushort x) { fMaximumPosition =x;}
		void SetTrigger (ushort x){ fTrigger =x;}
		void SetCFDTime(double x){ fCFDTime =x ;}

		// Getters
		ushort GetGain()const{return fGain;}
		ushort GetTraceValue(int i)const {return fTrace[i];}
		ushort GetAGTTraceValue(int i)const {return fAutoGainTraceTime[i];}
		ushort * GetTrace() {return fTrace;}
		double SignalIs()const{return fSignalIs;}
		double GetBaseline()const{return fBaseline;}
		double GetNoise()const{return fNoise;}//sigma
		ushort GetRiseTime()const{return fRiseTime;}
		ushort GetMaximumPosition()const{return fMaximumPosition;}
		ushort GetTrigger()const{return fTrigger;}
		double GetSignalHeight()const{return fSignalHeight;}//max val - baseline
		double GetCFDTime()const{return fCFDTime;}
		bool GainSwitched() const {return fGain_switch_flag;}
		double GetRCConstant() const {return fRCConstant;}
		bool IsSaturated() const {return fSaturated;}
		bool IsNoisy() const {return fNoisySignal;}
		void GetSignalInfo();
		int GetDssdBoardNumber(int *p_board);
		int GetStripNumber(int * const, int * const); 
		int GetStripNumber() const {return fStripNumber;} 
		//Get Addesses
		int* GetStripNumberAddress() {return &fStripNumber;} 
		double* GetBaselineAddress() {return &fBaseline;}
		ushort* GetTriggerAddress() {return &fTrigger;}
		ushort* GetMaximumPositionAddress() {return &fMaximumPosition;}
		double* GetNoiseAddress(){return &fNoise;}
		double* GetCFDTimeAddress(){return &fCFDTime;}
		ushort* GetRiseTimeAddress(){return &fRiseTime;} 
		int GetDifferentialSignalHeight() const{return fDifferentialSignalAmplitude;}
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
