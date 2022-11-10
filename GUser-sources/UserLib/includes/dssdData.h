/*!
 * \file dssdData.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */


#ifndef dssdData_h
#define dssdData_h 1
#include "numexo2Data.h"
#include "TMath.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class dssdData : public numexo2Data
{
	private:
		ushort gain; /*!< Gain from the data. */
		ushort *trace;/*!< 1D array for storing the trace. */
		//extracted data
		int stripnumber;/*!< strip number obtained using get_DSSD_strip_number(int *p_board, int *p_channel) */
		double Baseline;/*!< Baseline of the signal calculated for 200 points in GetSignalInfo()  */
		double signal_is;/*!< variable for the polarity of the signal. */
		double Noise;/*!< Noise of the signal. Noise = standard deviation calculated over 200 points in the begining of the trace.  */
		double signalHeight;/*!< Height of the signal is calculated by subtarcting the Baseline frim the maximum value. */
		bool gain_switch_flag;/*!< For checking the gain switching of the signal. Its value true if the signal switches gain else it is false.  */
		ushort RiseTime;/*!< Risetime of the siganl */
		ushort DecayTime;/*!< Decaytime of the signal */
		ushort SaturationTime;/*!< Saturation time */		
		ushort Max_pos;/*!< Sample number of the occurance of the Maximum value. */
		ushort Trigger;/*!< Sample number of the occurance of the trigger. */
		ullint cfd_time;/*!< Time obtained by using the digital constant fraction discriminator in digitalCFD class. */
		int inflection_point;/*!< */
		int reflection_point;/*!< */
		double RC_constant;/*!< */
		bool saturated;/*!< */
		bool noisy_signal;/*!< */
		int fluctuation_distance;/*!< */
		std::map<std::pair<int, int >, int > stripMap;
		int* diff_arr;
		//private methods		
		double calculate_RC_constant();
		inline int check_gain_switching();
		void map_stripNumber();
	public:
		dssdData();
		~dssdData();
		//Setters	
		void set_gain( ushort i){gain = i;}
		void set_trace_value(int i, ushort v){trace[i] = v;}
		void set_Baseline(double x){Baseline = x;}
		void set_Noise(double x){ Noise =x;}//sigma
		void set_signalHeight(double x) { signalHeight =x;}//max val - baseline
		void set_RiseTime(ushort x){ RiseTime =x;}
		void set_MaxPos( ushort x) { Max_pos =x;}
		void set_Trigger (ushort x){ Trigger =x;}
		void set_cfdTime(ullint x){ cfd_time =x ;}

		// Getters
		ushort get_gain()const{return gain;}
		ushort get_trace_value(int i)const {return trace[i];}
		ushort * get_trace() const {return trace;}
		double get_signal_is()const{return signal_is;}
		double get_Baseline()const{return Baseline;}
		double get_Noise()const{return Noise;}//sigma
		ushort get_RiseTime()const{return RiseTime;}
		ushort get_Max_pos()const{return Max_pos;}
		ushort get_Trigger()const{return Trigger;}
		double get_signalHeight()const{return signalHeight;}//max val - baseline
		ullint get_cfd_time()const{return cfd_time;}
		bool gain_switched() const {return gain_switch_flag;}
		double get_RC_constant() const {return RC_constant;}
		bool it_is_saturated() const {return saturated;}
		bool it_is_noisy() const {return noisy_signal;}
		void GetSignalInfo();
		int get_dssdBoardNumber(int *p_board);
		int get_stripnumber(int * const, int * const); 
		int get_stripnumber() const {return stripnumber;} 
//Get Addesses
	double* get_Baseline_address() {return &Baseline;}
		double* get_Noise_address(){return &Noise;}
		ushort* get_RiseTime_address(){return &RiseTime;} 
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
