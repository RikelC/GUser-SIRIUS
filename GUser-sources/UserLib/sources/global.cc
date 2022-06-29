#include "global.h"
myGlobal* myGlobal::instance = 0;
myGlobal* myGlobal::getInstance() {
	if(!instance) {
		instance = new myGlobal();
		return instance;
	}
	else {
		return instance;
	}
}
myGlobal::myGlobal(){
	// read the configuration file
	readRunConfig();
	//-initialize
	frameName[0]= "Cobo";
	frameName[1]= "Eby";
	frameName[2]= "Hello";
	frameName[3]= "xml header";
	frameName[4]= "rea trace";
	frameName[5]= "rea gen";
	frameName[6]= "sirius";
	frameName[7]= "else";

	fBoardList_DSSD = new int[NBOARDS_DSSD];
	fBoardList_Tunnel = new int[NBOARDS_TUNNEL];
	fConvertNoBoardIndexLocal_Tunnel = new int[200];
	fConvertNoBoardIndexLocal = new int [200];


	if(NBOARDS_DSSD != list_dssd.size()){
		cout<<"Check your Run.Config file..."<<endl;
		cout<<" Total number of DSSD boards does not match with your board list\n terminating the program!!!\n";
		exit(-1);
	}

	if(NBOARDS_TUNNEL != list_tunnel.size()){
		cout<<"Check your Run.Config file..."<<endl;
		cout<<" Total number of Tunnel boards does not match with your board list\n terminating the program!!!\n";
		exit(-1);
	}


	for(int i = 0; i < NBOARDS_DSSD; i++){
		fBoardList_DSSD[i] = list_dssd[i];
	}
	//---------
	for(int i = 0; i < NBOARDS_TUNNEL; i++){
		fBoardList_Tunnel[i] = list_tunnel[i];
	}


	list_dssd.clear();
	list_tunnel.clear();




	for(int board=0 ; board< NBOARDS_DSSD; board++){
		fConvertNoBoardIndexLocal[ fBoardList_DSSD[board] ] = board ;
	}
	//----------------------------
	for(int board=0 ; board< NBOARDS_TUNNEL; board++){
		fConvertNoBoardIndexLocal_Tunnel[fBoardList_Tunnel[board]] = board ;
	}

	NSTRIPS_DSSD = NBOARDS_DSSD * NCHANNELS;
}

myGlobal::~myGlobal(){
	cout<<"singleto destrucyor called."<<endl;
	delete [] fBoardList_DSSD;
	delete [] fBoardList_Tunnel;
	delete [] fConvertNoBoardIndexLocal_Tunnel;
	delete [] fConvertNoBoardIndexLocal;
	if(instance)delete instance; instance=NULL;
}


