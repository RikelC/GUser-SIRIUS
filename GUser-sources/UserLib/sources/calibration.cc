/*!
 * \file calibration.cc
 *  \author Rikel CHAKMA
 *  \brief
 *  \details
 *
 */


#include "calibration.h"
//! Constructor
/*! Create 2D parameter arrays. Initialize with default values by setting gain[][] = 1. and offset[][] = 0.
 * perform assign_dssd_calibration_parametersi(), assign_tunnel_calibration_parameters() and assign_ge_calibration_parameters().
 *
 *
 */
calibration::calibration(){
	s1 = myGlobal::getInstance();
	calib_m_hg = new double*[s1->NBOARDS_DSSD];
	calib_c_hg = new double*[s1->NBOARDS_DSSD];
	calib_m_lg = new double*[s1->NBOARDS_DSSD];
	calib_c_lg = new double*[s1->NBOARDS_DSSD];
	calib_m_t  = new double*[s1->NBOARDS_TUNNEL];
	calib_c_t  = new double*[s1->NBOARDS_TUNNEL];
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){;
		calib_m_hg[i] = new double[NCHANNELS];
		calib_c_hg[i] = new double[NCHANNELS];
		calib_m_lg[i] = new double[NCHANNELS];
		calib_c_lg[i] = new double[NCHANNELS];
	}
	for(int i = 0; i < s1->NBOARDS_TUNNEL; i++){;
		calib_m_t[i] = new double[NCHANNELS];
		calib_c_t[i] = new double[NCHANNELS];
	}
	rand = new TRandom3;
	//initialize some preset value
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			calib_m_hg[i][j] = 1.;
			calib_c_hg[i][j] = 0.;
			calib_m_lg[i][j] = 1.;
			calib_c_lg[i][j] = 0.;
		}
	}
	for(int i = 0; i < s1->NBOARDS_TUNNEL; i++){
		for(int j = 0; j < NCHANNELS; j++){
			calib_m_hg[i][j] = 1.;
			calib_c_hg[i][j] = 0.;
			calib_m_lg[i][j] = 1.;
			calib_c_lg[i][j] = 0.;
		}
	}
	assign_dssd_calibration_parameters();
	assign_tunnel_calibration_parameters();

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
/*! Clear heap memory.
*/
calibration::~calibration(){

	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		delete [] calib_m_hg[i];
		delete [] calib_c_hg[i];
		delete [] calib_m_lg[i];
		delete [] calib_c_lg[i];
	}
	delete [] calib_m_hg;
	delete [] calib_c_hg;
	delete [] calib_m_lg;
	delete [] calib_c_lg;
	for(int i = 0; i < s1->NBOARDS_TUNNEL; i++){
		delete [] calib_m_t[i];
		delete [] calib_c_t[i];
	}
	delete [] calib_m_t;
	delete [] calib_c_t;

	delete rand;
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Assignment of DSSD calibration parameters
/*! Read the DSSD high gain and low gain calibration parameter files defined in Run.config and iitialize the DSSD 2D parameter variables.
*/
void calibration::assign_dssd_calibration_parameters(){
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
			calib_m_hg[bIdx][chID] = c_gain;
			calib_c_hg[bIdx][chID] = c_offset;
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
			calib_m_lg[bIdx][chID] = c_gain;
			calib_c_lg[bIdx][chID] = c_offset;
		}
		myfile.close();
	}
	else{
		std::cout <<s1->dssd_LGcalib_filename<<" file could not be opened..."<<std::endl;
	}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Assignment of TUNNEL calibration parameters
/*! Read the TUNNEL icalibration parameter file defined in Run.config and iitialize the TUNNEL 2D parameter variables.
*/

void calibration::assign_tunnel_calibration_parameters(){
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
			calib_m_t[bIdx][chID] = c_gain;
			calib_c_t[bIdx][chID] = c_offset;
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
double calibration::perform(dssdData* const data){
	int b = data->get_boardIdx();
	int ch = data->get_channelID();
	double calE = 0.;//calib_m[b][ch]*(data->get_raw_energy() + rand->Uniform(0, 1)) + calib_c[b][ch];

	if(data->get_stripnumber() >=0 && data->get_stripnumber() < 128){
		if(s1->FRONT_FPCSA_GAIN_MODE == "HG"){
			calE = ((std::abs(data->get_raw_energy()) + rand->Uniform(0,1)) * calib_m_hg[b][ch]) + calib_c_hg[b][ch];
		}
		else if(s1->FRONT_FPCSA_GAIN_MODE == "LG"){
			calE = ((std::abs(data->get_raw_energy()) + rand->Uniform(0,1)) * calib_m_lg[b][ch]) + calib_c_lg[b][ch];
		}
		else if(s1->FRONT_FPCSA_GAIN_MODE == "AUTO"){
			if(data->gain_switched() ){
				calE = ((std::abs(data->get_raw_energy()) + rand->Uniform(0,1)) * calib_m_lg[b][ch]) + calib_c_lg[b][ch];
			}
			else{
				calE = ((std::abs(data->get_raw_energy()) + rand->Uniform(0,1)) * calib_m_hg[b][ch]) + calib_c_hg[b][ch];
			}

		}
	}
	else if(data->get_stripnumber() > 127 && data->get_stripnumber() < 256){
		if(s1->BACK_FPCSA_GAIN_MODE == "HG"){
			calE = ((data->get_raw_energy() + rand->Uniform(0,1)) * calib_m_hg[b][ch]) + calib_c_hg[b][ch];
		}
		else if(s1->BACK_FPCSA_GAIN_MODE == "LG"){
			calE = ((data->get_raw_energy() + rand->Uniform(0,1)) * calib_m_lg[b][ch]) + calib_c_lg[b][ch];
		}
		else if(s1->BACK_FPCSA_GAIN_MODE == "AUTO"){
			if(data->gain_switched() ){
				calE = ((data->get_raw_energy() + rand->Uniform(0,1)) * calib_m_lg[b][ch]) + calib_c_lg[b][ch];
			}
			else{
				calE = ((data->get_raw_energy() + rand->Uniform(0,1)) * calib_m_hg[b][ch]) + calib_c_hg[b][ch];
			}

		}
	}
	data->set_calibrated_energy(calE);
	return calE;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Calibration of TUNNEL
/*! Perform Calibration of the tunnelData object.
*/

double calibration::perform(tunnelData* const data){
	int b = data->get_boardIdx();
	int ch = data->get_channelID();

	double calE = ((std::abs(data->get_raw_energy()) + rand->Uniform(0,1)) * calib_m_t[b][ch]) + calib_c_t[b][ch];

	data->set_calibrated_energy(calE);
	return calE;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
