/*!
 * \file DigitalCFD.h
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 */

#include "DigitalCFD.h"
//--------------------
// Constructor
//--------------------
DigitalCFD::DigitalCFD()
{
	s1 = MyGlobal::GetInstance();
	rand = new TRandom3();
	fVOut = new double[s1->TRACE_SIZE];
	fVIn = new double[s1->TRACE_SIZE];
	fVInDelayed = new double[s1->TRACE_SIZE];
	fDelay = new ushort*[s1->NBOARDS_DSSD];
	fFraction = new double*[s1->NBOARDS_DSSD];
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		fDelay[i] = new ushort[NCHANNELS];
		fFraction[i] = new double[NCHANNELS];
	}
	//initialize some preset value
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			fDelay[i][j] = 10;
			fFraction[i][j] = .3;
		}
	}
	fZeroCrossingTime = 0.;
	fZeroCrossingSample = 0.;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//---------------
// Destructor
//--------------
DigitalCFD::~DigitalCFD()
{
	delete rand;
	delete [] fVOut;
	delete [] fVIn;
	delete [] fVInDelayed;
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		delete [] fDelay[i];
		delete [] fFraction[i];
	}
	delete [] fDelay;
	delete [] fFraction;
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//-----------------------------
// Assignment of CFD parameters
//------------------------------
void DigitalCFD::AssignCFDParameters(){
	std::string line; ushort boardID, bIdx, chID, d, f;

	std::ifstream myfile;
	//DSSD parameter file	
	myfile.open(s1->cfd_parameter_filename_dssd,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> d >> f;
			bIdx = s1->boardIndex_DSSD[boardID];
			fDelay[bIdx][chID] = d;
			fFraction[bIdx][chID] = f;
		}
		myfile.close();
	}else{
		std::cout<<"dssd cfd_parameters.txt file not open"<<std::endl;
	}

	//SED parameter file
	myfile.open(s1->cfd_parameter_filename_sed,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> d >> f;
			bIdx = s1->boardIndex_DSSD[boardID];
			fDelay[bIdx][chID] = d;
			fFraction[bIdx][chID] = f;
		}
		myfile.close();
	}else{
		std::cout<<"sed cfd_parameters.txt file not open"<<std::endl;
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------------------------------------
// Interpolated Time at a given fFraction of the max height
//---------------------------------------------------------
double DigitalCFD::GetFractionalTime(DssdData* const data,  double fract){
	double amplitude = TMath::Abs(data->GetSignalHeight());
	int max_pos = (int)data->GetMaximumPosition();
	double baseline = data->GetBaseline();
	double temp = 0.;

	double y2= 0., y1= 0., x2= 0., x1 = 0., y = 0., x = 0., F = 0.;

	if(!data->GainSwitched()){
		temp = TMath::Abs((double)data->GetTraceValue(max_pos) - baseline);
		y = fract*amplitude;
		while ( temp > y && (max_pos>0)){
			max_pos--;
			temp = TMath::Abs(data->GetTraceValue(max_pos) - baseline);
		}

		x2 = (double)(max_pos + 1)*sampling_period;
		y2 = TMath::Abs(data->GetTraceValue(max_pos + 1));

		x1 = (double)(max_pos)*sampling_period;
		y1 = TMath::Abs(data->GetTraceValue(max_pos));
	}else{
		temp = TMath::Abs((double)data->GetAGTTraceValue(max_pos) - baseline);
		y = fract*amplitude;
		while ( temp > y && (max_pos>0)){
			max_pos--;
			temp = TMath::Abs(data->GetAGTTraceValue(max_pos) - baseline);
		}

		x2 = (double)(max_pos + 1)*sampling_period;
		y2 = TMath::Abs(data->GetAGTTraceValue(max_pos + 1));

		x1 = (double)(max_pos)*sampling_period;
		y1 = TMath::Abs(data->GetAGTTraceValue(max_pos));

	}

	//cfd[i] = (maxbin -1) * sampling_period;//-1 because the first bin is empty
	// a linear interpolation for more precise value
	// y = y1 + (x - x1)* (y2 - y1)/(x2 -x1)
	// F = (y2 - y1)/(x2 -x1)
	// x = x1 + (y -y1)/F


	F = (y2 - y1)/(x2 - x1);

	x = x1 + (y -y1)/F;

	return  x;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------------------
// Constant Fraction Discriminator time
//---------------------------------------
double DigitalCFD::GetCFDTime( DssdData* const data){
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	int del = fDelay[b][ch];
	double fract = fFraction[b][ch];
	int max_pos = data->GetMaximumPosition();
	double cfd_time=0;
	fVOut[0] = 0.;
	fVIn[0] = 0.;
	fVInDelayed[0] = 0.;

	double y2= 0., y1= 0.;
	int x2= 0, x1 = 0;
	double y = 0., x = 0., M = 0., C = 0.;

	if(!data->GainSwitched()){
		for (int i = 0; i < s1->TRACE_SIZE; i++) {
			fVIn[i] = TMath::Abs(data->GetTraceValue(i) - data->GetBaseline());
		}
	}
	else{
		for (int i = 0; i < s1->TRACE_SIZE; i++) {
			fVIn[i] = TMath::Abs(data->GetAGTTraceValue(i) - data->GetBaseline());
		}
	}

	for (int j = 0; j < s1->TRACE_SIZE; j++) {
		//forward fDelay
		if(j <= del)
			fVInDelayed[j] = 0.;
		else
			fVInDelayed[j] = fVIn[j - del];
	}

	for (int t =0; t < s1->TRACE_SIZE; t++) {
		fVOut[t] = fVInDelayed[t] - fract*fVIn[t];
	}

	//find max pos in the interval (max_pos - 100, max_pos+100)
	x2 = max_pos - 100;
	y2 = fVOut[x2];
	for(int n = max_pos - 100; n < (max_pos + 100);n++){
		if(fVOut[n] > y2){
			y2 = fVOut[n];
			x2 = n;
		}
	}
	//find zero crossing point via linear interpolation
	while ( y2 > 0.){
		x2--;
		y2 = fVOut[x2];
	}
	if(y2 < 0.){
		x2++; 
		y2 = fVOut[x2];
	}
	x1 = x2 - 1;
	y1 = fVOut[x1];
	x1 +=rand->Uniform(0,1.); x2 +=rand->Uniform(0,1.);
	M = (y2 - y1)/(double)(x2 - x1);
	C = y1 - M*static_cast<double>(x1);
	y = 0.;
	if(M ==0.) x =x1;
	else x = (y -C)/M;
	fZeroCrossingSample = int(x);
	fDisplacement = fFixedTimeSample - fZeroCrossingSample; 
	fZeroCrossingTime = (x  + fDisplacement) * (double)sampling_period;
	fZeroCrossingSample = int(x + fDisplacement);

	return fZeroCrossingTime;
}


double DigitalCFD::GetCFDTime( DssdData* const data, int del,  double fract, TGraph* h1, TGraph *h2, TGraph *h3, TGraph* h4 ){
	int max_pos = data->GetMaximumPosition();
	double cfd_time=0;
	fVOut[0] = 0.;
	fVIn[0] = 0.;
	fVInDelayed[0] = 0.;

	double y2= 0., y1= 0.;
	int x2= 0, x1 = 0;
	double y = 0., x = 0., M = 0., C = 0.;
	if(!data->GainSwitched()){
		for (int i = 0; i < s1->TRACE_SIZE; i++) {
			fVIn[i] = TMath::Abs(data->GetTraceValue(i) - data->GetBaseline());
		}
	}
	else{
		for (int i = 0; i < s1->TRACE_SIZE; i++) {
			fVIn[i] = TMath::Abs(data->GetAGTTraceValue(i) - data->GetBaseline());
		}
	}

	for (int j = 0; j < s1->TRACE_SIZE; j++) {
		//forward fDelay
		if(j <= del)
			fVInDelayed[j] = 0.;
		else
			fVInDelayed[j] = fVIn[j - del];
	}

	for (int t =0; t < s1->TRACE_SIZE; t++) {
		fVOut[t] = fVInDelayed[t] - fract*fVIn[t];

		h1->SetPoint(t-1,(double)(t-1), fVIn[t]);
		h2->SetPoint(t-1,(double)(t-1), fVInDelayed[t]);
		h3->SetPoint(t-1,(double)(t-1), fract*fVIn[t]);
		h4->SetPoint(t-1,(double)(t-1), fVOut[t]);

	}
	//find max pos in the interval (max_pos - 100, max_pos+100)	
	x2 = max_pos - 100;
	y2 = fVOut[x2];
	for(int n = max_pos - 100; n < (max_pos + 100);n++){
		if(fVOut[n] > y2){
			y2 = fVOut[n];
			x2 = n;
		}
	}
	//find zero crossing point via linear interpolation
	while ( y2 > 0.){
		x2--;
		y2 = fVOut[x2];
	}
	if(y2 < 0.){
		x2++; 
		y2 = fVOut[x2];
	}
	x1 = x2 - 1;
	y1 = fVOut[x1];
	x1 +=rand->Uniform(0,1.); x2 +=rand->Uniform(0,1.);
	M = (y2 - y1)/(double)(x2 - x1);
	C = y1 - M*static_cast<double>(x1);
	y = 0.;
	if(M ==0.) x = x1;
	else x = (y -C)/M;
	fZeroCrossingSample = int(x);
	fDisplacement = fFixedTimeSample - fZeroCrossingSample; 
	fZeroCrossingTime = (x  + fDisplacement) * (double)sampling_period;
	fZeroCrossingSample = int(x + fDisplacement);

	return fZeroCrossingTime; 
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//----------------------------------------------------
// Get digital Constant fFraction discriminator time
//----------------------------------------------------
double DigitalCFD::GetCFDTime( DssdData* const data, int del,  double fract, TH2F* hcfd){
	int max_pos = data->GetMaximumPosition();
	double cfd_time=0;
	fVOut[0] = 0.;
	fVIn[0] = 0.;
	fVInDelayed[0] = 0.;

	double y2= 0., y1= 0.;
	int x2= 0, x1 = 0;
	double y = 0., x = 0., M = 0., C = 0.;

	//cout<<"del "<<del<<"  frac "<<fract<<endl;
	if(!data->GainSwitched()){
		for (int i = 0; i < s1->TRACE_SIZE; i++) {
			fVIn[i] =  TMath::Abs(data->GetTraceValue(i) - data->GetBaseline());
		}
	}
	else{
		for (int i = 0; i < s1->TRACE_SIZE; i++) {
			fVIn[i] = TMath::Abs(data->GetAGTTraceValue(i) - data->GetBaseline());
		}
	}

	for (int j = 0; j < s1->TRACE_SIZE; j++) {
		//forward fDelay
		if(j <= del)
			fVInDelayed[j] = 0.;
		else
			fVInDelayed[j] = fVIn[j - del];
	}

	for (int t =0; t < s1->TRACE_SIZE; t++) {
		fVOut[t] = fVInDelayed[t] - fract*fVIn[t];
		if(hcfd)hcfd->Fill(t,fVOut[t]);
	}

	//find max pos in the interval (max_pos - 100, max_pos+100)
	x2 = max_pos - 100;
	y2 = fVOut[x2];
	for(int n = max_pos - 100; n < (max_pos + 100);n++){
		if(fVOut[n] > y2){
			y2 = fVOut[n];
			x2 = n;
		}
	}
	//find zero crossing point via linear interpolation
	while ( y2 > 0.){
		x2--;
		y2 = fVOut[x2];
	}
	/*if(y2 < 0.){
	  x2++; 
	  y2 = fVOut[x2];
	  }*/

	/*	x1 = x2 - 2;
		x2 +=2; 
		y2 = fVOut[x2];
		*/
	x1 = x2 + 1;
	y1 = fVOut[x1];
	//x1 +=rand->Uniform(0,1.); x2 +=rand->Uniform(0,1.);
	M = (y1 - y2)/(double)(x1 - x2);
	C = y1 - M*static_cast<double>(x1);
	y = 0.;
	if(M ==0.) x = x1;
	else x = (y -C)/M;

	fZeroCrossingSample = x;
	fZeroCrossingSampleHeight = fVOut[int(x)];
//cout<<"x1: "<<x1<<"  x2:  "<<x2<<"  x: "<<x<<endl;
//cout<<"y1: "<<y1<<"  y2:  "<<y2<<"  y0: "<<fZeroCrossingSampleHeight<<endl;
	//fDisplacement = fFixedTimeSample - fZeroCrossingSample; 
	//fDisplacement =0;
	//fZeroCrossingTime = (x  + fDisplacement) * (double)sampling_period;
	fZeroCrossingTime = x * (double)sampling_period;
	//fZeroCrossingSample = int(x + fDisplacement);

	return fZeroCrossingTime;
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! get CFD time
double DigitalCFD::Perform(DssdData * const dData,std::string type, TH2F* hcfd ){
	double cfd_time = 0;

	if(s1->CFDMethod == 0) cfd_time = static_cast<double>(dData->GetTimeStamp() * timestamp_unit);

	else if(s1->CFDMethod == 1){
		if(type.compare("DSSD")==0)
			cfd_time = static_cast<double>((dData->GetTimeStamp() * timestamp_unit) - ((s1->pre_trig_buffer - s1->nStart_trace)*sampling_period)) + GetFractionalTime(dData, s1->dssdCFDFraction);
		else if(type.compare("SED")==0)
			cfd_time = static_cast<double>((dData->GetTimeStamp() * timestamp_unit) - ((s1->pre_trig_buffer - s1->nStart_trace)*sampling_period)) + GetFractionalTime(dData, s1->sedCFDFraction);
	}
	else if(s1->CFDMethod == 2){
		if(s1->use_default_CFD_parameters){
			if(type.compare("DSSD")==0)
				cfd_time = static_cast<double>((dData->GetTimeStamp() * timestamp_unit) - ((s1->pre_trig_buffer - s1->nStart_trace) *sampling_period)) + GetCFDTime(dData, s1->dssdCFDDelay, s1->dssdCFDFraction, hcfd);
			else if(type.compare("SED")==0)
				cfd_time = static_cast<double>((dData->GetTimeStamp() * timestamp_unit) - ((s1->pre_trig_buffer - s1->nStart_trace) *sampling_period)) + GetCFDTime(dData, s1->sedCFDDelay, s1->sedCFDFraction, hcfd);
		}
		else{
			cfd_time =static_cast<double>((dData->GetTimeStamp() * timestamp_unit) - ((s1->pre_trig_buffer - s1->nStart_trace)*sampling_period)) + GetCFDTime(dData);
		}
	}
	dData->SetCFDTime(cfd_time);
	return cfd_time;
}