void myGlobal::readRunConfig(){

	ifstream file;
	file.open("ConfigFiles/Run.config", std::iostream::in);
	std::string line, variable, value; bool eqality_sign_found; 
	if(file.is_open()){


		while(file.good() && !file.eof()){
			//ignore white spaces
			//if first == # ignore line else read
			//
			//if(first == '#')file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
			eqality_sign_found = false;
			std::getline(file, line);
			variable.clear(); value.clear();
			for(size_t i = 0; i < line.length(); i++){
				if(line[i] == ' ' || line[i] == '\n') continue;
				else{
					if(line[i] == '#'){
						//is a comment
						break;
					}else{
						if(line[i] != '='){
							if(!eqality_sign_found)
								variable.append(1, line[i]);
							else
								value.append(1, line[i]);
						}else{
							eqality_sign_found = true;
						}
					}

				}
			}
			if(!variable.empty() && !value.empty()){

				//convert to upper case
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

				//verbose level
				if(variable.compare("VERBOSELEVEL")==0)
					fverbose = std::stoi(value);
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
				//total number of DSSD boards
				else if(variable.compare("TOTALNUMBEROFDSSDBOARDS")==0)
					NBOARDS_DSSD = std::stoi(value); 
				//----------------
				//DSSD board list
				//-----------------			
				else if(variable.compare("DSSDBOARDLIST")==0){

					while(value.size() > 3){
						size_t pos = value.find_first_of(",");
						std::string val = value.substr(0, pos);
						value.erase(value.begin(), value.begin()+pos+1);
						//cout<<"substr "<<val<<" value "<<value<<endl;
						list_dssd.push_back(std::stoi(val));
					}
					//treat the last elemnt
					list_dssd.push_back(std::stoi(value));
				}
				//----------------------
				//MB1
				///----------------------
				else if(variable.compare("MB1P4BOARD1")==0)
					MB1_P4_BOARD1 = std::stoi(value);
				else if(variable.compare("MB1P4BOARD2")==0)
					MB1_P4_BOARD2 = std::stoi(value);
				else if(variable.compare("MB1P5BOARD1")==0)
					MB1_P5_BOARD1 = std::stoi(value);
				else if(variable.compare("MB1P5BOARD2")==0)
					MB1_P5_BOARD2 = std::stoi(value);
				//-------------				
				//MB2
				//-----------
				else if(variable.compare("MB2P4BOARD1")==0)
					MB2_P4_BOARD1 = std::stoi(value);
				else if(variable.compare("MB2P4BOARD2")==0)
					MB2_P4_BOARD2 = std::stoi(value); 
				else if(variable.compare("MB2P5BOARD1")==0)
					MB2_P5_BOARD1 = std::stoi(value);
				else if(variable.compare("MB2P5BOARD2")==0)
					MB2_P5_BOARD2 = std::stoi(value);
				//----------				
				//MB3
				////--------
				else if(variable.compare("MB3P4BOARD1")==0)
					MB3_P4_BOARD1 = std::stoi(value);
				else if(variable.compare("MB3P4BOARD2")==0)
					MB3_P4_BOARD2 = std::stoi(value);
				else if(variable.compare("MB3P5BOARD1")==0)
					MB3_P5_BOARD1 = std::stoi(value);
				else if(variable.compare("MB3P5BOARD2")==0)
					MB3_P5_BOARD2 = std::stoi(value);
				//----------				
				//MB4
				//-----------
				else if(variable.compare("MB4P4BOARD1")==0)
					MB4_P4_BOARD1 = std::stoi(value);
				else if(variable.compare("MB4P4BOARD2")==0)
					MB4_P4_BOARD2 = std::stoi(value);
				else if(variable.compare("MB4P5BOARD1")==0)
					MB4_P5_BOARD1 = std::stoi(value);
				else if(variable.compare("MB4P5BOARD2")==0)
					MB4_P5_BOARD2 = std::stoi(value);

				//FPCSA gain
				else if(variable.compare("FPCSAGAIN")==0)
					FPCSA_GAIN = std::stod(value);
				//Shaper Amplification gain
				else if(variable.compare("SHAPERAMPLIFICATIONGAIN")==0)
					shaperAmplificationGain = std::stod(value);
				// DSSD calibration file				
				else if(variable.compare("DSSDCALIBRATIONFILE")==0)
					dssd_calib_filename = value;
				// Trapezoidal algorithm			
				else if(variable.compare("FILTERALGORITHM")==0)
					filterAlgorithm = value;	
				// default K value			
				else if(variable.compare("USEDEFAULTFILTERPARAMETERS")==0)
				{
					if(value.compare("yes") == 0 || value.compare("YES") == 0)
						useDefaultFilterParameters = true;
					else if(value.compare("no") == 0 || value.compare("NO") == 0)
						useDefaultFilterParameters = false;
				}
				// default K value			
				else if(variable.compare("DEFAULTKVALUE")==0)
					default_trap_k = std::stoi(value);
				// default M value		
				else if(variable.compare("DEFAULTMVALUE")==0)
					default_trap_m = std::stoi(value);
				//Trapezoidal parameter file		
				else if(variable.compare("TRAPEZOIDALPARAMETERFILE")==0)
					trap_parameter_filename = value;
				// sum energies of the neighboring strips			
				else if(variable.compare("SUMENERGIESOFTHENEIGHBORINGDSSDSTRIPS")==0){
					if(value.compare("yes") == 0 || value.compare("YES") == 0)
						sum_neighboring_strips = true;
					else if(value.compare("no") == 0 || value.compare("NO") == 0)
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
				//DSSD Front and back Gap in the histograms
				else if(variable.compare("DSSDFRONT-BACKBINGAP")==0)
					DSSD_FrontBackGap = std::stoi(value);

				//-------------
				//Tunnel
				//-----------
				//number of tunnel boards
				else if(variable.compare("TOTALNUMBEROFTUNNELBOARDS")==0)
					NBOARDS_TUNNEL = std::stoi(value);
				// tunnel board list
				//	
				else if(variable.compare("TUNNELBOARDLIST")==0){
					while(value.size() > 3){
						size_t pos = value.find_first_of(",");
						std::string val = value.substr(0, pos);
						value.erase(value.begin(), value.begin()+pos+1);
						//cout<<"substr "<<val<<" value "<<value<<endl;
						list_tunnel.push_back(std::stoi(val));
					}
					//treat the last elemnt 
					list_tunnel.push_back(std::stoi(value));
				}
				//tunnel calibration file
				else if(variable.compare("TUNNELCALIBRATIONFILE")==0)
					tunnel_calib_filename = value;

				//cout<<"variable "<<variable<<" value "<<value<<endl;
			}
		}
		file.close();


		if(fverbose >1){
			cout<<"verbose level = "<<fverbose<<endl;
			cout<<"pre trig buffer = "<<pre_trig_buffer<<endl;
			cout<<"number of samples to be ignored at the start = "<<nStart_trace<<endl;
			cout<<"number of smapels to be ignored at the end  = "<<nEnd_trace<<endl;
			cout<<"trace size = "<<TRACE_SIZE<<endl;
			cout<<"Total number of DSSD boards = "<<NBOARDS_DSSD<<endl;
			cout<<"DSSD board list = "; for(auto it : list_dssd) cout<<it<<", ";cout <<endl;
			cout<<"MB1 P4 BOARD1 = "<<MB1_P4_BOARD1<<endl;
			cout<<"MB1 P4 BOARD2 = "<<MB1_P4_BOARD2<<endl;
			cout<<"MB1 P5 BOARD1 = "<<MB1_P5_BOARD1<<endl;
			cout<<"MB1 P5 BOARD2 = "<<MB1_P5_BOARD2<<endl;

			cout<<"MB2 P4 BOARD1 = "<<MB2_P4_BOARD1<<endl;
			cout<<"MB2 P4 BOARD2 = "<<MB2_P4_BOARD2<<endl;
			cout<<"MB2 P5 BOARD1 = "<<MB2_P5_BOARD1<<endl;
			cout<<"MB2 P5 BOARD2 = "<<MB2_P5_BOARD2<<endl;

			cout<<"MB3 P4 BOARD1 = "<<MB3_P4_BOARD1<<endl;
			cout<<"MB3 P4 BOARD2 = "<<MB3_P4_BOARD2<<endl;
			cout<<"MB3 P5 BOARD1 = "<<MB3_P5_BOARD1<<endl;
			cout<<"MB3 P5 BOARD2 = "<<MB3_P5_BOARD2<<endl;

			cout<<"MB4 P4 BOARD1 = "<<MB4_P4_BOARD1<<endl;
			cout<<"MB4 P4 BOARD2 = "<<MB4_P4_BOARD2<<endl;
			cout<<"MB4 P5 BOARD1 = "<<MB4_P5_BOARD1<<endl;
			cout<<"MB4 P5 BOARD2 = "<<MB4_P5_BOARD2<<endl;

			cout<<"FPCSA Gain = "<<FPCSA_GAIN<<endl;
			cout<<"DSSD Calibration File = "<<dssd_calib_filename<<endl;
			cout<<"Filter Algorithm = "<<filterAlgorithm<<endl;
			cout<<"Shaper Amplification Gain = "<<shaperAmplificationGain<<endl;
			cout<<"use default filter parameters = "<<useDefaultFilterParameters<<endl;
			cout<<"default k value = "<<default_trap_k<<endl;
			cout<<"default m value = "<<default_trap_m<<endl;
			cout<<"Trapezoidal Parameter File = "<<trap_parameter_filename<<endl;
			cout<<"Sum Energies of the neighboring strips = "<<sum_neighboring_strips<<endl;
			cout<<"DSSD Front-Front Coincidence Window = "<<ff_window<<endl;
			cout<<"DSSD Front-Back Coincidence Window = "<<fb_window<<endl;
			cout<<"DSSD Back-Front Coincidence Window = "<<bf_window<<endl;
			cout<<"DSSD Back-Back Coincidence Window = "<<bb_window<<endl;
			cout<<"DSSD Front-Back Bin Gap = "<<DSSD_FrontBackGap<<endl;
			cout<<"Total number of tunnel boards = "<<NBOARDS_TUNNEL<<endl;
			cout<<"Tunnel Board list = "; for(auto it : list_tunnel) cout<< it <<", ";cout<<endl;
			cout<<"Tunnel calibration filename = "<<tunnel_calib_filename<<endl;

		}

	}
	else{

		std::cerr<<" Error opening the configuration file... "<<std::endl;
		exit(-1);
	}




}
