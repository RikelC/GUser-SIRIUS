#include "digitalCFD.h"

digitalCFD::digitalCFD()
{
	s1 = myGlobal::getInstance();
	vout = new Double_t[s1->TRACE_SIZE];
	vin = new Double_t[s1->TRACE_SIZE];
	vindelayed = new Double_t[s1->TRACE_SIZE];
	delay = new UShort_t*[s1->NBOARDS_DSSD];
	fraction = new Double_t*[s1->NBOARDS_DSSD];
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		delay[i] = new UShort_t[NCHANNELS];
		fraction[i] = new Double_t[NCHANNELS];
	}
	//initialize some preset value
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			delay[i][j] = 10;
			fraction[i][j] = .3;
		}
	}
}

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

void digitalCFD::assign_cfd_parameters(){
	std::string line; UShort_t boardID, bIdx, chID, d, f;

	std::ifstream myfile;
	myfile.open("cfd_parameters.txt",std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> d >> f;
			bIdx = s1->fConvertNoBoardIndexLocal[boardID];
			delay[bIdx][chID] = d;
			fraction[bIdx][chID] = f;
		}
		myfile.close();
	}else{
		std::cout<<"cfd_parameters.txt file not open"<<std::endl;
	}

}


Double_t digitalCFD::getCFD_Time1(dssdData* const data,  Double_t fract){
	Double_t amplitude = data->get_signalHeight();
	Int_t max_pos = (Int_t)data->get_Max_pos();
	Double_t baseline = data->get_Baseline();
	Double_t temp = 0.;

	Double_t y2= 0., y1= 0., x2= 0., x1 = 0., y = 0., x = 0., F = 0.;

	temp = (Double_t)data->get_trace_value(max_pos) - baseline;
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
	x2 = (Double_t)(max_pos + 1)*sampling_period;
	y2 = data->get_trace_value(max_pos + 1);

	x1 = (Double_t)(max_pos)*sampling_period;
	y1 = data->get_trace_value(max_pos);

	F = (y2 - y1)/(x2 - x1);

	x = x1 + (y -y1)/F;

	return x;
}

Double_t digitalCFD::getCFD_Time2( dssdData* const data){
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	Int_t del = delay[b][ch];
	Double_t fract = fraction[b][ch];
	Int_t max_pos = data->get_Max_pos();
	Double_t cfd_time=0;
	vout[0] = 0.;
	vin[0] = 0.;
	vindelayed[0] = 0.;

	Double_t y2= 0., y1= 0.;
	Int_t x2= 0, x1 = 0;
	Double_t y = 0., x = 0., M = 0., C = 0.;

	for (Int_t i = 1; i < s1->TRACE_SIZE; i++) {
		vin[i] = data->get_trace_value(i) - data->get_Baseline();
	}


	for (Int_t j = 1; j < s1->TRACE_SIZE; j++) {
		//forward delay
		if(j <= del)
			vindelayed[j] = 0.;
		else
			vindelayed[j] = vin[j - del];
	}

	for (Int_t t =1; t < s1->TRACE_SIZE; t++) {
		vout[t] = vindelayed[t] - fract*vin[t];

		/* hcfd_vin->SetPoint(t-1,(Double_t)(t-1), vin[t]);
		   hcfd_vindelayed->SetPoint(t-1,(Double_t)(t-1), vindelayed[t]);
		   hcfd_fracvin->SetPoint(t-1,(Double_t)(t-1), fract*vin[t]);
		   hcfd->SetPoint(t-1,(Double_t)(t-1), vout[t]);*/

	}

	//find max pos in the interval (max_pos - 100, max_pos+100)
	x2 = max_pos - 101;
	y2 = vout[x2];
	for(Int_t n = max_pos - 100; n < (max_pos + 100);n++){
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
	M = (y2 - y1)/(Double_t)(x2 - x1);
	C = y1 - M*x1;
	y = 0.;
	x = (y -C)/M;

	Double_t cfdTime = x* sampling_period;
	return cfdTime;
}


Double_t digitalCFD::getCFD_Time2( dssdData* const data, Int_t del,  Double_t fract){
	Int_t max_pos = data->get_Max_pos();
	Double_t cfd_time=0;
	vout[0] = 0.;
	vin[0] = 0.;
	vindelayed[0] = 0.;

	Double_t y2= 0., y1= 0.;
	Int_t x2= 0, x1 = 0;
	Double_t y = 0., x = 0., M = 0., C = 0.;

	for (Int_t i = 1; i < s1->TRACE_SIZE; i++) {
		vin[i] = data->get_trace_value(i) - data->get_Baseline();
	}


	for (Int_t j = 1; j < s1->TRACE_SIZE; j++) {
		//forward delay
		if(j <= del)
			vindelayed[j] = 0.;
		else
			vindelayed[j] = vin[j - del];
	}

	for (Int_t t =1; t < s1->TRACE_SIZE; t++) {
		vout[t] = vindelayed[t] - fract*vin[t];

		/* hcfd_vin->SetPoint(t-1,(Double_t)(t-1), vin[t]);
		   hcfd_vindelayed->SetPoint(t-1,(Double_t)(t-1), vindelayed[t]);
		   hcfd_fracvin->SetPoint(t-1,(Double_t)(t-1), fract*vin[t]);
		   hcfd->SetPoint(t-1,(Double_t)(t-1), vout[t]);*/

	}

	//find max pos in the interval (max_pos - 100, max_pos+100)
	x2 = max_pos - 101;
	y2 = vout[x2];
	for(Int_t n = max_pos - 100; n < (max_pos + 100);n++){
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
	M = (y2 - y1)/(Double_t)(x2 - x1);
	C = y1 - M*x1;
	y = 0.;
	x = (y -C)/M;

	Double_t cfdTime = x* sampling_period;
	return cfdTime;
}


Double_t digitalCFD::getCFD_Time3( dssdData* const data,  Double_t fract){
	Int_t max_pos = data->get_Max_pos();
	Double_t temp = 0.;
	unsigned int pts = 4;

	Double_t *x = new Double_t[pts];
	Double_t *y = new Double_t[pts];

	Double_t y_interp = 0., x_interp =0.;
	y_interp = fract*data->get_signalHeight();
	temp = data->get_trace_value(max_pos) - data->get_Baseline();

	while ( temp > y_interp && (max_pos>0)){
		max_pos--;
		temp = data->get_trace_value(max_pos) - data->get_Baseline();
	}

	max_pos--;//
	for(Int_t p = 0; p < pts;p++){
		x[p] = (double) max_pos;
		y[p] = data->get_trace_value(max_pos) -data->get_Baseline();
		max_pos++;
	}

	ROOT::Math::Interpolator inter (pts, ROOT::Math::Interpolation::kCSPLINE);
	//ROOT::Math::Interpolation::Type type  : kLINEAR, kPOLYNOMIAL, kCSPLINE, kAKIMA, kCSPLINE_PERIODIC, kAKIMA_PERIODIC

	inter.SetData (pts, y, x);
	x_interp = inter.Eval(y_interp);
	delete [] x;
	delete [] y;
	return x_interp;
}
