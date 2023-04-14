/*!
 * \file digitalFilters.cc
 * \author Rikel CHAKMA
 * \brief Digital pulse processing algorithms. 
 * \details The trapezoidal pulse processing algorithms are based on Ref. Valentin T.Jordanov et al., NIM, 353, 261-264 (1994). https://doi.org/10.1016/0168-9002(94)91652-7
 */


#include "digitalFilters.h"
//! Constructor
digitalFilters::digitalFilters(){

	s1 = myGlobal::getInstance();
	Capacitance = 1.;//in pF
	RC_constant = 700.;
	trapezoidal_shaper_M = 0. ;
	trapezoidal_shaper_m2 = 0.;
	trapezoidal_shaper_m1 = 0.;
	array_u = new double[s1->TRACE_SIZE];
	array_v = new double[s1->TRACE_SIZE];
	array_d = new double[s1->TRACE_SIZE];
	array_p = new double[s1->TRACE_SIZE];
	Rectangular = new double[s1->TRACE_SIZE];
	Trapezoidal = new double[s1->TRACE_SIZE];
	kPar = new ushort*[s1->NBOARDS_DSSD];
	mPar = new ushort*[s1->NBOARDS_DSSD];
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		kPar[i] = new ushort[NCHANNELS];
		mPar[i] = new ushort[NCHANNELS];
		for(int j = 0; j < NCHANNELS; j++){
			kPar[i][j] = 200;
			mPar[i][j] = 50;
		}
	}
	if(s1->trap_flattop_mode == 4){
		diff_arr = new double[s1->TRACE_SIZE];
		diff_arr2 = new double[s1->TRACE_SIZE];
	}	
	MA = new double [s1->TRACE_SIZE];
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
digitalFilters::~digitalFilters(){
	delete [] array_u;
	delete [] array_v;
	delete [] array_d;
	delete [] array_p;
	delete [] Rectangular;
	delete [] Trapezoidal;
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		delete [] kPar[i];
		delete [] mPar[i];
	}
	delete [] kPar;
	delete [] mPar;
	if(s1->trap_flattop_mode == 4){
		delete [] diff_arr;
		delete [] diff_arr2;
	}
	delete [] MA;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get simple moving average of N points on the subset [nStart, nEnd) of an array of data points v.
