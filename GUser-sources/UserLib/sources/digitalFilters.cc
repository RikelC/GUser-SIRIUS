#include "digitalFilters.h"

digitalFilters::digitalFilters(){

	s1 = myGlobal::getInstance();
	Capacitance = 1.;//in pF
	RC_constant = 700.;
	trapezoidal_shaper_M = 0. ;
	trapezoidal_shaper_m2 = 0.;
	trapezoidal_shaper_m1 = 0.;
	array_u = new Double_t[s1->TRACE_SIZE];
	array_v = new Double_t[s1->TRACE_SIZE];
	array_d = new Double_t[s1->TRACE_SIZE];
	array_p = new Double_t[s1->TRACE_SIZE];
	Rectangular = new Double_t[s1->TRACE_SIZE];
	Trapezoidal = new Double_t[s1->TRACE_SIZE];
	kPar = new UShort_t*[s1->NBOARDS_DSSD];
	mPar = new UShort_t*[s1->NBOARDS_DSSD];
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		kPar[i] = new UShort_t[NCHANNELS];
		mPar[i] = new UShort_t[NCHANNELS];
		for(int j = 0; j < NCHANNELS; j++){
			kPar[i][j] = 200;
			mPar[i][j] = 50;
		}
	}




}

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
}


void digitalFilters::assign_k_m_values(){
	std::string line; UShort_t boardID, bIdx, chID, k_val, m_val;

	std::ifstream myfile;
	myfile.open("min_trapezoidal_parameters.txt",std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> k_val >> m_val;
			bIdx = s1->fConvertNoBoardIndexLocal[boardID];
			kPar[bIdx][chID] = k_val;
			mPar[bIdx][chID] = m_val;
		}
		myfile.close();
	}else{std::cout<<"min_trapezoidal_parameters.txt"<<std::endl;}

}


void digitalFilters::set_DSSD_gain(Double_t gain){
	Capacitance = gain; RC_constant =  Resistance*Capacitance;
	trapezoidal_shaper_M = pow((exp(sampling_period/RC_constant) - 1.), -1.);
	trapezoidal_shaper_m2 = 1.;// Gain of the Shaper Amplifier
	trapezoidal_shaper_m1 =  trapezoidal_shaper_m2 *  trapezoidal_shaper_M;
}

void digitalFilters::set_RC_constant(Double_t  r){
	RC_constant = r;
	trapezoidal_shaper_M = pow((exp(sampling_period/RC_constant) - 1.), -1.);
	trapezoidal_shaper_m2 = 1.;// Gain of the Shaper Amplifier
	trapezoidal_shaper_m1 =  trapezoidal_shaper_m2 *  trapezoidal_shaper_M;
}

