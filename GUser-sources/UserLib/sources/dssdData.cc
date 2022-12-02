/*!
 * \file dssdData.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */
#include "dssdData.h"
//! Constructor
/*! Get the instance of myGlobal class and initialize the variables.
*/
dssdData::dssdData(){
	map_stripNumber();
	trace = new ushort[s1->TRACE_SIZE];
	if(s1->trigger_def ==2)diff_arr = new int[s1->TRACE_SIZE];
	gain = 0;
	//extracted data
	stripnumber = -1;
	Baseline = 0.;
	signal_is = 0.;
	Noise = 0.;//sigma
	signalHeight = 0.;//max val - baseline
	RiseTime = 0;
	DecayTime = 0;
	SaturationTime = 0;		
	Max_pos = 0;
	Trigger = 0;
	cfd_time = 0;
	gain_switch_flag = 0;
	inflection_point = 0;
	reflection_point = 0;
	RC_constant = 0.;
	saturated = 0;
	noisy_signal = 0;
	fluctuation_distance = 0;

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
/*! Clear the heap memory.
*/
dssdData::~dssdData(){

	delete [] trace;
	stripMap.clear();	
	if(s1->trigger_def == 2) delete [] diff_arr;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Calculation of RC constant
/*!
 *
 *
 */
double dssdData::calculate_RC_constant(){
	//Determination of gain
	double y1, y2, tau=0.; int m =50;//50,60
	double range =0.;
	int rmin = Max_pos+20;
	int rmax = s1->TRACE_SIZE -m; 
	double tau_avg = 0;

	if(stripnumber >= 0 && stripnumber < 128) {
		if(s1->FRONT_FPCSA_GAIN_MODE == "HG"){
			m =s1->mv_window_calcul_HG;
			rmin = Max_pos + s1->skip_nSamples_calcul_HG;
		}
		else if(s1->FRONT_FPCSA_GAIN_MODE == "LG"){
			m =s1->mv_window_calcul_LG;
			rmin = Max_pos + s1->skip_nSamples_calcul_LG;

		}
	}
	else if(stripnumber > 127 && stripnumber < 255){ 
		if(s1->BACK_FPCSA_GAIN_MODE == "HG"){
			m =s1->mv_window_calcul_HG;
			rmin = Max_pos + s1->skip_nSamples_calcul_HG;
		}
		else if(s1->BACK_FPCSA_GAIN_MODE == "LG"){
			m =s1->mv_window_calcul_LG;
			rmin = Max_pos + s1->skip_nSamples_calcul_LG;

		}
	}
	if(gain_switch_flag){
		rmin = reflection_point +s1->skip_nSamples_calcul_LG;
		m =s1->mv_window_calcul_LG;// 30 good value for LG
	}

	for( int n = rmin; n < rmax; n++){

		y2 = TMath::Abs( trace[n+m] -Baseline);
		y1 = TMath::Abs( trace[n] - Baseline);

		if(y2/y1 == 1) continue;
		range++;
		tau = TMath::Abs((double)m/log(y2/y1));
		tau_avg += tau;

	}
	tau_avg /=range;

	RC_constant =  tau_avg*sampling_period;

	return RC_constant;

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Check gain switching
int dssdData::check_gain_switching(){
	int diff =0;  
	int m = 10; 	
	int max =0; int min =0; 
	int max_pos_diff = 0, min_pos_diff = 0;

	for(int n = 0; n < s1->TRACE_SIZE -10; n++){
		if(n >= m)
			diff = trace[n] - trace[n-m];

		if(n > 400 && n < 600){
			if(max < diff) { max = diff;  max_pos_diff = (int)n;}
			if(min > diff) { min = diff;  min_pos_diff = (int)n;}
		}
	}

	int r = max - min;

	gain_switch_flag = 0;

	if(stripnumber < 128 && r > s1->front_gainSwitch_threshold) gain_switch_flag = 1;

	else if(stripnumber > 127 && r > s1->back_gainSwitch_threshold) gain_switch_flag = 1;

	inflection_point =0;
	reflection_point =0;
	if(gain_switch_flag){

		//Find the dip in auto gain
		int cons_rise =0;
		int diff =0;
		ushort min_signal = trace[Max_pos];
		for( int n = Max_pos+1; n < Max_pos + 200; n++){
			diff =  min_signal - trace[n];
			if(signal_is <0.){		
				if(diff < 0) {min_signal = trace[n]; inflection_point = n; cons_rise =0;}
				else{ cons_rise++;if(cons_rise > 5) break;}
			}
			else {		
				if(diff > 0) {min_signal = trace[n]; inflection_point = n; cons_rise =0;}
				else{ cons_rise++;if(cons_rise > 5) break;}
			}

		}

		//cout<<"min s "<<inflection_point<<" min signal "<<min_signal<<endl;

		ushort max_signal = min_signal;
		if(signal_is > 0.){
			for(int n = inflection_point+1; n < inflection_point+100; n++){

				if(max_signal < trace[n] ){max_signal = trace[n]; reflection_point = n;}

			}
		}

		else{
			for(int n = inflection_point+1; n < inflection_point+100; n++){

				if( max_signal > trace[n] ){max_signal = trace[n]; reflection_point = n;}
			}
		}


		//transform
		for(int n = 0; n < s1->TRACE_SIZE; n++){
			if(n<reflection_point+s1->skip_nSamples_calcul_LG)trace[n] = Baseline;

		}
	}


	return r;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Measure observables from the trace 
void dssdData::GetSignalInfo(){
	//Reset the values
	stripnumber = get_stripnumber(&boardID,&channelID);
	Baseline = 0;
	signal_is=0;
	Noise=0;//sigma
	signalHeight=0;//max val - baseline
	RiseTime=0;
	Max_pos=0;
	Trigger=0;
	DecayTime =0;
	SaturationTime =0;
	int n1 = 10;
	int n2 = s1->TRACE_SIZE -200;
	//calculate baseline
	double temp =0.;
	ushort interval = 200;
	for (ushort i = n1; i < n1+interval; i++) {
		Baseline += (double)trace[i];
	}
	Baseline /=  (double)(interval);
	//calculate the noise
	for (ushort i = n1; i < n1+interval; i++) {
		temp = ((double)trace[i] - Baseline);
		Noise += temp*temp;
	}
	Noise = TMath::Sqrt(Noise/ (double)(interval));

	//determine the polarity of the signal
	n1 = 200; 	
	for(ushort n = n1; n < n2; n++){
		signal_is+=trace[n]- Baseline;
	}

	//Get max value and max pos
	ushort max_val = trace[0];
	ushort max_pos = 0;
	double min_temp =0.; ushort Min_pos =0;
	for (ushort i = n1; i < n2; i++) {
		if(signal_is > 0.){
			if(trace[i] > max_val){
				max_val = trace[i];
				max_pos = i;
			}else{
				temp = (double)trace[i] - Baseline;
				if(temp < 0){

					if(temp < min_temp){ min_temp = temp; Min_pos = i;}
				}
			}
		}
		else{
			if(trace[i] < max_val){
				max_val = trace[i];
				max_pos = i;
			}else{
				temp = (double)trace[i] - Baseline;
				if(temp > 0){

					if(temp > min_temp){ min_temp = temp; Min_pos = i;}
				}
			}
		}
	}
	signalHeight = TMath::Abs(double(max_val) - Baseline);
	Max_pos = max_pos;

	Trigger = Max_pos;
	if(s1->trigger_def ==2){

		int diff =0;  int m = 10;  
		for( int n = 0; n < s1->TRACE_SIZE; n++){
			if(n >= m)
				diff = trace[n] - trace[n-m];
			diff_arr[n] = diff;

		}
		//calculate the trigger
		for (ushort i = Max_pos; i >0; i--) {
			if(signal_is < 0.){
				if(diff_arr[i] > 0){
					Trigger = i; break;
				}
			}
			else{

				if(diff_arr[i]<0){Trigger = i; break;}
			}

		}

	}else{
		//calculate the trigger
		for (ushort i = Max_pos; i >0; i--) {
			temp =  TMath::Abs(static_cast<double>(trace[i]) - Baseline);
			if(temp <= 0.3*signalHeight){
				Trigger = i; break;
			}
		}
Trigger += 1;
	}	

	//calculate risetime
	if(s1->riseTime_def == 2){
		interval = Max_pos - Trigger;
		RiseTime = interval * sampling_period;
	}
	else{
		interval =0;
		temp = 0.;
		for(ushort i = Max_pos-1; i > 0; i--){
			temp = TMath::Abs(static_cast<double>(trace[i]) - Baseline);
			if(temp >= 0.1*signalHeight && temp <= 0.9*signalHeight)
				interval++;
			if(temp <0.1*signalHeight) break;
		}

		RiseTime = interval * sampling_period;

	}

	//calculate saturation time
	interval =0;
	temp = 0.;
	for(ushort i = Max_pos-1; i > 0; i--){
		temp = TMath::Abs(static_cast<double>(trace[i]) - Baseline);
		if(temp >= 0.9*signalHeight){
			interval++;
		}
		else break;
	}

	for(ushort i = Max_pos+1; i < s1->TRACE_SIZE; i++){
		temp =TMath::Abs(static_cast<double>(trace[i]) - Baseline);
		if(temp >= 0.9*signalHeight){
			interval++;
		}
		else break;
	}

	interval +=1; //include Max_pos

	SaturationTime = interval *sampling_period;


	if(SaturationTime > s1->saturationTime_threshold) saturated = 1;
	else saturated = 0;


	//	if( boardID != 192 && channelID != 12){
	if(stripnumber < 128 && s1->FRONT_FPCSA_GAIN_MODE == "AUTO")
		check_gain_switching();
	else if(stripnumber > 127 && s1->BACK_FPCSA_GAIN_MODE == "AUTO")
		check_gain_switching();

	//	}


	if(s1->use_default_FPCSA_Gain == false)	RC_constant = calculate_RC_constant();
	//calculate decaytime
	interval =0;
	temp = 0.;
	for(ushort i = Max_pos+1; i < s1->TRACE_SIZE; i++){
		temp = TMath::Abs(static_cast<double>(trace[i]) - Baseline);
		if(temp >= 0.1*signalHeight && temp <= 0.9*signalHeight){
			interval++;
		}
		if(temp <0.1*signalHeight) break;

	}
	DecayTime = interval * sampling_period;

	fluctuation_distance = Max_pos - Min_pos;
	if(DecayTime > s1->decayTime_threshold && fluctuation_distance > 0&& fluctuation_distance < 100) noisy_signal = 1;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//!  board number
int dssdData::get_dssdBoardNumber(int *p_board){
	return s1->boardIndex_DSSD[*p_board];
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
/*! Strip number map
 *
 */
void dssdData::map_stripNumber(){
	std::vector<std::string> token = split_string(s1->dssdFormat,",");
	int board, strip, strip1, strip2, channel1, channel2;
	for(int i = 0; i< token.size(); i++){
		sscanf(token.at(i).c_str(), "%d:%d-%d:%d-%d", &board, &channel1, &channel2, &strip1, &strip2);
		strip = strip1;
		for(int j = channel1; j<= channel2;j++){
			std::pair<int, int> key = std::make_pair(board,j);
			stripMap[key] = strip;
			strip++;
		}
	}
	token.clear();
	s1->dssdFormat.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Strip number
int dssdData::get_stripnumber(int * const p_board, int * const p_channel){
	int stripNo =-10;
	std::pair<int, int> key = std::make_pair(*p_board, *p_channel);
	stripNo = stripMap[key];
	return stripNo;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
/********************************************************

  DSSD Routing
  ---------------------     


  physical
  strip      0--------------------------->127
  ------
  MB
  strip      64<-----------1------------->64            128 
  | P5 | P4 |=======| P5 | P4 |      1        ^
  ___________________________  ____ ^        |
  |   MB1           MB2       |  P4  |        |
  |                           | ____ |        |
  |                       MB3 |  P5  |        |
  |            DSSD           | ____ |        |
  |                           |               |
  |                           |      64       |
  |            Back           | ____ ^        |
  |                           |  P4  |        |
  |                       MB4 | ____ |        |
  |                           |  P5  |        | 
  |___________________________| ____ |       255
  1
  MB strip  physical strip

 *******************************************************/


