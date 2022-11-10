/*!
 * \file digitalCFD.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#include "digitalCFD.h"
//--------------------
// Constructor
//--------------------
digitalCFD::digitalCFD()
{
	s1 = myGlobal::getInstance();
	vout = new double[s1->TRACE_SIZE];
	vin = new double[s1->TRACE_SIZE];
	vindelayed = new double[s1->TRACE_SIZE];
	delay = new ushort*[s1->NBOARDS_DSSD];
	fraction = new double*[s1->NBOARDS_DSSD];
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		delay[i] = new ushort[NCHANNELS];
		fraction[i] = new double[NCHANNELS];
	}
	//initialize some preset value
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			delay[i][j] = 10;
			fraction[i][j] = .3;
		}
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//---------------
// Destructor
//--------------
digitalCFD::~digitalCFD()
{
	delete [] vout;
	delete [] vin;
	delete [] vindelayed;
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		delete [] delay[i];
		delete [] fraction[i];
	}
	delete [] delay;
	delete [] fraction;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//-----------------------------
// Assignment of CFD parameters
//------------------------------
void digitalCFD::assign_cfd_parameters(){
	std::string line; ushort boardID, bIdx, chID, d, f;

	std::ifstream myfile;
	myfile.open(s1->cfd_parameter_filename,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> d >> f;
			bIdx = s1->boardIndex_DSSD[boardID];
			delay[bIdx][chID] = d;
			fraction[bIdx][chID] = f;
		}
		myfile.close();
	}else{
		std::cout<<"cfd_parameters.txt file not open"<<std::endl;
	}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------------------------------------
// Interpolated Time at a given fraction of the max height
//---------------------------------------------------------
double digitalCFD::getFractional_Time(dssdData* const data,  double fract){
	double amplitude = data->get_signalHeight();
	int max_pos = (int)data->get_Max_pos();
	double baseline = data->get_Baseline();
	double temp = 0.;

	double y2= 0., y1= 0., x2= 0., x1 = 0., y = 0., x = 0., F = 0.;

	temp = (double)data->get_trace_value(max_pos) - baseline;
	y = fract*amplitude;
	while ( temp > y && (max_pos>0)){
		max_pos--;
		temp = data->get_trace_value(max_pos) - baseline;
	}


	//cfd[i] = (maxbin -1) * sampling_period;//-1 because the first bin is empty
	// a linear interpolation for more precise value
	// y = y1 + (x - x1)* (y2 - y1)/(x2 -x1)
	// F = (y2 - y1)/(x2 -x1)
	// x = x1 + (y -y1)/F
	x2 = (double)(max_pos + 1)*sampling_period;
	y2 = data->get_trace_value(max_pos + 1);

	x1 = (double)(max_pos)*sampling_period;
	y1 = data->get_trace_value(max_pos);

	F = (y2 - y1)/(x2 - x1);

	x = x1 + (y -y1)/F;

	return x;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------------------
// Constant Fraction Discriminator time
//---------------------------------------
double digitalCFD::getCFD_Time( dssdData* const data){
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	int del = delay[b][ch];
	double fract = fraction[b][ch];
	int max_pos = data->get_Max_pos();
	double cfd_time=0;
	vout[0] = 0.;
	vin[0] = 0.;
	vindelayed[0] = 0.;

	double y2= 0., y1= 0.;
	int x2= 0, x1 = 0;
	double y = 0., x = 0., M = 0., C = 0.;

	for (int i = 1; i < s1->TRACE_SIZE; i++) {
		vin[i] = data->get_trace_value(i) - data->get_Baseline();
	}


	for (int j = 1; j < s1->TRACE_SIZE; j++) {
		//forward delay
		if(j <= del)
			vindelayed[j] = 0.;
		else
			vindelayed[j] = vin[j - del];
	}

	for (int t =1; t < s1->TRACE_SIZE; t++) {
		vout[t] = vindelayed[t] - fract*vin[t];

		/* hcfd_vin->SetPoint(t-1,(double)(t-1), vin[t]);
		   hcfd_vindelayed->SetPoint(t-1,(double)(t-1), vindelayed[t]);
		   hcfd_fracvin->SetPoint(t-1,(double)(t-1), fract*vin[t]);
		   hcfd->SetPoint(t-1,(double)(t-1), vout[t]);*/

	}

	//find max pos in the interval (max_pos - 100, max_pos+100)
	x2 = max_pos - 101;
	y2 = vout[x2];
	for(int n = max_pos - 100; n < (max_pos + 100);n++){
		if(data->get_signal_is() > 0. ){
			if(vout[n] > y2){
				y2 = vout[n];
				x2 = n;
			}
		}else{
			if(vout[n] < y2){
				y2 = vout[n];
				x2 = n;
			}
		}
	}
	//find zero crossing point via linear interpolation
	if(data->get_signal_is() > 0.){
		while ( y2 > 0.){
			x2--;
			y2 = vout[x2];
		}
		if(y2 < 0.){x2++;y2 = vout[x2];}
	}else{
		while ( y2 < 0.){
			x2--;
			y2 = vout[x2];
		}
		if(y2 > 0.){x2++;y2 = vout[x2];}
	}

	x1 = x2 - 1;
	y1 = vout[x1];
	M = (y2 - y1)/(double)(x2 - x1);
	C = y1 - M*x1;
	y = 0.;
	x = (y -C)/M;

	double cfdTime = x* sampling_period;
	return cfdTime;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//----------------------------------------------------