Double_t digitalFilters::get_max_val_trapezoidal( Double_t signal_is, Double_t* v , UShort_t max_pos, Double_t max_val){
	//average on both sides of the max position
	Double_t maxR =0.; Double_t maxL = 0.;Double_t rR =0.; Double_t rL =0.;
	UShort_t j2 = max_pos + 1;
	UShort_t j1 = max_pos -1;
	if(signal_is > 0.){
		while(v[j2] > 0.999 * max_val){
			maxR += v[j2];
			j2++;
			rR++;
		}

		while(v[j1] > 0.999 * max_val){
			maxL += v[j1];
			j1--;
			rL++;
		}

	}else{
		while(v[j2] < 0.999 * max_val){
			maxR += v[j2];
			j2++;
			rR++;
		}
		while(v[j1] < 0.9999 * max_val){
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
//----------------
// Algorithm 1
//-------------------
Double_t digitalFilters::trapezoidal_filter_algorithm1(  dssdData* const data, TH1* h=nullptr){
	Double_t max_val =0.;
	UShort_t max_pos =0;
	Double_t signalAmplitude = 0.;
	//get k and m parameters
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	UShort_t k = kPar[b][ch];
	UShort_t m = mPar[b][ch];
	UShort_t l = k+m;

	for(UShort_t n = 0; n <s1->TRACE_SIZE; n++){

		if(n < k){
			array_u[n] = (Double_t)data->get_trace_value(n) - data->get_Baseline();
			array_v[n] = (Double_t)data->get_trace_value(n) - data->get_Baseline();
		}

		else{
			array_u[n] = (Double_t)(data->get_trace_value(n) - (Double_t)data->get_trace_value(n-k));
		}

		if(n >= l+k){
			array_v[n] = (Double_t)(data->get_trace_value(n-l) - (Double_t)data->get_trace_value(n-l-k));
		}

		array_d[n] = array_u[n] - array_v[n];

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /RC_constant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){
			signalAmplitude = Trapezoidal[n]/(Double_t)k;
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
	max_val = get_max_val_trapezoidal( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	signalAmplitude = max_val /(Double_t)(k);
	return std::abs(signalAmplitude);
}



Double_t digitalFilters::trapezoidal_filter_algorithm1(  dssdData* const data, UShort_t k , UShort_t m, TH1* h=nullptr){
	Double_t max_val =0.;
	UShort_t max_pos = 0;
	Double_t signalAmplitude = 0.;
	UShort_t l = k+m;
	for(UShort_t n = 0; n <s1->TRACE_SIZE; n++){
		if(n < k){
			array_u[n] = (Double_t)data->get_trace_value(n) - data->get_Baseline();
			array_v[n] = (Double_t)data->get_trace_value(n) - data->get_Baseline();
		}

		else{
			array_u[n] = (Double_t)(data->get_trace_value(n) - (Double_t)data->get_trace_value( n-k));
		}

		if(n >= l+k){
			array_v[n] = (Double_t)(data->get_trace_value( n-l) - (Double_t)data->get_trace_value( n-l-k));
		}

		array_d[n] = array_u[n] - array_v[n];

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /RC_constant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram		
		if(h){
			signalAmplitude = Trapezoidal[n]/(Double_t)k;
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
	max_val = get_max_val_trapezoidal( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	signalAmplitude = max_val /(Double_t)(k);
	return std::abs(signalAmplitude);
}

//----------------
// Algorithm 2
//---------------
Double_t digitalFilters::trapezoidal_filter_algorithm2( dssdData* const data, TH1* h=nullptr){
	Double_t max_val =0.;
	UShort_t max_pos = 0;
	Double_t signalAmplitude = 0.;
	//get k and m parameters
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	UShort_t k = kPar[b][ch];
	UShort_t m = mPar[b][ch];
	UShort_t l = k+m;

	for(UShort_t n = 0; n <s1->TRACE_SIZE; n++){

		if( n < k){
			array_d[n] = (Double_t) data->get_trace_value(n) - data->get_Baseline(); 
		}
		if(n >= k){
			array_d[n] = (Double_t)data->get_trace_value(n) - data->get_Baseline(); 
			array_d[n] -= (Double_t)data->get_trace_value(n-k) - data->get_Baseline(); 
		}
		if(n >= l +k){
			array_d[n] -= (Double_t)data->get_trace_value(n-l) - data->get_Baseline(); 
			array_d[n] += (Double_t)data->get_trace_value(n-k-l) - data->get_Baseline();
		}

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /RC_constant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram		
		if(h){
			signalAmplitude = Trapezoidal[n]/(Double_t)k;
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
	max_val = get_max_val_trapezoidal( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	signalAmplitude = max_val/(Double_t)k;
	return std::abs(signalAmplitude);
}

Double_t digitalFilters::trapezoidal_filter_algorithm2( dssdData* const data,  UShort_t k , UShort_t m, TH1* h=nullptr){
	Double_t max_val =0.;
	UShort_t max_pos = 0;
	UShort_t l = k+m;
	Double_t signalAmplitude = 0.;

	for(UShort_t n = 0; n <s1->TRACE_SIZE; n++){

		if( n < k){
			array_d[n] = (Double_t)data->get_trace_value(n) - data->get_Baseline(); 
		}
		if(n >= k){
			array_d[n] = (Double_t)data->get_trace_value(n) - data->get_Baseline(); 
			array_d[n] -= (Double_t)data->get_trace_value(n-k) - data->get_Baseline(); 
		}
		if(n >= l +k){
			array_d[n] -= (Double_t)data->get_trace_value(n-l) - data->get_Baseline(); 
			array_d[n] += (Double_t)data->get_trace_value(n-k-l) - data->get_Baseline();
		}

		Rectangular[n] = Rectangular[n-1] + array_d[n] - (exp(- sampling_period /RC_constant) *array_d[n-1]);

		Trapezoidal[n] = Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){
			signalAmplitude = Trapezoidal[n]/(Double_t)k;
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
	max_val = get_max_val_trapezoidal( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	signalAmplitude = max_val/(Double_t)k;
	return std::abs(signalAmplitude);
}

//-----------------
// Algorithm 3
//-----------------
Double_t digitalFilters::trapezoidal_filter_algorithm3(dssdData* const data, TH1* h=nullptr){
	Double_t max_val =0.;
	UShort_t max_pos = 0;
	Double_t signalAmplitude = 0.;
	//get k and m parameters
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	UShort_t k = kPar[b][ch];
	UShort_t m = mPar[b][ch];
	UShort_t l = k+m;

	for(UShort_t n = 0; n <s1->TRACE_SIZE; n++){

		if (n < k)
			array_u[n] = (Double_t)data->get_trace_value(n) - data->get_Baseline(); 
		else
			array_u[n] = (Double_t)data->get_trace_value(n) - (Double_t)data->get_trace_value(n-k); 

		array_d[n] = array_u[n];

		if(n >= k+l)
			array_d[n] -= array_u[n -k-l];

		array_p[n] = array_p[n-1] + trapezoidal_shaper_m2 * array_d[n];

		Rectangular[n] = array_p[n] +  array_d[n] * trapezoidal_shaper_m1;

		Trapezoidal[n] =Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram
		if(h){	
			signalAmplitude = Trapezoidal[n]*sampling_period /(RC_constant * (Double_t)k);
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
	max_val = get_max_val_trapezoidal( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	signalAmplitude = max_val*sampling_period /(RC_constant * (Double_t)k);
	return std::abs(signalAmplitude);
}

Double_t digitalFilters::trapezoidal_filter_algorithm3(dssdData* const data, UShort_t k, UShort_t m, TH1*h = nullptr){
	Double_t max_val =0.;
	UShort_t max_pos = 0;
	UShort_t l = k +m;
	Double_t signalAmplitude = 0.;
	for(UShort_t n = 0; n <s1->TRACE_SIZE; n++){

		if (n < k)
			array_u[n] = (Double_t)data->get_trace_value(n) - data->get_Baseline(); 
		else
			array_u[n] = (Double_t)data->get_trace_value(n) - (Double_t)data->get_trace_value(n-k); 

		array_d[n] = array_u[n];

		if(n >= k+l)
			array_d[n] -= array_u[n -k-l];

		array_p[n] = array_p[n-1] + trapezoidal_shaper_m2 * array_d[n];

		Rectangular[n] = array_p[n] +  array_d[n] * trapezoidal_shaper_m1;

		Trapezoidal[n] =Trapezoidal[n-1] + Rectangular[n];
		//Fill histogram	
		if(h){		
			signalAmplitude = Trapezoidal[n]*sampling_period /(RC_constant * (Double_t)k);
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
	max_val = get_max_val_trapezoidal( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	signalAmplitude = max_val*sampling_period /(RC_constant * (Double_t)k);
	return std::abs(signalAmplitude);
}
//--------------------------
// Algorithm 4
//--------------------------
Double_t digitalFilters::signal(Int_t n, Int_t nStart, Int_t m, dssdData* const data){
	if((n - m) < nStart) return data->get_Baseline();
	else return (Double_t)  data->get_trace_value(n-m);
}

Double_t digitalFilters::trapezoidal_filter_algorithm4( dssdData* const data, TH1* h=nullptr){
	Double_t max_val =0.;
	UShort_t max_pos = 0;
	Double_t signalAmplitude = 0.;
	//get k and m parameters
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	UShort_t k = kPar[b][ch];
	UShort_t m = mPar[b][ch];
	UShort_t l = k+m;

	for(UShort_t n = 0; n <s1->TRACE_SIZE; n++){

		//equation 1: dkl[n] = v[n] - v[n-k] - v[n-l] + v[n-k-l];// signal(Int_t n, Int_t m, UShort_t* v, Double_t baseline) v[n] is the signal

		array_d[n] = (Double_t)data->get_trace_value(n) - signal(n,0, k, data)
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
			signalAmplitude = Trapezoidal[n]*sampling_period /(RC_constant *(Double_t)k);
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

	//-------------
	max_val = get_max_val_trapezoidal( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	signalAmplitude = max_val*sampling_period /(RC_constant * (Double_t)k);
	return std::abs(signalAmplitude);
}

Double_t digitalFilters::trapezoidal_filter_algorithm4( dssdData* const data, UShort_t k, UShort_t m, TH1* h=nullptr){
	Double_t max_val =0.;
	UShort_t max_pos = 0;
	UShort_t l = k +m;
	Double_t signalAmplitude = 0.;
	for(UShort_t n = 0; n <s1->TRACE_SIZE; n++){

		//equation 1: dkl[n] = v[n] - v[n-k] - v[n-l] + v[n-k-l];// signal(Int_t n, Int_t m, UShort_t* v, Double_t baseline) v[n] is the signal

		array_d[n] = (Double_t)data->get_trace_value(n) - signal(n,0, k, data)
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
			signalAmplitude = Trapezoidal[n]*sampling_period /(RC_constant * (Double_t)k);
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

	//-------------
	max_val = get_max_val_trapezoidal( data->get_signal_is(), Trapezoidal, max_pos,  max_val);
	signalAmplitude = max_val*sampling_period /(RC_constant * (Double_t)k);
	return std::abs(signalAmplitude);
}
