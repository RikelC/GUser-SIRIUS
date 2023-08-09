/*!
 * \file Calibration.cc
 *  \author Rikel CHAKMA
 *  \brief
 *  \details
 *
 */


#include "Calibration.h"
//! Constructor
/*! Create 2D parameter arrays. Initialize with default values by setting gain[][] = 1. and offset[][] = 0.
 * Perform AssignDssdCalibrationParametersi(), AssignTunnelCalibrationParameters() and assign_ge_Calibration_parameters().
 *
 *
 */
Calibration::Calibration(){
	s1 = MyGlobal::GetInstance();
	fGainHG = new double*[s1->NBOARDS_DSSD];
	fOffsetHG = new double*[s1->NBOARDS_DSSD];
	fGainLG = new double*[s1->NBOARDS_DSSD];
	fOffsetLG = new double*[s1->NBOARDS_DSSD];
	fGainTunnel  = new double*[s1->NBOARDS_TUNNEL];
	fOffsetTunnel  = new double*[s1->NBOARDS_TUNNEL];
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){;
		fGainHG[i] = new double[NCHANNELS];
		fOffsetHG[i] = new double[NCHANNELS];
		fGainLG[i] = new double[NCHANNELS];
		fOffsetLG[i] = new double[NCHANNELS];
	}
	for(int i = 0; i < s1->NBOARDS_TUNNEL; i++){;
		fGainTunnel[i] = new double[NCHANNELS];
		fOffsetTunnel[i] = new double[NCHANNELS];
	}
	rand = new TRandom3;
	//initialize some preset value
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			fGainHG[i][j] = 1.;
			fOffsetHG[i][j] = 0.;
			fGainLG[i][j] = 1.;
			fOffsetLG[i][j] = 0.;
		}
	}
	for(int i = 0; i < s1->NBOARDS_TUNNEL; i++){
		for(int j = 0; j < NCHANNELS; j++){
			fGainHG[i][j] = 1.;
			fOffsetHG[i][j] = 0.;
			fGainLG[i][j] = 1.;
			fOffsetLG[i][j] = 0.;
		}
	}
	AssignDssdCalibrationParameters();
	AssignTunnelCalibrationParameters();

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
/*! Clear heap memory.
*/
Calibration::~Calibration(){

	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		delete [] fGainHG[i];
		delete [] fOffsetHG[i];
		delete [] fGainLG[i];
		delete [] fOffsetLG[i];
	}
	delete [] fGainHG;
	delete [] fOffsetHG;
	delete [] fGainLG;
	delete [] fOffsetLG;
	for(int i = 0; i < s1->NBOARDS_TUNNEL; i++){
		delete [] fGainTunnel[i];
		delete [] fOffsetTunnel[i];
	}
	delete [] fGainTunnel;
	delete [] fOffsetTunnel;

	delete rand;
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Assignment of DSSD Calibration parameters
/*! Read the DSSD high gain and low gain Calibration parameter files defined in Run.config and iitialize the DSSD 2D parameter variables.
*/
void Calibration::AssignDssdCalibrationParameters(){
	std::string line; int boardID, bIdx, chID;
	double c_gain, c_offset;

	std::ifstream myfile;
	myfile.open(s1->dssd_HGcalib_filename,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> c_gain >> c_offset;

			bIdx = s1->boardIndex_DSSD[boardID];
			fGainHG[bIdx][chID] = c_gain;
			fOffsetHG[bIdx][chID] = c_offset;
			//cout<<"boardID "<<boardID<<" idx "<<bIdx<<" ch "<<chID<<"  gain "<<c_gain<<"  offset "<<c_offset<<endl;
		}
		myfile.close();
	}
	else{
		std::cout <<s1->dssd_HGcalib_filename<<" file could not be opened..."<<std::endl;
	}
	myfile.open(s1->dssd_LGcalib_filename,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> c_gain >> c_offset;

			bIdx = s1->boardIndex_DSSD[boardID];
			fGainLG[bIdx][chID] = c_gain;
			fOffsetLG[bIdx][chID] = c_offset;
		}
		myfile.close();
	}
	else{
		std::cout <<s1->dssd_LGcalib_filename<<" file could not be opened..."<<std::endl;
	}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Assignment of TUNNEL Calibration parameters
