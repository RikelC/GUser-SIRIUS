/*!
 * \file Global.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */

#include "Global.h"
#include "DssdParameters.h"
//!convert a string of lowe case to upper case
void ConvertToUpperCase(std::string &variable){ 

	std::locale loc;
	std::string str = variable;
	variable.clear();
	for(std::string::size_type i=0; i <str.length();++i){
		if(std::isalpha(str[i],loc))
			variable.append(1, std::toupper(str[i], loc));
		else
			variable.append(1, str[i]);
	}
	str.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//--------------------------
// Removal of extra spaces
//--------------------------
//needs to be checked
void RemoveExtraSpaces(std::string &variable){
	char ch; int space_counter =0;
	std::string temp = variable;
	variable.clear();
	std::string::size_type i_start =0; 
	//ignore white spaces in the begining
	do{
		ch = temp[i_start];
		i_start++;
	}
	while(isspace(ch));

	i_start -=1;

	for(std::string::size_type i = i_start ;i< temp.length();i++){
		ch = temp[i];

		if(isalnum(ch)){
			variable.append(1, ch);
			space_counter = 0;
		}
		else if(isspace(ch)){
			space_counter++;
			if(space_counter ==1)
				variable.append(1, ch);
		}

	}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//--------------------------
// Check if the line is empty 
//--------------------------
bool IsEmptyLine(std::string &line){

	bool check = 1;
	for(std::string::size_type i = 0; i< line.length();i++){
		if(line[i] == ' ' || line[i] == '\n')continue; else{ check = 0; break;}
	}

	return check;
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Global function: split a string into sub strings based on the delimiter
std::vector<std::string> SplitString(std::string str, std::string delimiter){
	std::vector<std::string> vec;
	std::size_t pos =0;
	std::string token;
	while((pos = str.find(delimiter)) != std::string::npos ){
		token = str.substr(0,pos);
		str.erase(0, pos + delimiter.length());
		if(isalnum(token[0]))vec.push_back(token);
	}
	if(isalnum(str[0]))vec.push_back(str);
	return vec;
}


//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! get instance of the Global class
MyGlobal* MyGlobal::instance = 0;
MyGlobalDestroyer MyGlobal::destroyer;
MyGlobal* MyGlobal::GetInstance() {
	if(!instance) {
		instance = new MyGlobal();
		destroyer.SetMyGlobal(instance);
	}
	return instance;
}

MyGlobal* MyGlobal::GetInstance(int mode) {
	if(!instance) {
		instance = new MyGlobal(mode);
		destroyer.SetMyGlobal(instance);
	}
	return instance;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor
/*!
 * 
 */
MyGlobal::MyGlobal(){
	// read the configuration file
	ReadRunConfigFile();
	//-initialize
	frameName[0]= "Cobo";
	frameName[1]= "Eby";
	frameName[2]= "Hello";
	frameName[3]= "xml header";
	frameName[4]= "rea trace";
	frameName[5]= "rea gen";
	frameName[6]= "sirius";
	frameName[7]= "merge";
	frameName[8]= "else";
	boardIndex_DSSD = new int[200];
	boardIndex_Tunnel = new int[200];
	for(int i = 0; i < NBOARDS_DSSD; i++) boardIndex_DSSD[boardList_DSSD[i]] = i;
	for(int i = 0; i < NBOARDS_TUNNEL; i++) boardIndex_Tunnel[boardList_Tunnel[i]] = i;
	NSTRIPS_DSSD = NBOARDS_DSSD * NCHANNELS;
	NofMacroPixels = 24 * NDETECTOR_TUNNEL;

}
MyGlobal::MyGlobal(int mode){
	// read the configuration file
	readMode = mode;
	ReadRunConfigFile();
	//-initialize
	frameName[0]= "Cobo";
	frameName[1]= "Eby";
	frameName[2]= "Hello";
	frameName[3]= "xml header";
	frameName[4]= "rea trace";
	frameName[5]= "rea gen";
	frameName[6]= "sirius";
	frameName[7]= "merge";
	frameName[8]= "else";
	boardIndex_DSSD = new int[200];
	boardIndex_Tunnel = new int[200];
	for(int i = 0; i < NBOARDS_DSSD; i++) boardIndex_DSSD[boardList_DSSD[i]] = i;
	for(int i = 0; i < NBOARDS_TUNNEL; i++) boardIndex_Tunnel[boardList_Tunnel[i]] = i;
	NSTRIPS_DSSD = NBOARDS_DSSD * NCHANNELS;
	NofMacroPixels = 24 * NDETECTOR_TUNNEL;

}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
/*/
 *
 *
 */
MyGlobal::~MyGlobal(){
	delete [] boardIndex_DSSD;
	delete [] boardIndex_Tunnel;
	boardList_DSSD.clear();
	nChannel_DSSD.clear();
	boardList_Tunnel.clear();
	cout<<"MyGlobal destructor called."<<endl;

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Check if the board is a dssd board
bool MyGlobal::from_dssd(int &board){
	bool check = false;
	for(int i =0; i <NBOARDS_DSSD;i++){if(board == boardList_DSSD[i]) check = true;}
	return check;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Check is the board is a tunnel board
bool MyGlobal::from_tunnel(int &board){
	bool check = false;
	for(int i =0; i <NBOARDS_TUNNEL;i++){if(board == boardList_Tunnel[i]) check = true;}
	return check;
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Read the Run.Config file
/*!
 *
 *
 *
 */
void MyGlobal::ReadRunConfigFile(){
	NDETECTOR_TUNNEL = 0;
	dssdFormat.clear();
	tunnelFormat.clear();

	DSSD_PARAMETERS::filterAlgorithm = "Trap1";
	ifstream file;
	if(readMode == 1)file.open("ConfigFiles/Run_online.config", std::iostream::in);
	else if(readMode == 2)file.open("ConfigFiles/Run_offline.config", std::iostream::in);
	else
		file.open("ConfigFiles/Run.config", std::iostream::in);
	std::string line, variable, value; bool eqality_sign_found; 
	if(file.is_open()){
		while(file.good() && !file.eof()){
			eqality_sign_found = false;
			std::getline(file, line);
			variable.clear(); value.clear();
			for(size_t i = 0; i < line.length(); i++)
			{
				if(line[i] == '#')break; // is a comment
				// remove front spaces
				if(line[i] == ' ' || line[i] == '\n'){ continue;
				}
				else{
					if(line[i] != '='){
						if(!eqality_sign_found)
							variable.append(1, line[i]);
						else
							value.append(1, line[i]);
					}
					else{
						eqality_sign_found = true;

					}
				}

			}
			//	}
			// It is a comment
			if(line[0] == '/' && line[1] == '*') continue;			
			//----------
			//get rid of comments in value
			//cout<<variable<<"     "<<value<<endl;
			size_t pos_comment = value.find("//");
			//cout<<"after  "<<variable<<"     "<<value<<endl;
			if(pos_comment !=std::string::npos)
				value.erase(pos_comment, value.length());

			if(!variable.empty() && !value.empty()){
				ConvertToUpperCase(variable);
				if(variable.compare("ACQUISITIONMODE")==0){
					ConvertToUpperCase(value);
					acquisitionMode = value;
				}


				else if(variable.compare("SAVETTREEFILE")==0){
					ConvertToUpperCase(value);
					if(value == "YES")
						fsaveTree = true;
					else fsaveTree = false;
				}
				else if(variable.compare("SAVEHISTOGRAMFILE")==0){
					ConvertToUpperCase(value);
					if(value == "YES")
						fsaveHisto = true;
					else fsaveHisto = false;
				}

				//verbose level
				else if(variable.compare("VERBOSELEVEL")==0)
					fverbose = std::stoi(value);
				//Data Merged? 
				else if(variable.compare("DATAMERGED")==0){
					ConvertToUpperCase(value);
					if(value == "YES") data_merged = true;
					else data_merged = false;
				}
				//buffer size
				else if(variable.compare("BUFFERSIZE")==0)
					buffer_size = std::stoul(value);

				//size of pre-trig-buffer				
				else if(variable.compare("PRETRIGBUFFER")==0)
					pre_trig_buffer = std::stoi(value);
				//number of sample to be ignored in the begining				
				else if(variable.compare("NUMBEROFSAMPLESTOBEIGNOREDATSTART")==0)
					nStart_trace = std::stoi(value);
				// number of samples to be ignored in the end	
				else if(variable.compare("NUMBEROFSAMPLESTOBEIGNOREDATEND")==0)
					nEnd_trace = std::stoi(value);
				// trace size		
				else if(variable.compare("TRACESIZE")==0)
					TRACE_SIZE = std::stoi(value);
				//FPCSA gain
				else if(variable.compare("BACKFPCSAGAINMODE")==0)
				{
					ConvertToUpperCase(value);
					BACK_FPCSA_GAIN_MODE = value;
				}
				else if(variable.compare("BACKHIGHGAIN")==0)
					backHighGain = std::stod(value);
				else if(variable.compare("BACKLOWGAIN")==0)
					backLowGain = std::stod(value);
				else if(variable.compare("BACKFPCSALEVELCOMPARATOR")==0){
					back_gainSwitch_threshold =  std::stoi(value);
				}
				else if(variable.compare("FRONTFPCSAGAINMODE")==0)
				{
					ConvertToUpperCase(value);
					FRONT_FPCSA_GAIN_MODE = value;
				}
				else if(variable.compare("FRONTHIGHGAIN")==0)
					frontHighGain = std::stod(value);
				else if(variable.compare("FRONTLOWGAIN")==0)
					frontLowGain = std::stod(value);
				else if(variable.compare("FRONTFPCSALEVELCOMPARATOR")==0){
					front_gainSwitch_threshold = std::stoi(value);
				}	
				else if(variable.compare("FRONTGAINSWITCHDURATION")==0){
					front_gs_duration = std::stoi(value);
				}	
				else if(variable.compare("BACKGAINSWITCHDURATION")==0){
					back_gs_duration = std::stoi(value);
				}	
				else if(variable.compare("MOVINGWINDOWFORGAINSWITCHCALCULATION")==0)
					mv_window_calcul_gainSwitch = std::stoi(value);
				else if(variable.compare("USEDEFAULTFPCSAGAIN")==0){
					ConvertToUpperCase(value);
					if(value == "YES") use_default_FPCSA_Gain = true;
					else use_default_FPCSA_Gain = false;					
				}
				//Decay constat Calculation
				else if(variable.compare("MOVINGWINDOWFORHIGHGAINDECAYCONSTANTCALCULATION")==0)
					mv_window_calcul_HG = std::stoi(value);
				else if(variable.compare("MOVINGWINDOWFORLOWGAINDECAYCONSTANTCALCULATION")==0)
					mv_window_calcul_LG = std::stoi(value);

				else if(variable.compare("NUMBEROFSAMPLESTOBESKIPPEDFORLOWGAINDECAYCONSTANTCALCULATION")==0)
					skip_nSamples_calcul_LG = std::stoi(value);
				else if(variable.compare("NUMBEROFSAMPLESTOBESKIPPEDFORHIGHGAINDECAYCONSTANTCALCULATION")==0)
					skip_nSamples_calcul_HG = std::stoi(value);
				//Signal identification
				else if(variable.compare("DECAYTIMETHRESHOLD")==0)
					decayTime_threshold = std::stoi(value);
				else if(variable.compare("SATURATIONTIMETHRESHOLD")==0)
					saturationTime_threshold = std::stoi(value);
				else if(variable.compare("TRIGGERDEFINITION ")==0)
					trigger_def = static_cast<unsigned short int>(std::stoi(value));
				else if(variable.compare("RISETIMEDEFINITION")==0)
					riseTime_def = static_cast<unsigned short int>(std::stod(value));
				// Trapezoidal algorithm			
				else if(variable.compare("FILTERALGORITHM")==0){
					ConvertToUpperCase(value);
					filterAlgorithm = value;	
				}
				else if(variable.compare("USEDEFAULTFILTERPARAMETER")==0){
					ConvertToUpperCase(value);
					if(value == "YES")
						use_default_filter_parameters = 1;
					else 
						use_default_filter_parameters = 0;
				}
				// default K value			
				else if(variable.compare("DEFAULTKVALUE")==0)
					default_trap_k = std::stoi(value);
				// default M value		
				else if(variable.compare("DEFAULTMVALUE")==0)
					default_trap_m = std::stoi(value);
				//Shaper Amplification gain
				else if(variable.compare("SHAPERAMPLIFICATIONGAIN")==0)
					shaperAmplificationGain = std::stod(value);
				else if(variable.compare("TRAPEZOIDALFLATTOPMEASUREMENTMETHOD")==0){
					trap_flattop_mode = std::stoi(value);
				}

				else if(variable.compare("HEIGHTFRACTIONLIMITWITHRESPECTTOMAXIMUMHEIGHT")==0)
					height_fraction_MaxTrapHeight = std::stod(value);
				//Trapezoidal parameter file		
				else if(variable.compare("TRAPEZOIDALPARAMETERFILE")==0)
					trap_parameter_filename = value;

				else if(variable.compare("USEDEFAULTCFDPARAMETERS")==0){
					ConvertToUpperCase(value);
					if(value=="YES") use_default_CFD_parameters = 1;
					else use_default_CFD_parameters = 0;
				}
				else if(variable.compare("CFDMETHOD")==0)
					CFDMethod = std::stoi(value);
				else if(variable.compare("DSSDDELAY")==0)
					dssdCFDDelay = std::stoi(value);
				else if(variable.compare("DSSDFRACTION")==0)
					dssdCFDFraction = std::stod(value);
				else if(variable.compare("SEDDELAY")==0)
					sedCFDDelay = std::stoi(value);
				else if(variable.compare("SEDFRACTION")==0)
					sedCFDFraction = std::stod(value);

				//CFD parameter file		
				else if(variable.compare("DSSDCFDPARAMETERFILE")==0)
					cfd_parameter_filename_dssd = value;
				else if(variable.compare("SEDCFDPARAMETERFILE")==0)
					cfd_parameter_filename_sed = value;

				//----------------------
				//MB1
				///----------------------
				else if(variable.compare("MB1P4BOARD1")==0){// dssd board index 0
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB1P4BOARD2")==0){// dssd board index 1
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB1P5BOARD1")==0){// dssd board index 2
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB1P5BOARD2")==0){// dssd board index 3
					dssdFormat += "," + value;
				}
				//-------------				
				//MB2
				//-----------
				else if(variable.compare("MB2P4BOARD1")==0){// dssd board index 4
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB2P4BOARD2")==0){// dssd board index 5
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB2P5BOARD1")==0){// dssd board index 6
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB2P5BOARD2")==0){// dssd board index 7
					dssdFormat += "," + value;
				}
				//----------				
				//MB3
				////--------
				else if(variable.compare("MB3P4BOARD1")==0){// dssd board index 8
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB3P4BOARD2")==0){// dssd board index 9
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB3P5BOARD1")==0){// dssd board index 10
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB3P5BOARD2")==0){// dssd board index 11
					dssdFormat += "," + value;
				}
				//----------				
				//MB4
				//-----------
				else if(variable.compare("MB4P4BOARD1")==0){// dssd board index 12
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB4P4BOARD2")==0){// dssd board index 13
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB4P5BOARD1")==0){// dssd board index 14
					dssdFormat += "," + value;
				}
				else if(variable.compare("MB4P5BOARD2")==0){// dssd board index 15
					dssdFormat += "," + value;
				}
				//Tracker DSSD
				else if(variable.compare("TRACKERNUMEXO2")==0){
					//		dssdFormat += "," + value;// included in the DSSD board list as well to be able to create the histograms using the DSSD variables(see GUser.C)
					trackerFormat =  value;
				}


				// DSSD calibration file				
				else if(variable.compare("DSSDHIGHGAINCALIBRATIONFILE")==0)
					dssd_HGcalib_filename = value;
				else if(variable.compare("DSSDLOWGAINCALIBRATIONFILE")==0)
					dssd_LGcalib_filename = value;
				else if(variable.compare("TIMESTAMPALIGNMENTFILE")==0)
					timestampAlignment_filename = value;
				else if(variable.compare("CFDTIMEALIGNMENTFILE")==0)
					cfd_timeAlignment_filename = value;
				// sum energies of the neighboring strips			
				else if(variable.compare("SUMENERGIESOFTHENEIGHBORINGDSSDSTRIPS")==0){
					ConvertToUpperCase(value);
					if(value.compare("YES") == 0)
						sum_neighboring_strips = true;
					else 
						sum_neighboring_strips = false;
				}
				//DSSD coincidence window for pixel building
				else if(variable.compare("DSSDFRONT-FRONTCOINCIDENCEWINDOW")==0)
					ff_window = std::stoi(value);
				else if(variable.compare("DSSDFRONT-BACKCOINCIDENCEWINDOW")==0)
					fb_window = std::stoi(value);
				else if(variable.compare("DSSDBACK-FRONTCOINCIDENCEWINDOW")==0)
					bf_window = std::stoi(value);
				else if(variable.compare("DSSDBACK-BACKCOINCIDENCEWINDOW")==0)
					bb_window = std::stoi(value);
				else if(variable.compare("RATECALCULATIONTIMELAPSE")==0){
					rate_calcul_time_lapse = std::stoi(value);
				}
				// ToF Correlations
				else if(variable.compare("TOFLOWER")==0){
					ToF_lower = std::stoll(value);
				}
				else if(variable.compare("TOFUPPER")==0){
					ToF_upper = std::stoll(value);
				}

				//-------------
				//Tunnel
				//-----------

				else if(variable.compare("TUNNEL1")==0){
					NDETECTOR_TUNNEL += 1;			
					tunnelFormat += "," + value;
				}
				else if(variable.compare("TUNNEL2")==0){
					NDETECTOR_TUNNEL += 10;			
					tunnelFormat += "," + value;
				}
				else if(variable.compare("TUNNEL3")==0){
					NDETECTOR_TUNNEL += 100;			
					tunnelFormat += "," + value;
				}
				else if(variable.compare("TUNNEL4")==0){
					NDETECTOR_TUNNEL += 1000;			
					tunnelFormat += "," + value;
				}
				//tunnel calibration file
				else if(variable.compare("TUNNELCALIBRATIONFILE")==0)
					tunnel_calib_filename = value;

				//cout<<"variable "<<variable<<" value "<<value<<endl;
			}
	}
	//---------------
	// Close the file
	// ---------------
	file.close();
	//dssd list
	std::vector<std::string> list = SplitString(dssdFormat, ",");
	for(int i = 0; i < list.size(); i++){
		std::vector<std::string>temp = SplitString(list[i], ":");
		int board = std::stoi(temp[0]);
		std::vector<std::string>channelRange = SplitString(temp[1], "-");
		int nChannels = std::stoi(channelRange[1])-std::stoi(channelRange[0]) + 1;
		boardList_DSSD.push_back(board);
		nChannel_DSSD.push_back(nChannels);
		temp.clear();
	}
	//----------------------
	// check if the boards are duplicate
	// -----------------------
	//Check for double value if the user had made a mistake in defining the run numbers
	std::vector<int>common_ones;
	for(unsigned int n =0; n<boardList_DSSD.size();n++){	
		unsigned int counter =0;
		for(unsigned int m =n; m<boardList_DSSD.size();m++){
			if(n != m){
				if(boardList_DSSD[n] == boardList_DSSD[m]){counter++; common_ones.push_back(m);}
			}
		}
		n += counter;
	}	
	//remove the common runs
	if(common_ones.size() >0){
		unsigned int idx =0;
		for(unsigned int k =0; k<common_ones.size();k++){
			boardList_DSSD.erase(boardList_DSSD.begin()+common_ones[k]-idx);idx++;
			nChannel_DSSD.erase(nChannel_DSSD.begin()+common_ones[k]-idx);idx++;
		}
		common_ones.clear();
	}


	list.clear();

	// Extract Tracker numexo board, channel and strip number 
	std::vector<std::string>temp = SplitString(trackerFormat, ":");
	trackerNumexoBoard = std::stoi(temp[0]);
	std::vector<std::string>channelRange = SplitString(temp[1], "-");
	int nChannels = std::stoi(channelRange[1])-std::stoi(channelRange[0]) + 1;
	trackerNumexoChannel = std::stoi(channelRange[0]);
	trackerNumexoStrip = std::stoi(temp[2]);
	temp.clear();

	//Add the tracker numexo2 board to the dssd board list
	//first check if the board used for the tracker is a dssd board

	bool trackerBoard_is_a_dssdBoard = false;
	for(unsigned int n =0; n<boardList_DSSD.size();n++){	

		if(boardList_DSSD[n] == trackerNumexoBoard) trackerBoard_is_a_dssdBoard = true;
	}
	if(!trackerBoard_is_a_dssdBoard){

		dssdFormat += "," + trackerFormat;
		boardList_DSSD.push_back(trackerNumexoBoard);
		nChannel_DSSD.push_back(nChannels);

	}
	//for(int i =0; i < nChannel_DSSD.size();i++)cout<<"board : "<<boardList_DSSD[i]<<"  nChannels "<<nChannel_DSSD[i]<<endl;
	NBOARDS_DSSD = boardList_DSSD.size();
	//tunnel list
	list = SplitString(tunnelFormat, ",");
	for(int i = 0; i < list.size(); i++){
		std::vector<std::string>temp = SplitString(list[i], ":");
		int board = std::stoi(temp[0]);
		boardList_Tunnel.push_back(board);
		temp.clear();
	}

	//----------------------
	// check if the boards are duplicate
	// -----------------------
	for(unsigned int n =0; n<boardList_Tunnel.size();n++){	
		unsigned int counter =0;
		for(unsigned int m =n; m<boardList_Tunnel.size();m++){
			if(n != m){
				if(boardList_Tunnel[n] == boardList_Tunnel[m]){counter++; common_ones.push_back(m);}
			}
		}
		n += counter;
	}	
	//remove the common ones
	if(common_ones.size() >0){
		unsigned int idx =0;
		for(unsigned int k =0; k<common_ones.size();k++){ boardList_Tunnel.erase(boardList_Tunnel.begin()+common_ones[k]-idx);idx++;}
		common_ones.clear();
	}



	NBOARDS_TUNNEL = boardList_Tunnel.size();
	list.clear();
	if(NDETECTOR_TUNNEL == 1 || NDETECTOR_TUNNEL == 10 || NDETECTOR_TUNNEL == 100 || NDETECTOR_TUNNEL == 1000) NDETECTOR_TUNNEL = 1;
	else if(NDETECTOR_TUNNEL == 11 || NDETECTOR_TUNNEL == 1100 || NDETECTOR_TUNNEL == 110) NDETECTOR_TUNNEL = 2;
	else if(NDETECTOR_TUNNEL == 111 || NDETECTOR_TUNNEL == 1110) NDETECTOR_TUNNEL = 3;
	else if(NDETECTOR_TUNNEL == 1111) NDETECTOR_TUNNEL = 4;
	//---------------------------------------
	// Check if 2 board numbers are the same
	//---------------------------------------
	for(int i =0; i < boardList_DSSD.size(); i++){
		for(int j =0; j < boardList_DSSD.size(); j++){
			if(i != j && boardList_DSSD[i] == boardList_DSSD[j]){
				cout<<"Wrong DSSD Configuration. Check the Run.config file."<<endl;
				cout<<"Board: "<<boardList_DSSD[i]<<" has multiple association. "<<endl;
				exit(-1);
			}
		}
	}

	for(int i =0; i < boardList_Tunnel.size(); i++){
		for(int j =0; j < boardList_Tunnel.size(); j++){
			if(i != j && boardList_Tunnel[i] == boardList_Tunnel[j]){
				cout<<"Wrong Tunnel Configuration. Check the Run.config file."<<endl;
				cout<<"Board: "<<boardList_Tunnel[i]<<" has multiple association. "<<endl;
				exit(-1);
			}
		}
	}
	if(fverbose >1){
		cout<<"save TTree = "<<int(fsaveTree)<<endl;
		cout<<"save Histo = "<<int(fsaveHisto)<<endl;
		cout<<"verbose level = "<<fverbose<<endl;
		//cout<<"save spectra if killed = "<<save_spec_if_killed<<endl;
		cout<<"data merged = "<<int(data_merged)<<endl;
		cout<<"Buffer size = "<<buffer_size<<endl;
		cout<<"pre trig buffer = "<<pre_trig_buffer<<endl;
		cout<<"number of samples to be ignored at the start = "<<nStart_trace<<endl;
		cout<<"number of smapels to be ignored at the end  = "<<nEnd_trace<<endl;
		cout<<"trace size = "<<TRACE_SIZE<<endl;
		//Gain related
		cout<<"Back FPCSA Gain mode = "<<BACK_FPCSA_GAIN_MODE<<endl;
		cout<<"Back high Gain = "<<backHighGain<<endl;
		cout<<"Back low Gain = "<<backLowGain<<endl;
		cout<<"Front FPCSA Gain mode = "<<FRONT_FPCSA_GAIN_MODE<<endl;
		cout<<"Front high Gain = "<<frontHighGain<<endl;
		cout<<"Front low Gain = "<<frontLowGain<<endl;
		//gain switching
		cout<<"moving window gain switch calculation = "<< mv_window_calcul_gainSwitch<<endl;
		cout<<"front gain switch threhold in ADC ch = "<< front_gainSwitch_threshold<<endl;
		cout<<"back gain switch threhold in ADC ch = "<< back_gainSwitch_threshold<<endl;
		cout<<"Use Default FPCSA Gain = "<<int(use_default_FPCSA_Gain)<<endl;
		// HG
		cout<<"moving window high gain calculation = "<< mv_window_calcul_HG<<endl;
		cout<<"skip nSamples for high gain calculation = "<<skip_nSamples_calcul_HG<<endl;
		//LG
		cout<<"moving window low gain calculation = "<< mv_window_calcul_LG<<endl;
		cout<<"skip nSamples for low gain calculation = "<<skip_nSamples_calcul_LG<<endl;
		cout <<"Decay time threshold = "<< decayTime_threshold<<endl;
		cout<<"Saturation time threshold = "<<saturationTime_threshold<<endl;
		cout<<"risetime definition= "<<riseTime_def<<endl;
		cout<<"Trigger definition= "<<trigger_def<<endl;
		//Trapezoidal filter
		cout<<"Filter Algorithm = "<<filterAlgorithm<<endl;
		cout<<"use default filter parameters = "<<int(use_default_filter_parameters)<<endl;
		cout<<"default k value = "<<default_trap_k<<endl;
		cout<<"default m value = "<<default_trap_m<<endl;
		cout<<"Shaper Amplification Gain = "<<shaperAmplificationGain<<endl;
		cout<<" trapezoidal flattop measurement method = "<<trap_flattop_mode<<endl;
		cout<<"fractional height to Max of Trap height = " <<height_fraction_MaxTrapHeight<<endl;

		cout<<"Trapezoidal Parameter File = "<<trap_parameter_filename<<endl;
		cout<<"CFD Method = "<<CFDMethod<<endl;
		cout<<"DSSD CFD Fraction = "<<dssdCFDFraction<<endl;
		cout<<"DSSD CFD Delay = "<<dssdCFDDelay<<endl;
		cout<<"SED CFD Fraction = "<<sedCFDFraction<<endl;
		cout<<"SED CFD Delay = "<<sedCFDDelay<<endl;
		cout<<"Use Default CFD parameters = "<<int(use_default_CFD_parameters)<<endl;
		cout<<"DSSD CFD Parameter File = "<<cfd_parameter_filename_dssd<<endl;
		cout<<"SED CFD Parameter File = "<<cfd_parameter_filename_sed<<endl;
		cout<<"Total number of DSSD boards = "<<NBOARDS_DSSD<<endl;
		cout<<"DSSD board list = "; for(auto it : boardList_DSSD) cout<<it<<", ";cout <<endl;
		//Calibration files
		cout<<"DSSD high gain Calibration File = "<<dssd_HGcalib_filename<<endl;
		cout<<"DSSD low gain Calibration File = "<<dssd_LGcalib_filename<<endl;
		//Analysis
		cout<<"Sum Energies of the neighboring strips = "<<sum_neighboring_strips<<endl;
		cout<<"DSSD Front-Front Coincidence Window = "<<ff_window<<endl;
		cout<<"DSSD Front-Back Coincidence Window = "<<fb_window<<endl;
		cout<<"DSSD Back-Front Coincidence Window = "<<bf_window<<endl;
		cout<<"DSSD Back-Back Coincidence Window = "<<bb_window<<endl;
		cout<<"RATECALCULATIONTIMELAPSE = "<<rate_calcul_time_lapse<<endl;
		//-------------------
		//Tunnel
		//---------------
		cout<<"Number of tunnel detectors = "<<NDETECTOR_TUNNEL<<endl;
		cout<<"Total number of tunnel boards = "<<NBOARDS_TUNNEL<<endl;
		cout<<"Tunnel Board list = "; for(auto it : boardList_Tunnel) cout<< it <<", ";cout<<endl;
		cout<<"Tunnel calibration filename = "<<tunnel_calib_filename<<endl;
		//-------------------
		//EXOGAM
		//---------------------

	}

}
else{

	std::cerr<<" Error opening the configuration file... "<<std::endl;
	exit(-1);
}
}
void MyGlobal::PressEnterToContinue()
{
	int c;
	//        printf( "Press ENTER to continue... and f to end the program!!! " );
	cout<<GREEN<< "Press ENTER to continue... and f to end the program!!! "<< RESET<<endl; 
	fflush( stdin );
	do
	{
		c = getchar(); 
		if(c == 'f') exit(-1);
	}
	while ((c != '\n') && (c != EOF));
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
