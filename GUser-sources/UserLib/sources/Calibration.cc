#include "Calibration.h"
Calibration::Calibration(){
	s1 = myGlobal::getInstance();
	calib_m = new Double_t*[s1->NBOARDS_DSSD];
	calib_c = new Double_t*[s1->NBOARDS_DSSD];
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){;
		calib_m[i] = new Double_t[NCHANNELS];
		calib_c[i] = new Double_t[NCHANNELS];
	}
	rand = new TRandom3;
	//initialize some preset value
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			calib_m[i][j] = 3.6;
			calib_c[i][j] = 30;
		}
	}

}

Calibration::~Calibration(){

	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		delete [] calib_m[i];
		delete [] calib_c[i];
	}
	delete [] calib_m;
	delete [] calib_c;
	delete rand;
}


void Calibration::assign_dssd_calibration_parameters(){
	std::string line; int boardID, bIdx, chID;
	double c_gain, c_offset;

	std::ifstream myfile;
	myfile.open(s1->dssd_calib_filename,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> c_gain >> c_offset;

			bIdx = s1->fConvertNoBoardIndexLocal[boardID];
			calib_m[bIdx][chID] = c_gain;
			calib_c[bIdx][chID] = c_offset;
		}
		myfile.close();
	}else{std::cout <<s1->dssd_calib_filename<<" file could not be opened..."<<std::endl;}
}

Double_t Calibration::perform(dssdData* const data){
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	Double_t calE = calib_m[b][ch]*(data->get_filter_Amplitude() + rand->Uniform(0, 1)) + calib_c[b][ch];
	return calE;
}
