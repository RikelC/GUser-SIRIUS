#ifndef dssdData_h
#define dssdData_h 1
#include "global.h"
#include "TMath.h"

class dssdData
{

	private:
		myGlobal *s1; 
		UShort_t gain;
		UInt_t eventnumber;
		ULong64_t timestamp;
		UShort_t *trace;
		int boardID;
		int boardIdx;
		int channelID;
		//extracted data
		int stripnumber;
		double Baseline;
		double signal_is;
		double Noise;//sigma
		double signalHeight;//max val - baseline
		UShort_t RiseTime;
		UShort_t Max_pos;
		UShort_t Trigger;
		double filt_Amplitude;
		ULong64_t cfd_time;
	public:
		dssdData();
		~dssdData();
		int get_DSSD_strip_number(int *p_board, int *p_channel);
		void set_eventnumber(UInt_t i){eventnumber =i;}



		void set_timestamp(ULong64_t i){timestamp =i;}
		void set_boardID(int i){boardID =i;}
		void set_boardIdx(int i){boardIdx =i;}
		void set_channelID(int i){channelID = i;}
		void set_gain( UShort_t i){gain = i;}
		void set_trace_value(int i, UShort_t v){trace[i] = v;}

		void set_Baseline(double x){Baseline = x;}
		void set_Noise(double x){ Noise =x;}//sigma
		void set_signalHeight(double x) { signalHeight =x;}//max val - baseline
		void set_RiseTime(UShort_t x){ RiseTime =x;}
		void set_MaxPos( UShort_t x) {Max_pos =x;}
		void set_Trigger (UShort_t x){ Trigger =x;}
		void set_filterAmplitude(double x) {filt_Amplitude = x;}
		void set_cfdTime(ULong64_t x){ cfd_time =x ;}


		UInt_t get_eventnumber()const {return eventnumber;}
		ULong64_t get_timestamp()const {return timestamp;}
		int get_boardID()const {return boardID;}
		int get_boardIdx()const {return boardIdx;}
		int get_channelID()const{return channelID;}
		UShort_t get_gain()const{return gain;}
		UShort_t get_trace_value(int i)const {return trace[i];}
		UShort_t* get_trace() const {return trace;}

		double get_signal_is()const{return signal_is;}
		double get_Baseline()const{return Baseline;}
		double get_Noise()const{return Noise;}//sigma
		double get_signalHeight()const{return signalHeight;}//max val - baseline
		UShort_t get_RiseTime()const{return RiseTime;}
		UShort_t get_Max_pos()const{return Max_pos;}
		UShort_t get_Trigger()const{return Trigger;}
		double get_filter_Amplitude()const{return filt_Amplitude;}
		ULong64_t get_cfd_time()const{return cfd_time;}
		int get_stripnumber() const {return stripnumber;}
		//address

		UInt_t* address_of_eventnumber() {return &eventnumber;}
		ULong64_t* address_of_timestamp() {return &timestamp;}
		int* address_of_boardID() {return &boardID;}
		int* address_of_channelID(){return &channelID;}
		UShort_t* address_of_gain(){return &gain;}
		UShort_t* address_of_trace(){return trace;}

		void GetSignalInfo();
};

#endif
