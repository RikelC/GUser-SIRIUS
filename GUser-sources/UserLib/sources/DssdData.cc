/*!
 * \file DssdData.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */
#include "DssdData.h"
//! Constructor
/*! Get the instance of myGlobal class and initialize the variables.
*/
DssdData::DssdData(){
	MapStripNumber();
	//if(s1->fReverseStrip)
	ReversePhysicalStripDirection();
	//fTrace = new ushort[s1->TRACE_SIZE];
	if(s1->trigger_def ==2)fDifferentialTrace = new int[s1->TRACE_SIZE];
	fGain = 0;
	//extracted data
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
	fRCConstant = 600.;
	fSaturated = 0;
	fNoisySignal = 0;
	fFluctuationDistance = 0;
	for(int i=0; i < 992; i++){
		fTrace[i]=0;
		//		autoGainTraceEnergy[i]=0;
		fAutoGainTraceTime[i]=0;
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
/*! Clear the heap memory.
*/
DssdData::~DssdData(){
	//	delete [] fTrace;
	fStripMap.clear();	
	fReverseStripMap.clear();	
	if(s1->trigger_def == 2) delete [] fDifferentialTrace;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Calculation of RC constant
/*!
 *
 *
 */
double DssdData::CalculateRCConstant(){
	//Determination of fGain
	double y1, y2, tau=0.; int m =50;//50,60
	double range =0.;
	int rmin = fMaximumPosition+20;
	int rmax = s1->TRACE_SIZE -m; 
	double tau_avg = 0;

	if(fStripNumber >= 0 && fStripNumber < 128) {
		if(s1->FRONT_FPCSA_GAIN_MODE == "HG"){
			m =s1->mv_window_calcul_HG;
			rmin = fMaximumPosition + s1->skip_nSamples_calcul_HG;
		}
		else if(s1->FRONT_FPCSA_GAIN_MODE == "LG"){
			m =s1->mv_window_calcul_LG;
			rmin = fMaximumPosition + s1->skip_nSamples_calcul_LG;

		}
	}
	else if(fStripNumber > 127 && fStripNumber < 255){ 
		if(s1->BACK_FPCSA_GAIN_MODE == "HG"){
			m =s1->mv_window_calcul_HG;
			rmin = fMaximumPosition + s1->skip_nSamples_calcul_HG;
		}
		else if(s1->BACK_FPCSA_GAIN_MODE == "LG"){
			m =s1->mv_window_calcul_LG;
			rmin = fMaximumPosition + s1->skip_nSamples_calcul_LG;

		}
	}
	if(fGain_switch_flag){
		rmin = fReflectionPoint +s1->skip_nSamples_calcul_LG;
		m =s1->mv_window_calcul_LG;// 30 good value for LG
	}

	for( int n = rmin; n < rmax; n++){

		y2 = TMath::Abs( fTrace[n+m] -fBaseline);
		y1 = TMath::Abs( fTrace[n] - fBaseline);

		if(y2/y1 == 1) continue;
		range++;
		tau = TMath::Abs((double)m/log(y2/y1));
		tau_avg += tau;

	}
	tau_avg /=range;

	fRCConstant =  tau_avg*sampling_period;

	return fRCConstant;

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Check fGain switching
int DssdData::CheckGainSwitching(){
	int diff =0;  
	int m = s1->mv_window_calcul_gainSwitch;	
	int max =0; int min =0; 
	int max_pos_diff = 0, min_pos_diff = 0;
	fGain_switch_flag = false;
	for(int n = 0; n < s1->TRACE_SIZE -10; n++){
		if(n >= m)
			diff = static_cast<int>(fTrace[n] - fTrace[n-m]);

		if(n > 300 && n < 700){
			if(max < diff) { max = diff;  max_pos_diff = (int)n;}
			if(min > diff) { min = diff;  min_pos_diff = (int)n;}
		}
	}

	int r = ( max - min);
	int rp = ( max_pos_diff - min_pos_diff);
	fDifferentialSignalAmplitude = r;
	//r = (int)TMath::Abs(fSignalHeight - fBaseline);
	int diff_extremum = static_cast<int>(fMinimumPosition2 - fMaximumPosition);

	fGain_switch_flag = 0;
	if(fSignalIs < 0){
		if(TMath::Abs(diff_extremum) < s1->front_gs_duration && r > s1->front_gainSwitch_threshold) fGain_switch_flag = 1;
	}
	else{
		if(TMath::Abs(diff_extremum) < s1->back_gs_duration && r > s1->back_gainSwitch_threshold) fGain_switch_flag = 1;
	}
	//if(fGain_switch_flag)cout<<" fGain switched .. diff "<<diff_extremum<<" r "<<r<<"  rp "<<rp<<endl;
	//else cout<<"  not switched .. diff "<<diff_extremum<<" r "<<r<<"  rp "<<rp<<endl;

	/*
	//Calculate the slope
	double y1 = 0.;
	double y2 = 0.;

	fGain_switch_flag = 0;
	double slope = 0., avg_slope=0.;
	int interval =1, counter=0;
	for(int n = fMaximumPosition; n < fMaximumPosition+20; n=n+interval){

	y1 = static_cast<double>(fTrace[n] - fBaseline);
	y2 = static_cast<double>(fTrace[n+interval] - fBaseline);
	slope = static_cast<double>((y2-y1)/(interval));
	if(fSignalIs > 0){
	if(slope <= 0){
	counter++;
	avg_slope +=slope;
	}
	else break;
	}else{

	if(slope >= 0){
	avg_slope +=slope;
	counter++;
	}
	else break;

	}
	}
	avg_slope/= static_cast<double>(interval*counter);
	//	cout<<"avg slope "<<avg_slope<<"  signal is "<<fSignalIs<<endl;
	if(fSignalIs < 0){
	if(avg_slope > 10 && TMath::Abs(fSignalHeight) > s1->front_fGainSwitch_threshold) fGain_switch_flag = 1;
	}
	else{
	if(avg_slope > 10 && TMath::Abs(fSignalHeight) > s1->back_fGainSwitch_threshold) fGain_switch_flag = 1;
	}
	//if(fGain_switch_flag)cout<<" fGain switched .. slope "<<avg_slope<<" height "<<fSignalHeight<<"  d "<<interval*counter<<endl;
	//else cout<<"  not switched .. slope "<<avg_slope<<"  signal height "<<fSignalHeight<<"  d "<<interval*counter<<endl;
	*/


	fInflectionPoint =0;
	fReflectionPoint =0;
	if(fGain_switch_flag){

		//Find the dip in auto fGain
		int cons_rise =0;
		int diff =0;
		ushort min_signal = fTrace[fMaximumPosition];
		for( int n = fMaximumPosition+1; n < fMaximumPosition + 200; n++){
			diff =  min_signal - fTrace[n];
			if(fSignalIs <0.){		
				if(diff < 0) {min_signal = fTrace[n]; fInflectionPoint = n; cons_rise =0;}
				else{ cons_rise++;if(cons_rise > 5) break;}
			}
			else {		
				if(diff > 0) {min_signal = fTrace[n]; fInflectionPoint = n; cons_rise =0;}
				else{ cons_rise++;if(cons_rise > 5) break;}
			}

		}

		//cout<<"min s "<<fInflectionPoint<<" min signal "<<min_signal<<endl;


		ushort max_signal = min_signal;
		if(fSignalIs > 0.){
			for(int n = fInflectionPoint+1; n < fInflectionPoint+100; n++){

				if(max_signal < fTrace[n] ){max_signal = fTrace[n]; fReflectionPoint = n;}

			}
		}

		else{
			for(int n = fInflectionPoint+1; n < fInflectionPoint+100; n++){

				if( max_signal > fTrace[n] ){max_signal = fTrace[n]; fReflectionPoint = n;}
			}
		}


		for(int n = 0; n < s1->TRACE_SIZE; n++){
			if(n<=fMaximumPosition)fAutoGainTraceTime[n] = fTrace[n];
			else{
				fAutoGainTraceTime[n] = Trace_Max_Val; 
				/*
				   if(fStripNumber < 128)
				   fRCConstant = Resistance* s1->frontHighGain;
				   else
				   fRCConstant = Resistance* s1->backHighGain;

				   if(fSignalIs > 0)
				   fAutoGainTraceTime[n] =  fBaseline + fSignalHeight * exp(- (n - fMaximumPosition)*sampling_period/ fRCConstant);
				   else
				   fAutoGainTraceTime[n] =  fBaseline - fSignalHeight * exp(- (n - fMaximumPosition)*sampling_period/ fRCConstant);
				   */
			}
		}




		//transform the fTrace for energy calculation
		for(int n = 0; n < s1->TRACE_SIZE; n++){
			if(n<fReflectionPoint+s1->skip_nSamples_calcul_LG)fTrace[n] = fBaseline;
		}
		//transform for time calculation
		/*		if(fMaximumPosition ==0){              
		//Get max value and max pos
		Trace_Max_Val = fTrace[0];
		for (ushort i = 0; i < s1->TRACE_SIZE; i++) {
		if(fSignalIs > 0.){
		if(fTrace[i] > Trace_Max_Val){
		Trace_Max_Val = fTrace[i];
		fMaximumPosition = i;
		}
		}
		else{
		if(fTrace[i] < Trace_Max_Val){
		Trace_Max_Val = fTrace[i];
		fMaximumPosition = i;
		}
		}
		}
		}
		*/
	}


	return r;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Measure observables from the fTrace 
void DssdData::GetSignalInfo(){
	//Reset the values
	Reset();



	/*********

	  Do after
	  */

	fStripNumber = GetStripNumber(&fBoard,&fChannel);

	int n1 = 10;
	int n2 = s1->TRACE_SIZE -200;
	//calculate baseline
	double temp =0.;
	ushort interval = 150;
	for (ushort i = n1; i < n1+interval; i++) {
		fBaseline += (double)fTrace[i];
	}
	fBaseline /=  (double)(interval);
	//calculate the noise
	for (ushort i = n1; i < n1+interval; i++) {
		temp = ((double)fTrace[i] - fBaseline);
		fNoise += temp*temp;
	}
	fNoise = TMath::Sqrt(fNoise/ (double)(interval));

	//determine the polarity of the signal
	n1 = 200; 	
	for(ushort n = n1; n < n2; n++){
		fSignalIs+=fTrace[n]- fBaseline;
	}

	//Get max value and max pos
	ushort max_val = fTrace[0];
	ushort max_pos = 0;
	double min_temp =0.; 
	for (ushort i = n1; i < n2; i++) {
		if(fSignalIs > 0.){
			if(fTrace[i] > max_val){
				max_val = fTrace[i];
				max_pos = i;
			}else{
				temp = (double)fTrace[i] - fBaseline;
				if(temp < 0){

					if(temp < min_temp){ min_temp = temp; fMinimumPosition = i;}
				}
			}
		}
		else{
			if(fTrace[i] < max_val){
				max_val = fTrace[i];
				max_pos = i;
			}else{
				temp = (double)fTrace[i] - fBaseline;
				if(temp > 0){

					if(temp > min_temp){ min_temp = temp; fMinimumPosition = i;}
				}
			}
		}
	}
	fSignalHeight = TMath::Abs(double(max_val) - fBaseline);
	fMaximumPosition = max_pos;
	Trace_Max_Val = max_val;
	// Min pos 2
	//

	double min_val = fSignalHeight;
	temp =0.; 
	for (ushort i = fMaximumPosition; i < fMaximumPosition+30; i++) {
		temp = TMath::Abs( fTrace[i] - fBaseline);
		if(temp < min_val){
			min_val = temp;
			fMinimumPosition2 = i;
		}
	}

	if(s1->trigger_def ==2){

		int diff =0;  int m = 10;  
		for( int n = 0; n < s1->TRACE_SIZE; n++){
			if(n >= m)
				diff = fTrace[n] - fTrace[n-m];
			fDifferentialTrace[n] = diff;

		}
		//calculate the trigger
		for (ushort i = fMaximumPosition; i >0; i--) {
			if(fSignalIs < 0.){
				if(fDifferentialTrace[i] > 0){
					fTrigger = i; break;
				}}
			else{

				if(fDifferentialTrace[i]<0){fTrigger = i; break;}
			}

		}

	}else{
		//calculate the trigger
		for (ushort i = fMaximumPosition; i >0; i--) {
			temp =  TMath::Abs(static_cast<double>(fTrace[i]) - fBaseline);
			if(temp <= 0.3*fSignalHeight){
				fTrigger = i; break;
			}
		}

	}	

	//calculate risetime
	if(s1->riseTime_def == 2){
		interval = fMaximumPosition - fTrigger;
		fRiseTime = interval * sampling_period;
	}
	else{
		interval =0;
		temp = 0.;
		for(ushort i = fMaximumPosition-1; i > 0; i--){
			temp = TMath::Abs(static_cast<double>(fTrace[i]) - fBaseline);
			if(temp >= 0.1*fSignalHeight && temp <= 0.9*fSignalHeight)
				interval++;
			if(temp <0.1*fSignalHeight) break;
		}

		fRiseTime = interval * sampling_period;

	}

	//calculate saturation time
	interval =0;
	temp = 0.;
	for(ushort i = fMaximumPosition-1; i > 0; i--){
		temp = TMath::Abs(static_cast<double>(fTrace[i]) - fBaseline);
		if(temp >= 0.9*fSignalHeight){
			interval++;
		}
		else break;
	}

	for(ushort i = fMaximumPosition+1; i < s1->TRACE_SIZE; i++){
		temp =TMath::Abs(static_cast<double>(fTrace[i]) - fBaseline);
		if(temp >= 0.9*fSignalHeight){
			interval++;
		}
		else break;
	}

	interval +=1; //include fMaximumPosition

	fSaturationTime = interval *sampling_period;


	if(fSaturationTime > s1->saturationTime_threshold) fSaturated = 1;
	else fSaturated = 0;



	//skip fGain swiching for tracker

	if( fBoard != s1->trackerNumexoBoard ){
		if(fStripNumber < 128 && s1->FRONT_FPCSA_GAIN_MODE == "AUTO")
			CheckGainSwitching();
		else if(fStripNumber > 127 && fStripNumber < 256 && s1->BACK_FPCSA_GAIN_MODE == "AUTO")
			CheckGainSwitching();

	}


	if(s1->use_default_FPCSA_Gain == false)	fRCConstant = CalculateRCConstant();
	//calculate decaytime
	interval =0;
	temp = 0.;
	for(ushort i = fMaximumPosition+1; i < s1->TRACE_SIZE; i++){
		temp = TMath::Abs(static_cast<double>(fTrace[i]) - fBaseline);
		if(temp >= 0.1*fSignalHeight && temp <= 0.9*fSignalHeight){
			interval++;
		}
		if(temp <0.1*fSignalHeight) break;

	}
	fDecayTime = interval * sampling_period;

	fFluctuationDistance = fMaximumPosition - fMinimumPosition;
	if(fDecayTime > s1->decayTime_threshold && fFluctuationDistance > 0&& fFluctuationDistance < 100) fNoisySignal = 1;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//!  board number
int DssdData::GetDssdBoardNumber(int *p_board){
	return s1->boardIndex_DSSD[*p_board];
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
/*! Strip number map
 *
 */
void DssdData::MapStripNumber(){
	//cout<<"MapStripNumber"<<endl;
	std::vector<std::string> token = SplitString(s1->dssdFormat,",");
	int board, strip, strip1, strip2, channel, channel1, channel2;
	for(int i = 0; i< token.size(); i++){
		sscanf(token.at(i).c_str(), "%d:%d-%d:%d-%d", &board, &channel1, &channel2, &strip1, &strip2);
		//cout<<" In board "<<board<<" channel1 = "<<channel1<<" channel2 = "<<channel2<<endl;
		strip = strip1;
		if(board == 180 || board == 181 || board == 169 || board == 165 || board == 164 || board == 192 || board == 178 || board ==176){
			//		cout<<"in select"<<endl;
			for(int j = channel2; j>= channel1;j--){

				std::pair<int, int> key = std::make_pair(board,j);
				fStripMap[key] = strip;
				fReverseStripMap[strip] = key;
				//cout<<"board "<<board<<" j = "<<j<<" strip"<<strip<<endl;
				strip++;
			}
		}else{	
			for(int j = channel1; j<= channel2;j++){
				std::pair<int, int> key = std::make_pair(board,j);
				fStripMap[key] = strip;
				fReverseStripMap[strip] = key;
				strip++;
			}
		}

	}
	token.clear();
	s1->dssdFormat.clear();

}	

void DssdData::ReversePhysicalStripDirection(){
	//0------------->127
	//becomes
	//127----------->0
	//128----------->255
	//becomes
	//255----------->128
	// And keep the rest same

	int strip =0, newStrip =0;
	int board =0, channel =0;
	for(std::map<std::pair<int, int >, int >::iterator it = fStripMap.begin();it !=fStripMap.end();it++){
		strip = it->second;
		board = it->first.first;
		channel = it->first.second;
		std::pair<int, int> key = std::make_pair(board,channel);
		if(strip >= 0 && strip < 128)
			newStrip = (127 - strip);
		else if(strip > 127 && strip < 256) 
			newStrip = 255 - (strip - 128);
		else  newStrip = strip;
		fStripMap[key] = newStrip;
		fReverseStripMap[newStrip] = key;

	}
}

void DssdData::PrintMapping(){
	for(std::map<std::pair<int, int >, int >::iterator it = fStripMap.begin();it !=fStripMap.end();it++){

		cout<<GREEN<<"Board: "<<RESET<<it->first.first<<BLUE<< "  Channel: "<<RESET<<it->first.second << RED<<"  Stripnumber: "<<it->second<<RESET<<endl;
	}
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Strip number
int DssdData::GetStripNumber(int * const p_board, int * const p_channel){
	int stripNo =-10;
	std::pair<int, int> key = std::make_pair(*p_board, *p_channel);
	stripNo = fStripMap[key];
	return stripNo;
}

//! Board number
int DssdData::GetBoardNumberOfStrip(int strip){
	return fReverseStripMap.find(strip)->second.first;
}
//! Board number
int DssdData::GetChannelNumberOfStrip(int strip){
	return fReverseStripMap.find(strip)->second.second;
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