template<class T>
void digitalFilters::get_moving_average_signal(T* const v, int N, int nStart, int nEnd){
	for(int n = nStart; n < nEnd;n++){

		if(n < nStart+N)
			MA[n] = (double)v[n]; 
		else
			MA[n] = MA[n-1] + (double)(v[n] - v[n-N])/(double)N;

	}
	for(int i =nStart; i < nEnd;i++)v[i] = MA[i];

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get the start stop range of the trapezoidal flat top.
/*!
 *
 */
template<class T>
int * digitalFilters::get_start_stop_range(T* const v, double signal_is, int m = 10){

	T diff =0;  T max_diff2 =0.; 
	for( int n = 0; n < s1->TRACE_SIZE; n++){
		if(n >= m)
			diff = v[n] - v[n-m];
		diff_arr[n] = diff;

	}
	//------------------
	// double diff
	for( int n = 0; n < s1->TRACE_SIZE; n++){
		if(n >= m)
			diff = diff_arr[n] - diff_arr[n-m];
		diff_arr2[n] = diff;
		if(signal_is > 0. && max_diff2 > diff_arr2[n]){
			max_diff2 = diff_arr2[n];
		}	
		if(signal_is < 0. && max_diff2 < diff_arr2[n]){
			max_diff2 = diff_arr2[n];
		}	
	}

	double threshold =  0.5*max_diff2;
	double left, right; int up_count =0, down_count =0, neg_peak_count =0, pos_peak_count =0;
	// find the minima points
	int min1_pos=0, min2_pos =0;

	if(signal_is > 0.){
		for( int n = 0; n < s1->TRACE_SIZE; n++){
			if(diff_arr2[n] < threshold){
				left = diff_arr2[n-1] - diff_arr2[n];
				right = diff_arr2[n+1] - diff_arr2[n];
				if(left > 0. && right < 0.){
					//downward
					down_count++;
				}
				else if(left > 0. && right > 0.){
					//change of sign (-ve peak)
					neg_peak_count++;
					if(signal_is > 0.){
						if(neg_peak_count ==1){
							min1_pos =n;
						}
						else if(neg_peak_count ==2){
							min2_pos =n;
						}
					}
					down_count =0;
					up_count =0;
				}
				else if(left < 0. && right > 0.){
					//upward
					up_count++;
				}
				else if(left < 0. && right < 0.){
					//change of sign (+ve peak)
					pos_peak_count++;
					down_count =0;
					up_count =0;
				}
			}
		}

	}else{
		for( int n = 0; n < s1->TRACE_SIZE; n++){
			if(diff_arr2[n] > threshold){
				left = diff_arr2[n-1] - diff_arr2[n];
				right = diff_arr2[n+1] - diff_arr2[n];
				if(left > 0. && right < 0.){
					//downward
					down_count++;
				}
				else if(left > 0. && right > 0.){
					//change of sign (-ve peak)
					neg_peak_count++;
					down_count =0;
					up_count =0;
				}
				else if(left < 0. && right > 0.){
					//upward
					up_count++;
				}
				else if(left < 0. && right < 0.){
					//change of sign (+ve peak)
					pos_peak_count++;

					if(pos_peak_count ==1){
						min1_pos =n;
					}
					else if(pos_peak_count ==2){
						min2_pos =n;
					}
					down_count =0;
					up_count =0;
				}
			}
		}
	}

	//------------
	int start = min1_pos;
	int stop = min2_pos-2*m;

	static int points[2];
	points[0] = start; points[1] =stop;

	return points;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Assignment of Optimized Trapezoidal parameters
/*! Read the optimized trapezoidal parameter file and assign the 2D arrays k[board][channel] and m[board][channel].
 *
 */
void digitalFilters::assign_k_m_values(){
	std::string line; ushort boardID, bIdx, chID, k_val, m_val;

	std::ifstream myfile;
	myfile.open(s1->trap_parameter_filename,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> k_val >> m_val;
			bIdx = s1->boardIndex_DSSD[boardID];
			kPar[bIdx][chID] = k_val;
			mPar[bIdx][chID] = m_val;
		}
		myfile.close();
	}else{std::cout<<s1->trap_parameter_filename<<" cannot be opened!!!"<<std::endl;}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------
// Setting FPCSA gain 
//--------------------------
void digitalFilters::set_DSSD_gain(double gain){
	Capacitance = gain; RC_constant =  Resistance*Capacitance;
	trapezoidal_shaper_M = pow((exp(sampling_period/RC_constant) - 1.), -1.);
	trapezoidal_shaper_m2 = s1->shaperAmplificationGain; 
	trapezoidal_shaper_m1 =  trapezoidal_shaper_m2 *  trapezoidal_shaper_M;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------
// Set RC constant
//--------------------------
void digitalFilters::set_RC_constant(double  r){
	RC_constant = r;
	trapezoidal_shaper_M = pow((exp(sampling_period/RC_constant) - 1.), -1.);
	trapezoidal_shaper_m2 = s1->shaperAmplificationGain; 
	trapezoidal_shaper_m1 =  trapezoidal_shaper_m2 *  trapezoidal_shaper_M;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------
// Get RC constant
//--------------------------
void digitalFilters::get_RC_constant(dssdData* const data){
	if(s1->use_default_FPCSA_Gain){
		if(data->get_stripnumber() >=0 && data->get_stripnumber() < 128){
			if(s1->FRONT_FPCSA_GAIN_MODE == "HG") RC_constant =  Resistance * s1->frontHighGain;
			if(s1->FRONT_FPCSA_GAIN_MODE == "LG") RC_constant =  Resistance * s1->frontLowGain;
			if(s1->FRONT_FPCSA_GAIN_MODE == "AUTO"){ 
				if(data->gain_switched())
					RC_constant =  Resistance * s1->frontLowGain;
				else
					RC_constant =  Resistance * s1->frontHighGain;
			}

		}
		else if(data->get_stripnumber() >127 && data->get_stripnumber() < 256){
			if(s1->BACK_FPCSA_GAIN_MODE == "HG") RC_constant =  Resistance * s1->backHighGain;
			if(s1->BACK_FPCSA_GAIN_MODE == "LG") RC_constant =  Resistance * s1->backLowGain;
			if(s1->BACK_FPCSA_GAIN_MODE == "AUTO"){ 
				if(data->gain_switched())
					RC_constant =  Resistance * s1->backLowGain;
				else
					RC_constant =  Resistance * s1->backHighGain;
			}

		}
	}else{
		RC_constant =  data->get_RC_constant();
	}


	trapezoidal_shaper_M = pow((exp(sampling_period/RC_constant) - 1.), -1.);
	trapezoidal_shaper_m2 = s1->shaperAmplificationGain; 
	trapezoidal_shaper_m1 =  trapezoidal_shaper_m2 *  trapezoidal_shaper_M;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Calculate max value of the trapezoidal automatically
double digitalFilters::get_auto_max_val_trapezoidal(double * const v){

	int *arr = get_start_stop_range(v,5);
	double height =0.;

	for(int i = arr[0];i<= arr[1];i++){

		height += v[i];

	}
	height /= (double)(arr[1]-arr[0] +1);

	return height; 
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get max value of the trapezodal1
double digitalFilters::get_max_val_trapezoidal1( double signal_is, double* const v , ushort max_pos, double max_val){
	//average on both sides of the max position
	double maxR =0.; double maxL = 0.;double rR =0.; double rL =0.;
	ushort j2 = max_pos + 1;
	ushort j1 = max_pos -1;
	if(signal_is > 0.){
		while(v[j2] > s1->height_fraction_MaxTrapHeight  * max_val){
			maxR += v[j2];
			j2++;
			rR++;
		}

		while(v[j1] > s1->height_fraction_MaxTrapHeight * max_val){
			maxL += v[j1];
			j1--;
			rL++;
		}

	}else{
		while(v[j2] <  s1->height_fraction_MaxTrapHeight*  max_val){
			maxR += v[j2];
			j2++;
			rR++;
		}
		while(v[j1] < s1->height_fraction_MaxTrapHeight * max_val){
			maxL += v[j1];
			j1--;
			rL++;
		}

	}
	//compute average
	max_val = max_val + maxL + maxR;
	max_val = max_val /(rR+rL+1.);
	//-------------
	return max_val;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get Max value of the trapezoidal2
double digitalFilters::get_max_val_trapezoidal2(dssdData* const data, double* const v , ushort k, ushort l){
	ushort start = data->get_Trigger()  + k;
	ushort stop = data->get_Trigger() + l;

	double avg =0.;

	for(ushort i = start; i <= stop; i++){
		avg += v[i];
	}

	avg /= (double)(stop -start +1);
	return avg;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get Max value of the trapezoidal3
double digitalFilters::get_max_val_trapezoidal3(  dssdData* const data, double* const v , ushort k, ushort l){

	ushort risetime = data->get_RiseTime()/sampling_period;
	ushort start =0;

	if(k >= risetime)
		start = data->get_Trigger()  + k;
	else if(risetime > k && risetime < l){

		start = data->get_Trigger()  + risetime;

	}
	else start = data->get_Trigger() + l;

	ushort stop = data->get_Trigger() + l;

	double avg =0.;

	for(ushort i = start; i <= stop; i++){
		avg += v[i];
	}

	avg /= (double)(stop -start +1);
	return avg;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 1
/*!
 *
 *
 */
double digitalFilters::trapezoidal_filter_algorithm1(  dssdData* const data, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos =0;
	double signalAmplitude = 0.;
	//get k and m parameters
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	ushort k = kPar[b][ch];
	ushort m = mPar[b][ch];
	ushort l = k+m;

	get_RC_constant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if(n < k){
			array_u[n] = (double)data->get_trace_value(n) - data->get_Baseline();
			array_v[n] = (double)data->get_trace_value(n) - data->get_Baseline();
		}

		else{
			array_u[n] = (double)(data->get_trace_value(n) - (double)data->get_trace_value(n-k));
		}

		if(n >= l+k){
			array_v[n] = (double)(data->get_trace_value(n-l) - (double)data->get_trace_value(n-l-k));
		}

		array_d[n] = array_u[n] - array_v[n];

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /RC_constant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){
			signalAmplitude = Trapezoidal[n]/(double)k;
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here
		if(data->get_signal_is() > 0.){
			if(Trapezoidal[n] > max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}else{
			if(Trapezoidal[n] < max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}


	}

	//-------------
	if(s1->trap_flattop_mode == 1)
		max_val = get_max_val_trapezoidal1( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = get_max_val_trapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = get_max_val_trapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = get_auto_max_val_trapezoidal(Trapezoidal);
	//-----------------

	signalAmplitude = max_val /(double)(k);

	data->set_raw_energy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 1 
/*!
 *
 *
 */
double digitalFilters::trapezoidal_filter_algorithm1(  dssdData* const data, ushort k , ushort m, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	double signalAmplitude = 0.;
	ushort l = k+m;
	get_RC_constant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){
		if(n < k){
			array_u[n] = (double)data->get_trace_value(n) - data->get_Baseline();
			array_v[n] = (double)data->get_trace_value(n) - data->get_Baseline();
		}

		else{
			array_u[n] = (double)(data->get_trace_value(n) - (double)data->get_trace_value( n-k));
		}

		if(n >= l+k){
			array_v[n] = (double)(data->get_trace_value( n-l) - (double)data->get_trace_value( n-l-k));
		}

		array_d[n] = array_u[n] - array_v[n];

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /RC_constant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram		
		if(h){
			signalAmplitude = Trapezoidal[n]/(double)k;
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here
		if(data->get_signal_is() > 0.){
			if(Trapezoidal[n] > max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}else{
			if(Trapezoidal[n] < max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}


	}
	if(s1->trap_flattop_mode == 1)
		max_val = get_max_val_trapezoidal1( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = get_max_val_trapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = get_max_val_trapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = get_auto_max_val_trapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val /(double)(k);
	data->set_raw_energy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 2
/*!
 *
 *
 *
 */
double digitalFilters::trapezoidal_filter_algorithm2( dssdData* const data, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	double signalAmplitude = 0.;
	//get k and m parameters
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	ushort k = kPar[b][ch];
	ushort m = mPar[b][ch];
	ushort l = k+m;
	get_RC_constant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if( n < k){
			array_d[n] = (double) data->get_trace_value(n) - data->get_Baseline(); 
		}
		if(n >= k){
			array_d[n] = (double)data->get_trace_value(n) - data->get_Baseline(); 
			array_d[n] -= (double)data->get_trace_value(n-k) - data->get_Baseline(); 
		}
		if(n >= l +k){
			array_d[n] -= (double)data->get_trace_value(n-l) - data->get_Baseline(); 
			array_d[n] += (double)data->get_trace_value(n-k-l) - data->get_Baseline();
		}

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /RC_constant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram		
		if(h){
			signalAmplitude = Trapezoidal[n]/(double)k;
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here
		if(data->get_signal_is() > 0.){
			if(Trapezoidal[n] > max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}else{
			if(Trapezoidal[n] < max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}
	}
	if(s1->trap_flattop_mode == 1)
		max_val = get_max_val_trapezoidal1( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = get_max_val_trapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = get_max_val_trapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = get_auto_max_val_trapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val/(double)k;
	data->set_raw_energy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 2
/*!
 *
 *
 */
double digitalFilters::trapezoidal_filter_algorithm2( dssdData* const data,  ushort k , ushort m, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	ushort l = k+m;
	double signalAmplitude = 0.;
	get_RC_constant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if( n < k){
			array_d[n] = (double)data->get_trace_value(n) - data->get_Baseline(); 
		}
		if(n >= k){
			array_d[n] = (double)data->get_trace_value(n) - data->get_Baseline(); 
			array_d[n] -= (double)data->get_trace_value(n-k) - data->get_Baseline(); 
		}
		if(n >= l +k){
			array_d[n] -= (double)data->get_trace_value(n-l) - data->get_Baseline(); 
			array_d[n] += (double)data->get_trace_value(n-k-l) - data->get_Baseline();
		}

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /RC_constant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){
			signalAmplitude = Trapezoidal[n]/(double)k;
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here
		if(data->get_signal_is() > 0.){
			if(Trapezoidal[n] > max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}else{
			if(Trapezoidal[n] < max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}
	}
	if(s1->trap_flattop_mode == 1)
		max_val = get_max_val_trapezoidal1( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = get_max_val_trapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = get_max_val_trapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = get_auto_max_val_trapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val/(double)k;
	data->set_raw_energy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 3
/*!
 *
 *
 */
double digitalFilters::trapezoidal_filter_algorithm3(dssdData* const data, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	double signalAmplitude = 0.;
	//get k and m parameters
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	ushort k = kPar[b][ch];
	ushort m = mPar[b][ch];
	ushort l = k+m;
	get_RC_constant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if (n < k)
			array_u[n] = (double)data->get_trace_value(n) - data->get_Baseline(); 
		else
			array_u[n] = (double)data->get_trace_value(n) - (double)data->get_trace_value(n-k); 

		array_d[n] = array_u[n];

		if(n >= k+l)
			array_d[n] -= array_u[n -k-l];

		array_p[n] = array_p[n-1] + trapezoidal_shaper_m2 * array_d[n];

		Rectangular[n] = array_p[n] +  array_d[n] * trapezoidal_shaper_m1;

		Trapezoidal[n] =Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){	
			signalAmplitude = Trapezoidal[n]*sampling_period /(RC_constant * (double)k);
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here 
		if(data->get_signal_is() > 0.){
			if(Trapezoidal[n] > max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}else{
			if(Trapezoidal[n] < max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}
	}
	if(s1->trap_flattop_mode == 1)
		max_val = get_max_val_trapezoidal1( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = get_max_val_trapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = get_max_val_trapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = get_auto_max_val_trapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val*sampling_period /(RC_constant * (double)k);
	data->set_raw_energy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 3
/*!
 *
 *
 *
 */
double digitalFilters::trapezoidal_filter_algorithm3(dssdData* const data, ushort k, ushort m, TH1* const h = nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	ushort l = k +m;
	double signalAmplitude = 0.;
	get_RC_constant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if (n < k)
			array_u[n] = (double)data->get_trace_value(n) - data->get_Baseline(); 
		else
			array_u[n] = (double)data->get_trace_value(n) - (double)data->get_trace_value(n-k); 

		array_d[n] = array_u[n];

		if(n >= k+l)
			array_d[n] -= array_u[n -k-l];

		array_p[n] = array_p[n-1] + trapezoidal_shaper_m2 * array_d[n];

		Rectangular[n] = array_p[n] +  array_d[n] * trapezoidal_shaper_m1;

		Trapezoidal[n] =Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram	
		if(h){		
			signalAmplitude = Trapezoidal[n]*sampling_period /(RC_constant * (double)k);
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here 
		if(data->get_signal_is() > 0.){
			if(Trapezoidal[n] > max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}else{
			if(Trapezoidal[n] < max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}
	}
	if(s1->trap_flattop_mode == 1)
		max_val = get_max_val_trapezoidal1( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = get_max_val_trapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = get_max_val_trapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = get_auto_max_val_trapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val*sampling_period /(RC_constant * (double)k);
	data->set_raw_energy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------
// subtract baseline
//--------------------------
double digitalFilters::signal(int n, int nStart, int m, dssdData* const data){
	if((n - m) < nStart) return data->get_Baseline();
	else return (double)  data->get_trace_value(n-m);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 4
/*!
 *
 *
 */
double digitalFilters::trapezoidal_filter_algorithm4( dssdData* const data, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	double signalAmplitude = 0.;
	//get k and m parameters
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	ushort k = kPar[b][ch];
	ushort m = mPar[b][ch];
	ushort l = k+m;
	get_RC_constant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		//equation 1: dkl[n] = v[n] - v[n-k] - v[n-l] + v[n-k-l];// signal(int n, int m, ushort* v, double baseline) v[n] is the signal

		array_d[n] = (double)data->get_trace_value(n) - signal(n,0, k, data)
			- signal(n,0, l, data)
			+ signal(n,0,k+l, data);

		//equation 2: p[n] = p[n-1] + dkl[n], n>= 0
		if(n == 0) array_p[n] = array_d[n];
		else array_p[n] = array_p[n-1] + array_d[n];

		//equation 3: r[n] = p[n] + M*dkl[n]
		Rectangular[n] = array_p[n] + (1./(exp(sampling_period/RC_constant) - 1.))*array_d[n];

		//equation 4: s[n] = s[n-1] + r[n], n>= 0
		if(n == 0) Trapezoidal[n] = Rectangular[n];
		else Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram	
		if(h){
			signalAmplitude = Trapezoidal[n]*sampling_period /(RC_constant *(double)k);
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here 
		if( data->get_signal_is() > 0.){
			if(Trapezoidal[n] > max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}else{
			if(Trapezoidal[n] < max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}
	}
	if(s1->trap_flattop_mode == 1)
		max_val = get_max_val_trapezoidal1( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = get_max_val_trapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = get_max_val_trapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = get_auto_max_val_trapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val*sampling_period /(RC_constant * (double)k);
	data->set_raw_energy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 4
/*!
 *
 *
 */
double digitalFilters::trapezoidal_filter_algorithm4( dssdData* const data, ushort k, ushort m, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	ushort l = k +m;
	double signalAmplitude = 0.;
	get_RC_constant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		//equation 1: dkl[n] = v[n] - v[n-k] - v[n-l] + v[n-k-l];// signal(int n, int m, ushort* v, double baseline) v[n] is the signal

		array_d[n] = (double)data->get_trace_value(n) - signal(n,0, k, data)
			- signal(n,0, l, data)
			+ signal(n,0,k+l, data);

		//equation 2: p[n] = p[n-1] + dkl[n], n>= 0
		if(n == 0) array_p[n] = array_d[n];
		else array_p[n] = array_p[n-1] + array_d[n];

		//equation 3: r[n] = p[n] + M*dkl[n]
		Rectangular[n] = array_p[n] + (1./(exp(sampling_period/RC_constant) - 1.))*array_d[n];

		//equation 4: s[n] = s[n-1] + r[n], n>= 0
		if(n == 0) Trapezoidal[n] = Rectangular[n];
		else Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){
			signalAmplitude = Trapezoidal[n]*sampling_period /(RC_constant * (double)k);
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here 
		if( data->get_signal_is() > 0.){
			if(Trapezoidal[n] > max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}else{
			if(Trapezoidal[n] < max_val){
				max_val = Trapezoidal[n];
				max_pos = n;
			}
		}
	}
	if(s1->trap_flattop_mode == 1)
		max_val = get_max_val_trapezoidal1( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = get_max_val_trapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = get_max_val_trapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = get_auto_max_val_trapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val*sampling_period /(RC_constant * (double)k);
	data->set_raw_energy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
double digitalFilters::perform(dssdData* const dData ,TH1* const hist = nullptr){
	double amp = 0.; 
	if(s1->filterAlgorithm == "TRAP1"){
		if(s1->use_default_filter_parameters)
			amp = trapezoidal_filter_algorithm1(dData ,s1->default_trap_k, s1->default_trap_m, hist);
		else
			amp = trapezoidal_filter_algorithm1( dData, hist);
	}
	else if(s1->filterAlgorithm == "TRAP2"){
		if(s1->use_default_filter_parameters)
			amp = trapezoidal_filter_algorithm2( dData,s1->default_trap_k, s1->default_trap_m, hist);
		else
			amp = trapezoidal_filter_algorithm2( dData, hist);
	}
	else if(s1->filterAlgorithm == "TRAP3"){
		if(s1->use_default_filter_parameters)
			amp =trapezoidal_filter_algorithm3( dData,s1->default_trap_k, s1->default_trap_m, hist);
		else
			amp = trapezoidal_filter_algorithm3( dData, hist);
	}
	else if(s1->filterAlgorithm == "TRAP4"){
		if(s1->use_default_filter_parameters)
			amp = trapezoidal_filter_algorithm4( dData,s1->default_trap_k, s1->default_trap_m, hist);
		else
			amp = trapezoidal_filter_algorithm4( dData, hist);
	}
return amp;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