// Get digital Constant fraction discriminator time
//----------------------------------------------------
double digitalCFD::getCFD_Time( dssdData* const data, int del,  double fract){
	int max_pos = data->get_Max_pos();
	double cfd_time=0;
	vout[0] = 0.;
	vin[0] = 0.;
	vindelayed[0] = 0.;

	double y2= 0., y1= 0.;
	int x2= 0, x1 = 0;
	double y = 0., x = 0., M = 0., C = 0.;

	for (int i = 1; i < s1->TRACE_SIZE; i++) {
		vin[i] = data->get_trace_value(i) - data->get_Baseline();
	}


	for (int j = 1; j < s1->TRACE_SIZE; j++) {
		//forward delay
		if(j <= del)
			vindelayed[j] = 0.;
		else
			vindelayed[j] = vin[j - del];
	}

	for (int t =1; t < s1->TRACE_SIZE; t++) {
		vout[t] = vindelayed[t] - fract*vin[t];

		/* hcfd_vin->SetPoint(t-1,(double)(t-1), vin[t]);
		   hcfd_vindelayed->SetPoint(t-1,(double)(t-1), vindelayed[t]);
		   hcfd_fracvin->SetPoint(t-1,(double)(t-1), fract*vin[t]);
		   hcfd->SetPoint(t-1,(double)(t-1), vout[t]);*/

	}

	//find max pos in the interval (max_pos - 100, max_pos+100)
	x2 = max_pos - 101;
	y2 = vout[x2];
	for(int n = max_pos - 100; n < (max_pos + 100);n++){
		if(data->get_signal_is() > 0. ){
			if(vout[n] > y2){
				y2 = vout[n];
				x2 = n;
			}
		}else{
			if(vout[n] < y2){
				y2 = vout[n];
				x2 = n;
			}
		}
	}
	//find zero crossing point via linear interpolation
	if(data->get_signal_is() > 0.){
		while ( y2 > 0.){
			x2--;
			y2 = vout[x2];
		}
		if(y2 < 0.){x2++;y2 = vout[x2];}
	}else{
		while ( y2 < 0.){
			x2--;
			y2 = vout[x2];
		}
		if(y2 > 0.){x2++;y2 = vout[x2];}
	}

	x1 = x2 - 1;
	y1 = vout[x1];
	M = (y2 - y1)/(double)(x2 - x1);
	C = y1 - M*x1;
	y = 0.;
	x = (y -C)/M;

	double cfdTime = x* sampling_period;
	return cfdTime;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//-----------------------------------------------------------
// Interpolated Time at given fraction of the maximum height
//------------------------------------------------------------
double digitalCFD::getFractional_Time( dssdData* const data,  double fract, std::string type){
	int max_pos = data->get_Max_pos();
	double temp = 0.;
	unsigned int pts = 4;

	double *x = new double[pts];
	double *y = new double[pts];

	double y_interp = 0., x_interp =0.;
	y_interp = fract*data->get_signalHeight();
	temp = data->get_trace_value(max_pos) - data->get_Baseline();

	while ( temp > y_interp && (max_pos>0)){
		max_pos--;
		temp = data->get_trace_value(max_pos) - data->get_Baseline();
	}

	max_pos--;//
	for(int p = 0; p < pts;p++){
		x[p] = (double) max_pos;
		y[p] = data->get_trace_value(max_pos) -data->get_Baseline();
		max_pos++;
	}

	ROOT::Math::Interpolator *inter;
	if(type=="CSPLINE")inter = new ROOT::Math::Interpolator (pts, ROOT::Math::Interpolation::kCSPLINE);
	else if(type =="CSPLINE_PERIODIC")inter = new ROOT::Math::Interpolator (pts, ROOT::Math::Interpolation::kCSPLINE_PERIODIC);
	else if(type =="LINEAR")inter = new ROOT::Math::Interpolator (pts, ROOT::Math::Interpolation::kLINEAR);
	else if(type =="POLYNOMIAL")inter = new ROOT::Math::Interpolator (pts, ROOT::Math::Interpolation::kPOLYNOMIAL);
	else if(type =="AKIMA")inter = new ROOT::Math::Interpolator (pts, ROOT::Math::Interpolation::kAKIMA);
	else if(type =="AKIMA_PERIODIC")inter = new ROOT::Math::Interpolator (pts, ROOT::Math::Interpolation::kAKIMA_PERIODIC);
	//ROOT::Math::Interpolation::Type type  : kLINEAR, kPOLYNOMIAL, kCSPLINE, kAKIMA, kCSPLINE_PERIODIC, kAKIMA_PERIODIC
	else {
		cout<<"No such "<<type<<" interpolation type is available. Choosing default CSPLINE interpolation method."<<endl;

		inter = new ROOT::Math::Interpolator (pts, ROOT::Math::Interpolation::kCSPLINE);
	}
	inter->SetData (pts, y, x);
	x_interp = inter->Eval(y_interp);
	delete [] x;
	delete [] y;
	delete inter;
	return x_interp;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! get CFD time
ullint digitalCFD::perform(dssdData * const dData){
	ullint cfd_time = 0;

	if(s1->CFDMethod == 0) cfd_time = dData->get_timestamp() * timestamp_unit;

	else if(s1->CFDMethod == 1)
		cfd_time = (dData->get_timestamp() * timestamp_unit) - (s1->pre_trig_buffer*sampling_period) + static_cast<ullint>(getFractional_Time(dData, s1->CFDFraction, s1->InterpolationType));
	else if(s1->CFDMethod == 2)
	{
		if(s1->use_default_CFD_parameters)
			cfd_time = (dData->get_timestamp() * timestamp_unit) - (s1->pre_trig_buffer*sampling_period) + static_cast<ullint>(getCFD_Time(dData, s1->CFDDelay, s1->CFDFraction));
		else
			cfd_time = (dData->get_timestamp() * timestamp_unit) - (s1->pre_trig_buffer*sampling_period) + static_cast<ullint>(getCFD_Time(dData));

	}
	return cfd_time;
}