/*! Read the TUNNEL iCalibration parameter file defined in Run.config and iitialize the TUNNEL 2D parameter variables.
*/

void Calibration::AssignTunnelCalibrationParameters(){
	std::string line; int boardID, bIdx, chID;
	double c_gain, c_offset;

	std::ifstream myfile;
	myfile.open(s1->tunnel_calib_filename,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> c_gain >> c_offset;

			bIdx = s1->boardIndex_Tunnel[boardID];
			fGainTunnel[bIdx][chID] = c_gain;
			fOffsetTunnel[bIdx][chID] = c_offset;
			//cout<<"boardID "<<boardID<<" idx "<<bIdx<<" ch "<<chID<<"  gain "<<c_gain<<"  offset "<<c_offset<<endl;
		}
		myfile.close();
	}
	else{
		std::cout <<s1->tunnel_calib_filename<<" file could not be opened..."<<std::endl;
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Calibration of DSSD
/*! Perform Calibration of the dssdData object.
*/
double Calibration::Perform(DssdData* const data){
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	double calE = 0.;//calib_m[b][ch]*(data->GetRawEnergy() + rand->Uniform(0, 1)) + calib_c[b][ch];

	if(data->GetStripNumber() >=0 && data->GetStripNumber() < 128){
		if(s1->FRONT_FPCSA_GAIN_MODE == "HG"){
			calE = ((std::abs(data->GetRawEnergy())) * fGainHG[b][ch]) + fOffsetHG[b][ch];
		}
		else if(s1->FRONT_FPCSA_GAIN_MODE == "LG"){
			calE = ((std::abs(data->GetRawEnergy())) * fGainLG[b][ch]) + fOffsetLG[b][ch];
		}
		else if(s1->FRONT_FPCSA_GAIN_MODE == "AUTO"){
			if(data->GainSwitched() ){
				calE = ((std::abs(data->GetRawEnergy())) * fGainLG[b][ch]) + fOffsetLG[b][ch];
			}
			else{
				calE = ((std::abs(data->GetRawEnergy())) * fGainHG[b][ch]) + fOffsetHG[b][ch];
			}

		}
	}
	else if(data->GetStripNumber() > 127 && data->GetStripNumber() < 256){
		if(s1->BACK_FPCSA_GAIN_MODE == "HG"){
			calE = ((data->GetRawEnergy()) * fGainHG[b][ch]) + fOffsetHG[b][ch];
		}
		else if(s1->BACK_FPCSA_GAIN_MODE == "LG"){
			calE = ((data->GetRawEnergy()) * fGainLG[b][ch]) + fOffsetLG[b][ch];
		}
		else if(s1->BACK_FPCSA_GAIN_MODE == "AUTO"){
			if(data->GainSwitched() ){
				calE = ((data->GetRawEnergy()) * fGainLG[b][ch]) + fOffsetLG[b][ch];
			}
			else{
				calE = ((data->GetRawEnergy()) * fGainHG[b][ch]) + fOffsetHG[b][ch];
			}

		}
	}
	data->SetCalibratedEnergy(calE);
	return calE;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Calibration of TUNNEL
/*! Perform Calibration of the TunnelData object.
*/

double Calibration::Perform(TunnelData* const data){
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();

	double calE = ((std::abs(data->GetRawEnergy()) ) * fGainTunnel[b][ch]) + fOffsetTunnel[b][ch];

	data->SetCalibratedEnergy(calE);
	return calE;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Calibration of Ge
/*! Perform Calibration of the TunnelData object.
*/

double Calibration::Perform(GeData* const data){
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();

	double calE = ((std::abs(data->GetRawEnergy()) ) * fGainTunnel[b][ch]) + fOffsetTunnel[b][ch];

	data->SetCalibratedEnergy(calE);
	return calE;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------/---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
