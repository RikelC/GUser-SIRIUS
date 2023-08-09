/*!
 * \file DigitalFilters.cc
 * \author Rikel CHAKMA
 * \brief Digital pulse processing algorithms. 
 * \details The trapezoidal pulse processing algorithms are based on Ref. Valentin T.Jordanov et al., NIM, 353, 261-264 (1994). https://doi.org/10.1016/0168-9002(94)91652-7
 */


#include "DigitalFilters.h"
//! Constructor
DigitalFilters::DigitalFilters(){

	s1 = MyGlobal::GetInstance();
	fCapacitance = 1.;//in pF
	fRCConstant = 700.;
	fTrapezoidalShaperM = 0. ;
	fTrapezoidalShaperM2 = 0.;
	fTrapezoidalShaperM1 = 0.;
	array_u = new double[s1->TRACE_SIZE];
	array_v = new double[s1->TRACE_SIZE];
	array_d = new double[s1->TRACE_SIZE];
	array_p = new double[s1->TRACE_SIZE];
	Rectangular = new double[s1->TRACE_SIZE];
	Trapezoidal = new double[s1->TRACE_SIZE];
	fKParameter = new ushort*[s1->NBOARDS_DSSD];
	fMParameter = new ushort*[s1->NBOARDS_DSSD];
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		fKParameter[i] = new ushort[NCHANNELS];
		fMParameter[i] = new ushort[NCHANNELS];
		for(int j = 0; j < NCHANNELS; j++){
			fKParameter[i][j] = 200;
			fMParameter[i][j] = 50;
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
DigitalFilters::~DigitalFilters(){
	delete [] array_u;
	delete [] array_v;
	delete [] array_d;
	delete [] array_p;
	delete [] Rectangular;
	delete [] Trapezoidal;
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		delete [] fKParameter[i];
		delete [] fMParameter[i];
	}
	delete [] fKParameter;
	delete [] fMParameter;
	if(s1->trap_flattop_mode == 4){
		delete [] diff_arr;
		delete [] diff_arr2;
	}
	delete [] MA;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get simple moving average of N points on the subset [nStart, nEnd) of an array of data points v.
template<class T>
void DigitalFilters::GetMovingAverageOfSignal(T* const v, int N, int nStart, int nEnd){
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
int * DigitalFilters::GetStartStopRange(T* const v, double signal_is, int m = 10){

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
void DigitalFilters::AssignKMValues(){
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
			fKParameter[bIdx][chID] = k_val;
			fMParameter[bIdx][chID] = m_val;
		}
		myfile.close();
	}else{std::cout<<s1->trap_parameter_filename<<" cannot be opened!!!"<<std::endl;}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------
// Setting FPCSA gain 
//--------------------------
void DigitalFilters::SetDssdGain(double gain){
	fCapacitance = gain; fRCConstant =  Resistance*fCapacitance;
	fTrapezoidalShaperM = pow((exp(sampling_period/fRCConstant) - 1.), -1.);
	fTrapezoidalShaperM2 = s1->shaperAmplificationGain; 
	fTrapezoidalShaperM1 =  fTrapezoidalShaperM2 *  fTrapezoidalShaperM;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------
// Set RC constant
//--------------------------
void DigitalFilters::SetRCConstant(double  r){
	fRCConstant = r;
	fTrapezoidalShaperM = pow((exp(sampling_period/fRCConstant) - 1.), -1.);
	fTrapezoidalShaperM2 = s1->shaperAmplificationGain; 
	fTrapezoidalShaperM1 =  fTrapezoidalShaperM2 *  fTrapezoidalShaperM;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------
// Get RC constant
//--------------------------
void DigitalFilters::GetRCConstant(DssdData* const data){
	if(s1->use_default_FPCSA_Gain){
		if(data->GetStripNumber() >=0 && data->GetStripNumber() < 128){
			if(s1->FRONT_FPCSA_GAIN_MODE == "HG") fRCConstant =  Resistance * s1->frontHighGain;
			if(s1->FRONT_FPCSA_GAIN_MODE == "LG") fRCConstant =  Resistance * s1->frontLowGain;
			if(s1->FRONT_FPCSA_GAIN_MODE == "AUTO"){ 
				if(data->GainSwitched())
					fRCConstant =  Resistance * s1->frontLowGain;
				else
					fRCConstant =  Resistance * s1->frontHighGain;
			}

		}
		else if(data->GetStripNumber() >127 && data->GetStripNumber() < 256){
			if(s1->BACK_FPCSA_GAIN_MODE == "HG") fRCConstant =  Resistance * s1->backHighGain;
			if(s1->BACK_FPCSA_GAIN_MODE == "LG") fRCConstant =  Resistance * s1->backLowGain;
			if(s1->BACK_FPCSA_GAIN_MODE == "AUTO"){ 
				if(data->GainSwitched())
					fRCConstant =  Resistance * s1->backLowGain;
				else
					fRCConstant =  Resistance * s1->backHighGain;
			}

		}
	}else{
		fRCConstant =  data->GetRCConstant();
	}


	fTrapezoidalShaperM = pow((exp(sampling_period/fRCConstant) - 1.), -1.);
	fTrapezoidalShaperM2 = s1->shaperAmplificationGain; 
	fTrapezoidalShaperM1 =  fTrapezoidalShaperM2 *  fTrapezoidalShaperM;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Calculate max value of the trapezoidal automatically
double DigitalFilters::GetAutoMaxValueOfTrapezoidal(double * const v){

	int *arr = GetStartStopRange(v,5);
	double height =0.;

	for(int i = arr[0];i<= arr[1];i++){

		height += v[i];

	}
	height /= (double)(arr[1]-arr[0] +1);

	return height; 
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get max value of the trapezodal1
double DigitalFilters::GetMaxValueOfTrapezoidal1( double signal_is, double* const v , ushort max_pos, double max_val){
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
double DigitalFilters::GetMaxValueOfTrapezoidal2(DssdData* const data, double* const v , ushort k, ushort l){
	ushort start = data->GetTrigger()  + k;
	ushort stop = data->GetTrigger() + l;

	double avg =0.;

	for(ushort i = start; i <= stop; i++){
		avg += v[i];
	}

	avg /= (double)(stop -start +1);
	return avg;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get Max value of the trapezoidal3
double DigitalFilters::GetMaxValueOfTrapezoidal3(  DssdData* const data, double* const v , ushort k, ushort l){

	ushort risetime = data->GetRiseTime()/sampling_period;
	ushort start =0;

	if(k >= risetime)
		start = data->GetTrigger()  + k;
	else if(risetime > k && risetime < l){

		start = data->GetTrigger()  + risetime;

	}
	else start = data->GetTrigger() + l;

	ushort stop = data->GetTrigger() + l;

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
double DigitalFilters::TrapezoidalFilterAlgorithm1(  DssdData* const data, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos =0;
	double signalAmplitude = 0.;
	//get k and m parameters
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	ushort k = fKParameter[b][ch];
	ushort m = fMParameter[b][ch];
	ushort l = k+m;

	GetRCConstant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if(n < k){
			array_u[n] = (double)data->GetTraceValue(n) - data->GetBaseline();
			array_v[n] = (double)data->GetTraceValue(n) - data->GetBaseline();
		}

		else{
			array_u[n] = (double)(data->GetTraceValue(n) - (double)data->GetTraceValue(n-k));
		}

		if(n >= l+k){
			array_v[n] = (double)(data->GetTraceValue(n-l) - (double)data->GetTraceValue(n-l-k));
		}

		array_d[n] = array_u[n] - array_v[n];

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /fRCConstant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){
			signalAmplitude = Trapezoidal[n]/(double)k;
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here
		if(data->SignalIs() > 0.){
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
		max_val = GetMaxValueOfTrapezoidal1( data->SignalIs(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = GetMaxValueOfTrapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = GetMaxValueOfTrapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = GetAutoMaxValueOfTrapezoidal(Trapezoidal);
	//-----------------

	signalAmplitude = max_val /(double)(k);

	data->SetRawEnergy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 1 
/*!
 *
 *
 */
double DigitalFilters::TrapezoidalFilterAlgorithm1(  DssdData* const data, ushort k , ushort m, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	double signalAmplitude = 0.;
	ushort l = k+m;
	GetRCConstant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){
		if(n < k){
			array_u[n] = (double)data->GetTraceValue(n) - data->GetBaseline();
			array_v[n] = (double)data->GetTraceValue(n) - data->GetBaseline();
		}

		else{
			array_u[n] = (double)(data->GetTraceValue(n) - (double)data->GetTraceValue( n-k));
		}

		if(n >= l+k){
			array_v[n] = (double)(data->GetTraceValue( n-l) - (double)data->GetTraceValue( n-l-k));
		}

		array_d[n] = array_u[n] - array_v[n];

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /fRCConstant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram		
		if(h){
			signalAmplitude = Trapezoidal[n]/(double)k;
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here
		if(data->SignalIs() > 0.){
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
		max_val = GetMaxValueOfTrapezoidal1( data->SignalIs(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = GetMaxValueOfTrapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = GetMaxValueOfTrapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = GetAutoMaxValueOfTrapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val /(double)(k);
	data->SetRawEnergy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 2
/*!
 *
 *
 *
 */
double DigitalFilters::TrapezoidalFilterAlgorithm2( DssdData* const data, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	double signalAmplitude = 0.;
	//get k and m parameters
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	ushort k = fKParameter[b][ch];
	ushort m = fMParameter[b][ch];
	ushort l = k+m;
	GetRCConstant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if( n < k){
			array_d[n] = (double) data->GetTraceValue(n) - data->GetBaseline(); 
		}
		if(n >= k){
			array_d[n] = (double)data->GetTraceValue(n) - data->GetBaseline(); 
			array_d[n] -= (double)data->GetTraceValue(n-k) - data->GetBaseline(); 
		}
		if(n >= l +k){
			array_d[n] -= (double)data->GetTraceValue(n-l) - data->GetBaseline(); 
			array_d[n] += (double)data->GetTraceValue(n-k-l) - data->GetBaseline();
		}

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /fRCConstant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram		
		if(h){
			signalAmplitude = Trapezoidal[n]/(double)k;
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here
		if(data->SignalIs() > 0.){
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
		max_val = GetMaxValueOfTrapezoidal1( data->SignalIs(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = GetMaxValueOfTrapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = GetMaxValueOfTrapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = GetAutoMaxValueOfTrapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val/(double)k;
	data->SetRawEnergy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 2
/*!
 *
 *
 */
double DigitalFilters::TrapezoidalFilterAlgorithm2( DssdData* const data,  ushort k , ushort m, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	ushort l = k+m;
	double signalAmplitude = 0.;
	GetRCConstant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if( n < k){
			array_d[n] = (double)data->GetTraceValue(n) - data->GetBaseline(); 
		}
		if(n >= k){
			array_d[n] = (double)data->GetTraceValue(n) - data->GetBaseline(); 
			array_d[n] -= (double)data->GetTraceValue(n-k) - data->GetBaseline(); 
		}
		if(n >= l +k){
			array_d[n] -= (double)data->GetTraceValue(n-l) - data->GetBaseline(); 
			array_d[n] += (double)data->GetTraceValue(n-k-l) - data->GetBaseline();
		}

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /fRCConstant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){
			signalAmplitude = Trapezoidal[n]/(double)k;
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here
		if(data->SignalIs() > 0.){
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
		max_val = GetMaxValueOfTrapezoidal1( data->SignalIs(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = GetMaxValueOfTrapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = GetMaxValueOfTrapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = GetAutoMaxValueOfTrapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val/(double)k;
	data->SetRawEnergy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 3
/*!
 *
 *
 */
double DigitalFilters::TrapezoidalFilterAlgorithm3(DssdData* const data, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	double signalAmplitude = 0.;
	//get k and m parameters
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	ushort k = fKParameter[b][ch];
	ushort m = fMParameter[b][ch];
	ushort l = k+m;
	GetRCConstant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if (n < k)
			array_u[n] = (double)data->GetTraceValue(n) - data->GetBaseline(); 
		else
			array_u[n] = (double)data->GetTraceValue(n) - (double)data->GetTraceValue(n-k); 

		array_d[n] = array_u[n];

		if(n >= k+l)
			array_d[n] -= array_u[n -k-l];

		array_p[n] = array_p[n-1] + fTrapezoidalShaperM2 * array_d[n];

		Rectangular[n] = array_p[n] +  array_d[n] * fTrapezoidalShaperM1;

		Trapezoidal[n] =Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){	
			signalAmplitude = Trapezoidal[n]*sampling_period /(fRCConstant * (double)k);
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here 
		if(data->SignalIs() > 0.){
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
		max_val = GetMaxValueOfTrapezoidal1( data->SignalIs(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = GetMaxValueOfTrapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = GetMaxValueOfTrapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = GetAutoMaxValueOfTrapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val*sampling_period /(fRCConstant * (double)k);
	data->SetRawEnergy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 3
/*!
 *
 *
 *
 */
double DigitalFilters::TrapezoidalFilterAlgorithm3(DssdData* const data, ushort k, ushort m, TH1* const h = nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	ushort l = k +m;
	double signalAmplitude = 0.;
	GetRCConstant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		if (n < k)
			array_u[n] = (double)data->GetTraceValue(n) - data->GetBaseline(); 
		else
			array_u[n] = (double)data->GetTraceValue(n) - (double)data->GetTraceValue(n-k); 

		array_d[n] = array_u[n];

		if(n >= k+l)
			array_d[n] -= array_u[n -k-l];

		array_p[n] = array_p[n-1] + fTrapezoidalShaperM2 * array_d[n];

		Rectangular[n] = array_p[n] +  array_d[n] * fTrapezoidalShaperM1;

		Trapezoidal[n] =Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram	
		if(h){		
			signalAmplitude = Trapezoidal[n]*sampling_period /(fRCConstant * (double)k);
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here 
		if(data->SignalIs() > 0.){
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
		max_val = GetMaxValueOfTrapezoidal1( data->SignalIs(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = GetMaxValueOfTrapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = GetMaxValueOfTrapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = GetAutoMaxValueOfTrapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val*sampling_period /(fRCConstant * (double)k);
	data->SetRawEnergy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------
// subtract baseline
//--------------------------
double DigitalFilters::Signal(int n, int nStart, int m, DssdData* const data){
	if((n - m) < nStart) return data->GetBaseline();
	else return (double)  data->GetTraceValue(n-m);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 4
/*!
 *
 *
 */
double DigitalFilters::TrapezoidalFilterAlgorithm4( DssdData* const data, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	double signalAmplitude = 0.;
	//get k and m parameters
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	ushort k = fKParameter[b][ch];
	ushort m = fMParameter[b][ch];
	ushort l = k+m;
	GetRCConstant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		//equation 1: dkl[n] = v[n] - v[n-k] - v[n-l] + v[n-k-l];// Signal(int n, int m, ushort* v, double baseline) v[n] is the signal

		array_d[n] = (double)data->GetTraceValue(n) - Signal(n,0, k, data)
			- Signal(n,0, l, data)
			+ Signal(n,0,k+l, data);

		//equation 2: p[n] = p[n-1] + dkl[n], n>= 0
		if(n == 0) array_p[n] = array_d[n];
		else array_p[n] = array_p[n-1] + array_d[n];

		//equation 3: r[n] = p[n] + M*dkl[n]
		Rectangular[n] = array_p[n] + (1./(exp(sampling_period/fRCConstant) - 1.))*array_d[n];

		//equation 4: s[n] = s[n-1] + r[n], n>= 0
		if(n == 0) Trapezoidal[n] = Rectangular[n];
		else Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram	
		if(h){
			signalAmplitude = Trapezoidal[n]*sampling_period /(fRCConstant *(double)k);
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here 
		if( data->SignalIs() > 0.){
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
		max_val = GetMaxValueOfTrapezoidal1( data->SignalIs(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = GetMaxValueOfTrapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = GetMaxValueOfTrapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = GetAutoMaxValueOfTrapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val*sampling_period /(fRCConstant * (double)k);
	data->SetRawEnergy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Algorithm 4
/*!
 *
 *
 */
double DigitalFilters::TrapezoidalFilterAlgorithm4( DssdData* const data, ushort k, ushort m, TH1* const h=nullptr){
	double max_val =0.;
	ushort max_pos = 0;
	ushort l = k +m;
	double signalAmplitude = 0.;
	GetRCConstant(data);
	for(ushort n = 0; n <s1->TRACE_SIZE; n++){

		//equation 1: dkl[n] = v[n] - v[n-k] - v[n-l] + v[n-k-l];// Signal(int n, int m, ushort* v, double baseline) v[n] is the signal

		array_d[n] = (double)data->GetTraceValue(n) - Signal(n,0, k, data)
			- Signal(n,0, l, data)
			+ Signal(n,0,k+l, data);

		//equation 2: p[n] = p[n-1] + dkl[n], n>= 0
		if(n == 0) array_p[n] = array_d[n];
		else array_p[n] = array_p[n-1] + array_d[n];

		//equation 3: r[n] = p[n] + M*dkl[n]
		Rectangular[n] = array_p[n] + (1./(exp(sampling_period/fRCConstant) - 1.))*array_d[n];

		//equation 4: s[n] = s[n-1] + r[n], n>= 0
		if(n == 0) Trapezoidal[n] = Rectangular[n];
		else Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){
			signalAmplitude = Trapezoidal[n]*sampling_period /(fRCConstant * (double)k);
			h->Fill(n+1, signalAmplitude);
		}
		//get max position here 
		if( data->SignalIs() > 0.){
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
		max_val = GetMaxValueOfTrapezoidal1( data->SignalIs(), Trapezoidal, max_pos,  max_val);
	else if(s1->trap_flattop_mode == 2)
		max_val = GetMaxValueOfTrapezoidal2( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 3)
		max_val = GetMaxValueOfTrapezoidal3( data, Trapezoidal, k, l);
	else if(s1->trap_flattop_mode == 4)
		max_val = GetAutoMaxValueOfTrapezoidal(Trapezoidal);
	//-------------
	signalAmplitude = max_val*sampling_period /(fRCConstant * (double)k);
	data->SetRawEnergy(signalAmplitude);
	return TMath::Abs(signalAmplitude);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
double DigitalFilters::Perform(DssdData* const dData ,TH1* const hist = nullptr){
	double amp = 0.; 
	if(s1->filterAlgorithm == "TRAP1"){
		if(s1->use_default_filter_parameters)
			amp = TrapezoidalFilterAlgorithm1(dData ,s1->default_trap_k, s1->default_trap_m, hist);
		else
			amp = TrapezoidalFilterAlgorithm1( dData, hist);
	}
	else if(s1->filterAlgorithm == "TRAP2"){
		if(s1->use_default_filter_parameters)
			amp = TrapezoidalFilterAlgorithm2( dData,s1->default_trap_k, s1->default_trap_m, hist);
		else
			amp = TrapezoidalFilterAlgorithm2( dData, hist);
	}
	else if(s1->filterAlgorithm == "TRAP3"){
		if(s1->use_default_filter_parameters)
			amp =TrapezoidalFilterAlgorithm3( dData,s1->default_trap_k, s1->default_trap_m, hist);
		else
			amp = TrapezoidalFilterAlgorithm3( dData, hist);
	}
	else if(s1->filterAlgorithm == "TRAP4"){
		if(s1->use_default_filter_parameters)
			amp = TrapezoidalFilterAlgorithm4( dData,s1->default_trap_k, s1->default_trap_m, hist);
		else
			amp = TrapezoidalFilterAlgorithm4( dData, hist);
	}
	return amp;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
