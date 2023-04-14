/*!
 *   \file     GruScript.C
 *   \brief    This Macro runs GRU in online or offline mode.
 *   \author   Rikel CHAKMA
 *   \date     23/07/2022.
 *   \details  To control the functionality of this program please read carefully the Run.config file located in the ConfigFiles directory.
 *   To execute this macro:
 *   --> at the system prompt: use GRU GruScript.C 
 *   --> at the GRU prompt: .x GruScript.C or 1) .L GruScript.C 2) GruScript()
 */          
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TROOT.h"
#include "TSystem.h"
#define RESET   "\033[1;0m"
#define BLACK   "\033[1;30m"
#define RED     "\033[1;31m" 
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
//----------------------------------------
// forward declaration of the functions
// -----------------------------------------
void GruScript(int mode);

void perform(int mode);

void Gru_Online(int &port, int &buffer_size, int &server_port);

void Gru_Offline(std::string **cmd, std::vector<std::string> filelist); 

void Gru_Offline(std::string **cmd);

void Gru_Offline(std::string **cmd, int &port, int &buffer_size, int &server_port); 

void to_upper(std::string & str);

std::vector<std::string> get_existing_files(std::string dir, std::vector<int> list);

std::vector<std::string> get_existing_files(std::string dir, std::vector<std::string> list); 

std::vector<std::string> get_to_be_processed_files(std::vector<string> list, std::vector<int> subRuns);

std::vector<int> extract_run_numbers_from_input(std::string run_numbers);

std::string *get_current_subrunRange(int l, std::vector<std::string> list);

std::string extract_run_number_from_filename(std::string filename);

std::string extract_subrun_number_from_filename( std::string filename);

std::string get_histo_filename(int l, std::string *h_sub_str, std::string output_dir, std::string runN, std::string subRunN, std::string sum_runs, std::string sum_subRuns,std::vector<std::string> runfiles, bool new_run_flag, int run_increament);

std::string get_tree_filename(int l, std::string *t_sub_str, std::string output_dir, std::string runN, std::string subRunN, std::string sum_runs, std::string sum_subRuns,std::vector<std::string> runfiles, bool new_run_flag, int run_increament);

std::string * extract_oFileFormats_substrings(std::string fileFormat);

bool is_a_new_run(int l, std::vector<std::string> list);

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//----------------------
// Main function
// --------------------
//! It is the Main function.
/*! Reset the instance of TROOT. A file named 'gruScript_pid.log' is created to store the process id. This file is then read to send SIGKILL or SIGINT signals to stop the process prematurely. Next, load the GRU environment and load and compile ./GUser_C.so. Note that the current GRU version uses ROOT 5.3XXX. It then calls the perform() function. Clean the TROOT environment before exiting the program.
*/
void GruScript(int mode){
	gROOT->Reset();
	/*	std::ofstream gru_pid;
		gru_pid.open("gruScript_pid.log");
		gru_pid << gSystem->GetPid()<<endl;
		gru_pid.close();
		*/
	char command[300];
	if (strncmp(gROOT->GetVersion(), "5.3",3)==0){
		printf ("version de root 5.3XXXX\n");
		TString test = gSystem->Getenv("GRUDIR");
		if (test.CompareTo("")==0) sprintf(command ,".include /home/acqexp/GRU/GRUcurrent/include");
		else sprintf(command ,".include %s/include",test.Data());
		gROOT->ProcessLine(".L ./GUser_C.so");//load and compile TUiser class
	}
	perform(mode);
	gROOT->Reset();
	//gROOT->ProcessLine(".q");

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//----------------------------------------
// function to perform main task 
//----------------------------------------
//! It is the sub main function.
/*! The values of the variables acqisition mode (online, offline, offline+vigru, offline_filelist), input directory where the raw binary data is stored, output directory, where TTree files and histogram files to be stored, run numbers, sub run numbers, list of files and number of events in each file to process in the offline mode, the file formats of the TTree and histogram files, net port number, net buffersize, root server port number for online and offline+vigru modes, and options such as save ttree,  sum sub Runs, sum Runs in to one file, save histograms from the Run.config file. In order to make case insensitive the variable names and the values (string) are converted to upper case. Depending on the chosen acquisition mode Gru_Online(int &port, int &buffer_size, int &server_port), Gru_Offline(std::string **cmd, int &port, int &buffer_size, int &server_port), Gru_Offline(std::string **cmd), Gru_Offline(std::string **cmd, std::vector<std::string> filelist) is performed.
*/
void perform(int mode){

	std::ifstream configFile; 
	std::string line,test_line, variable, value, acq_mode, input_dir, output_dir, run_numbers, subrun_numbers, number_of_events_to_process; 
	std::string treeFileFormat;
	std::string histFileFormat;
	int net_port_number, net_buffer_size, root_server_port_number;
	std::string save_ttree, sum_subRuns, sum_runs, save_histo, data_merged;
	bool equality_sign_found = 0;
	std::vector<std::string> filelist; 
	std::string **p_variable = new std::string*[20];

	p_variable[0] = &run_numbers;
	p_variable[1] = &subrun_numbers;
	p_variable[2] = &input_dir;
	p_variable[3] = &output_dir;
	p_variable[4] = &save_ttree;
	p_variable[5] = &treeFileFormat;
	p_variable[6] = &sum_subRuns;
	p_variable[7] = &save_histo;
	p_variable[8] = &histFileFormat;
	p_variable[9] = &number_of_events_to_process;
	p_variable[10] = &sum_runs;

	if(mode==1)configFile.open("ConfigFiles/Run_online.config", std::ifstream::in);
	else if(mode==2)configFile.open("ConfigFiles/Run_offline.config", std::ifstream::in);
	else configFile.open("ConfigFiles/Run.config", std::ifstream::in);
	if(configFile.is_open()){
		while(configFile.good() && !configFile.eof()){
			getline(configFile, line);
			//skip empty line with white spaces			
			std::istringstream str_stream(line);
			str_stream >> test_line;
			if(!str_stream) continue;
			str_stream.unget();

			equality_sign_found = 0;
			variable.clear(); value.clear();
			for(size_t i = 0; i< line.length();i++){
				if(line[i] == '#') break;
				if(line[i] == ' ' || line[i] == '\n') continue;
				else{
					if(line[i] != '='){
						if(!equality_sign_found) variable.append(1, line[i]);
						else value.append(1, line[i]);
					}else{
						equality_sign_found = 1;
					}
				}
			}

			to_upper(variable);
			if(variable.find("ENDOFGENERALSETTINGS")!=std::string::npos) break;

			size_t pos_comment = value.find("//");
			if(pos_comment != std::string::npos) value.erase(pos_comment, value.length());

			if(!variable.empty() && !value.empty()){
				if(variable.compare("ACQUISITIONMODE")==0) acq_mode = value;
				else if(variable.compare("NETPORTNUMBER")==0) net_port_number = atoi(value.c_str());
				else if(variable.compare("NETBUFFERSIZE")==0) net_buffer_size = atoi(value.c_str());
				else if(variable.compare("ROOTSERVERPORTNUMBER")==0) root_server_port_number = atoi(value.c_str());
				else if(variable.compare("RUNNUMBER")==0 || variable.compare("RUNNUMBERS")==0 ) run_numbers = value;
				else if(variable.compare("SUBRUNNUMBER")==0 || variable.compare("SUBRUNNUMBERS")==0 ) subrun_numbers = value;
				else if(variable.compare("INPUTFILEPATH")==0) input_dir = value;
				else if(variable.compare("OUTPUTFILEPATH")==0) output_dir = value;
				else if(variable.compare("SAVETTREEFILE")==0){to_upper(value);  save_ttree = value;;}
				else if(variable.compare("TTREEFILENAMEFORMAT")==0) treeFileFormat = value;
				else if(variable.compare("SUMRUNS")==0){to_upper(value); sum_runs = value;}
				else if(variable.compare("SUMSUBRUNS")==0){to_upper(value); sum_subRuns = value;}
				else if(variable.compare("SAVEHISTOGRAMFILE")==0){ to_upper(value); save_histo = value;}
				else if(variable.compare("HISTOGRAMFILENAMEFORMAT")==0) histFileFormat = value;
				else if(variable.compare("PROCESSNUMBEROFEVENTS")==0){to_upper(value); number_of_events_to_process = value;}
				else if(variable.compare("FILE")==0){filelist.push_back(value);}
			}

		}
		configFile.close();

		std::cout<<"*****************************************************************"<<std::endl;
		std::cout<<"Acquisition mode = "<<acq_mode<<std::endl;
		std::cout<<"Net Port number = "<<net_port_number<<std::endl;
		std::cout<<"Net Buffer size = "<<net_buffer_size<<std::endl;
		std::cout<<"Root Server Port number = "<<root_server_port_number<<std::endl;
		std::cout<<"Run numbers = "<<run_numbers<<std::endl;
		std::cout<<"Sub Run numbers = "<<subrun_numbers<<std::endl;
		std::cout<<"Input file path = "<<input_dir<<std::endl;
		std::cout<<"Output file path = "<<output_dir<<std::endl;
		std::cout<<"Save TTree file = "<<save_ttree<<std::endl;
		std::cout<<"TTree file format = "<<treeFileFormat<<std::endl;
		std::cout<<"Sum Runs = "<<sum_runs<<std::endl;
		std::cout<<"Sum subRuns = "<<sum_subRuns<<std::endl;
		std::cout<<"Save histogram file = "<<save_histo<<std::endl;
		std::cout<<"Histogram file format = "<<histFileFormat<<std::endl;
		std::cout<<"Number of events to process = "<<number_of_events_to_process<<std::endl;
		std::cout<<"*****************************************************************"<<std::endl;

		to_upper(acq_mode);
		std::ofstream gru_pid;
		if(acq_mode=="ONLINE"){
			gru_pid.open("gruScript_pid_online.log");
			gru_pid << gSystem->GetPid()<<endl;
			Gru_Online(net_port_number, net_buffer_size, root_server_port_number);
		}
		else if(acq_mode=="OFFLINE+VIGRU"){
			gru_pid.open("gruScript_pid_offline.log");
			gru_pid << gSystem->GetPid()<<endl;
			Gru_Offline(p_variable,net_port_number, net_buffer_size, root_server_port_number);


		}
		else if(acq_mode=="OFFLINE"){
			gru_pid.open("gruScript_pid_offline.log");
			gru_pid << gSystem->GetPid()<<endl;
			Gru_Offline(p_variable);
		}
		else if(acq_mode=="OFFLINE_FILELIST"){
			gru_pid.open("gruScript_pid_offline.log");
			gru_pid << gSystem->GetPid()<<endl;
			Gru_Offline(p_variable, filelist);
		}
	}else
		std::cout<<"Run.config file not found !!!!!"<<std::endl;
	gru_pid.close();
	delete [] p_variable;
	filelist.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//---------------
// For Online
// --------------
//! To perform online analysis.
/*! In the Run.config file, set Acquisition Mode = online. The parameters of this function are also specified in this file.
 * @param port Net port number (Usually the Watcher port number. Check in the Run Control.).
 * @param buffer_size Net buffer size.
 * @param server_port root server port number for Vigru.
 */
void Gru_Online(int &port, int &buffer_size, int &server_port){
	gROOT->Reset();
	GNetClientNarval  *net = new GNetClientNarval("localhost"); 
	net->SetPort (port);
	net->SetBufferSize(buffer_size);
	GUser * a= new GUser(1, net);          // creat user treatement environement
	GNetServerRoot * serv = new GNetServerRoot(server_port, a);
	a->EventInit("dssd_acquisition","mfm");                      // event initialisation
	a->SetSpectraMode(1);                // Declare all raw parameters as histograms
	a->InitUser();
	serv->StartServer();
	a->DoRun();                          // a->DoRun(2000);do treaments on 2000 first events ( 0 = all);
	a->EndUser();
	a->SpeSave("histo.root"); // save all declared histogram
	delete (a);   // finish
	a = NULL;
	delete serv;
	serv = NULL;
	net->Close();
	delete net;
	net = NULL;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//------------------------
// For Offline filelist
// -----------------------
//! To perform offline analysis of certain list of files.
/*! In the Run.config file, set Acquisition Mode = offline_filelist. The parameters of this function are also specified in this file.
 * @param cmd list of commands for offline analysisi.
 * @param runfiles list of files specified in Run.config with file = file name. 
 */
void Gru_Offline(std::string **cmd, std::vector<std::string> runfiles){  
	gROOT->Reset();
	std::string run_numbers                   =   *cmd[0];
	std::string subrun_numbers                =   *cmd[1];
	std::string input_dir                     =   *cmd[2];
	std::string output_dir                    =   *cmd[3];
	std::string save_ttree                    =   *cmd[4];
	std::string treeFileFormat                =   *cmd[5];
	std::string sum_subRuns                   =   *cmd[6];
	std::string save_histo                    =   *cmd[7];
	std::string histFileFormat                =   *cmd[8];
	std::string number_of_events_to_process   =   *cmd[9];
	std::string sum_runs                      =   *cmd[10];
	//remove trailling '/'
	if(input_dir[input_dir.length()-1]=='/'){

		input_dir.resize(input_dir.length()-1);
	}
	if(output_dir[output_dir.length()-1]=='/'){

		output_dir.resize(output_dir.length()-1);
	}
	// variables for output file names
	std::string treeFileName;
	std::string prev_treeFileName;
	std::string histFileName;
	std::string prev_histFileName;
	std::string *t_sub_str = extract_oFileFormats_substrings(treeFileFormat);
	std::string *h_sub_str = extract_oFileFormats_substrings(histFileFormat);
	GUser *a;
	GMFMFile *file;
	int current_runN =0, prev_runN =0, current_subrunN =0, prev_subrunN =0, run_increament =0;
	//----------------------
	// check if the files are duplicate
	// -----------------------
	//Check for double value if the user had made a mistake in defining the run numbers
	std::vector<int>common_runs;
	for(unsigned int n =0; n<runfiles.size();n++){	
		unsigned int counter =0;
		for(unsigned int m =n; m<runfiles.size();m++){
			if(n != m){
				if(runfiles[n] == runfiles[m]){counter++; common_runs.push_back(m);std::cout<<"m "<<m<<std::endl;}
			}
		}
		n += counter;
	}	
	//remove the common runs
	if(common_runs.size() >0){
		unsigned int idx =0;
		for(unsigned int k =0; k<common_runs.size();k++){ runfiles.erase(runfiles.begin()+common_runs[k]-idx);idx++;}
		common_runs.clear();
	}
	//-------------------------------------
	// check files exist in the input dir
	//-------------------------------------
	runfiles = get_existing_files(input_dir, runfiles);
	if(runfiles.size() ==0){
		cout<<"No files defined in the Run.Config exist in the input directory : "<<input_dir<<endl;
	}
	else{
		run_numbers.clear();
		for(unsigned int i =0; i < runfiles.size();i++){
			//cout<<"i "<<i<<"  file: "<<runfiles[i]<<endl;
			std::string runN = extract_run_number_from_filename(runfiles[i]);
			if(i==0) run_numbers = runN;
			else run_numbers += "," +runN;
		}
		//---------------
		// do file loop
		//-------------
		for(unsigned int l =0; l < runfiles.size(); l++){
			if(runfiles[l].empty())continue;
			std::string current_file = input_dir + "/" + runfiles[l];
			cout<<"file to be treated "<<current_file<<endl;
			if(sum_runs == "YES" && runfiles.size() > 1){
				if(l ==0){
					if(save_ttree == "YES")
						treeFileName = get_tree_filename(l, t_sub_str, output_dir, run_numbers, "all", sum_runs, sum_subRuns, runfiles, true, run_increament);
					if(save_histo =="YES")		
						histFileName = get_histo_filename( l,  h_sub_str,  output_dir, run_numbers, "all", sum_runs, sum_subRuns, runfiles, true, run_increament);

					file = new GMFMFile(current_file.data());   
					file->Open();                          // Open Device
					file->Rewind();        // rewind run								
					a= new GUser(2, file);            // creat user treatement environement
					a->g_instance = a;
					a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
					if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
					//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
					if(save_ttree == "YES")a->InitUserTTree(treeFileName.c_str());
					a->InitUser();     // Do Init User()
				}else{
					file->SetDevice(current_file.data()); // change of run
					file->Open();                          // Open Device
					file->Rewind();        // rewind run								
					a->SetDevIn(file);
					a->g_instance = a;
				}
				if(number_of_events_to_process == "ALL")
					a->DoRun();
				else{
					int  nEvents = atoi(number_of_events_to_process.c_str());
					a->DoRun(nEvents);
				}

				cout<<"Current TTree file name: "<<treeFileName<<endl;
				cout<<"Current Histo file name: "<<histFileName<<endl;
				file->Close();
			}// runs summed
			else{//runs not summed
				std::string runN = extract_run_number_from_filename(runfiles[l]);
				std::string subRunN = extract_subrun_number_from_filename(runfiles[l]);
				current_runN = atoi(runN.c_str());
				current_subrunN = atoi(subRunN.c_str());
				if(l ==0){
					if(save_ttree == "YES")
						treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
					if(save_histo =="YES")		
						histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
				}
				else{
					if(current_runN == prev_runN){// have the same run number
						if(current_subrunN = prev_subrunN){
							run_increament++;
							if(save_ttree == "YES")
								treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
							if(save_histo =="YES")
								histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
						}
						else{
							if(is_a_new_run(l, runfiles)){
								run_increament++;
								if(save_ttree == "YES")
									treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
								if(save_histo =="YES")
									histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
							}else{
								if(sum_subRuns == "NO"){							
									if(save_ttree == "YES")
										treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, false, run_increament);
									if(save_histo =="YES")
										histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, false, run_increament);
								}
							}
						}
					}
					else{//new run
						run_increament =0;
						if(save_ttree == "YES")
							treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
						if(save_histo =="YES")
							histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
					}
				}
				//-----------------
				//  Gru part
				//  ---------------
				if(l ==0){
					file = new GMFMFile(current_file.data());   
					file->Open();                          // Open Device
					file->Rewind();        // rewind run								
					a= new GUser(2, file);            // creat user treatement environement
					a->g_instance = a;
					a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
					if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
					//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
					if(save_ttree == "YES")a->InitUserTTree(treeFileName.c_str());
					a->InitUser();     // Do Init User()
				}
				else{
					if(current_runN == prev_runN){// have the same run number
						//first check if they are different runs with same run number
						if(current_subrunN = prev_subrunN){
							//--------------
							// save old file
							//--------------				
							a->EndUser(); 
							if(save_ttree == "YES")	a->SaveUserTTree();    
							if(save_histo == "YES") a->SpeSave(prev_histFileName.c_str());
							delete (a);   
							a = NULL;
							delete file;
							file = NULL;
							//------------------
							// open a new file
							//-------------------
							file = new GMFMFile(current_file.data());   
							file->Open();                          // Open Device
							file->Rewind();        // rewind run								
							a= new GUser(2, file);            // creat user treatement environement
							a->g_instance = a;
							a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
							if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
							//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
							if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
							a->InitUser();     // Do Init User()
						}
						else{//current file with diferent sub run no
							if(is_a_new_run(l, runfiles)){
								//--------------
								// save old file
								//--------------				
								a->EndUser();  
								if(save_ttree == "YES")	a->SaveUserTTree();   
								if(save_histo == "YES") a->SpeSave(prev_histFileName.c_str());
								delete (a);   
								a = NULL;
								delete file;
								file = NULL;
								//------------------
								// open a new file
								//-------------------
								file = new GMFMFile(current_file.data());   
								file->Open();                          // Open Device
								file->Rewind();        // rewind run								
								a= new GUser(2, file);            // creat user treatement environement
								a->g_instance = a;
								a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
								if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
								//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
								if(save_ttree == "YES")a->InitUserTTree(treeFileName.c_str());
								a->InitUser();     // Do Init User()
							}
							else{
								if(sum_subRuns == "NO"){
									//---------------
									// save old file
									//----------------
									a->EndUserRun();
									a->EndUser(); 
									if(save_ttree == "YES")	a->SaveUserTTree();
									if(save_histo == "YES") a->SpeSave(prev_histFileName.c_str());

									delete (a);  
									a = NULL;
									delete file;
									file = NULL;
									//--------------
									// open new file
									//--------------
									file = new GMFMFile(current_file.data());   
									file->Open();                          // Open Device
									file->Rewind();        // rewind run								
									a= new GUser(2, file);            // creat user treatement environement
									a->g_instance = a;
									a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
									if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
									//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
									if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
									a->InitUser(); 
								}else {
									file->SetDevice(current_file.data()); // change of run
									file->Open();                          // Open Device
									file->Rewind();        // rewind run								
									a->SetDevIn(file);
									a->g_instance = a;
								}
							}
						}
					}
					else{//new run
						//--------------
						// save old file
						//--------------				
						if(sum_subRuns == "YES"){
							a->EndUser();  
							if(save_ttree == "YES")	a->SaveUserTTree();   
							if(save_histo == "YES")	a->SpeSave(prev_histFileName.c_str());
							delete (a);   
							a = NULL;
							delete file;
							file = NULL;
						}
						//----------------
						// open a new file
						//----------------
						file = new GMFMFile(current_file.data());   
						file->Open();                          // Open Device
						file->Rewind();        // rewind run								
						a= new GUser(2, file);            // creat user treatement environement
						a->g_instance = a;
						a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
						if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
						//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
						if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
						a->InitUser();     // Do Init User()
					}
				}
				if(number_of_events_to_process == "ALL")
					a->DoRun();
				else{
					int  nEvents = atoi(number_of_events_to_process.c_str());
					a->DoRun(nEvents);
				}
				cout<<"Current TTree file name: "<<treeFileName<<endl;
				cout<<"Current Histo file name: "<<histFileName<<endl;
				file->Close();
				prev_runN = current_runN;
				prev_subrunN = prev_subrunN;
				prev_treeFileName = treeFileName;
				prev_histFileName = histFileName;
			}
		}//file loop
		//--------------
		// save last file
		//--------------				
		a->EndUser();     
		if(save_ttree == "YES")	a->SaveUserTTree();
		if(save_histo == "YES") a->SpeSave(histFileName.c_str());
		delete (a);   
		a = NULL;
		delete file;
		file = NULL;
		runfiles.clear();
		delete[] t_sub_str;
		delete[] h_sub_str;
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//------------------------
// For Offline
// -----------------------
//! To perform offline analysis.
/*! In the Run.config file, set Acquisition Mode = offline. The parameter of this function contains the list of commands specified in this file.
 * @param cmd list of commands for ofline analysis.
 */
void Gru_Offline(std::string **cmd){  
	gROOT->Reset();
	std::string run_numbers                   =   *cmd[0];
	std::string subrun_numbers                =   *cmd[1];
	std::string input_dir                     =   *cmd[2];
	std::string output_dir                    =   *cmd[3];
	std::string save_ttree                    =   *cmd[4];
	std::string treeFileFormat                =   *cmd[5];
	std::string sum_subRuns                   =   *cmd[6];
	std::string save_histo                    =   *cmd[7];
	std::string histFileFormat                =   *cmd[8];
	std::string number_of_events_to_process   =   *cmd[9];
	std::string sum_runs                      =   *cmd[10];
	//remove trailling '/'
	if(input_dir[input_dir.length()-1]=='/'){

		input_dir.resize(input_dir.length()-1);
	}
	if(output_dir[output_dir.length()-1]=='/'){

		output_dir.resize(output_dir.length()-1);
	}
	// Extract run numbers
	std::vector<int> Runs= extract_run_numbers_from_input(run_numbers);
	//Check for double value if the user had made a mistake in defining the run numbers
	std::vector<int>common_runs;
	for(unsigned int n =0; n<Runs.size();n++){	
		unsigned int counter =0;
		for(unsigned int m =n; m<Runs.size();m++){
			if(n != m){
				if(Runs[n] == Runs[m]){counter++; common_runs.push_back(m);std::cout<<"m "<<m<<std::endl;}
			}
		}
		n += counter;
	}	
	//remove the common runs
	if(common_runs.size() >0){
		unsigned int idx =0;
		for(unsigned int k =0; k<common_runs.size();k++){ Runs.erase(Runs.begin()+common_runs[k]-idx);idx++;}
		common_runs.clear();
	}
	//Extract sub run numbers
	std::vector<int> subRuns = extract_run_numbers_from_input(subrun_numbers);
	//Look for files if they exist in the input directory
	vector<std::string> runfiles_temp =  get_existing_files(input_dir, Runs);
	vector<std::string> runfiles =  get_to_be_processed_files(runfiles_temp, subRuns);
	runfiles_temp.clear();
	Runs.clear();
	subRuns.clear();

	// variables for output file names
	std::string treeFileName;
	std::string prev_treeFileName;
	std::string histFileName;
	std::string prev_histFileName;
	std::string *t_sub_str = extract_oFileFormats_substrings(treeFileFormat);
	std::string *h_sub_str = extract_oFileFormats_substrings(histFileFormat);
	GUser *a;
	GMFMFile *file;
	int current_runN =0, prev_runN =0, current_subrunN =0, prev_subrunN =0, run_increament =0;
	//---------------
	// do file loop
	//-------------
	for(unsigned int l =0; l < runfiles.size(); l++){
		if(runfiles[l].empty())continue;
		std::string current_file = input_dir + "/" + runfiles[l];
		cout<<"file to be treated "<<current_file<<endl;
		if(sum_runs == "YES" && runfiles.size() > 1){
			if(l ==0){
				if(save_ttree == "YES")
					treeFileName = get_tree_filename(l, t_sub_str, output_dir, run_numbers, "all", sum_runs, sum_subRuns, runfiles, true, run_increament);
				if(save_histo =="YES")		
					histFileName = get_histo_filename( l,  h_sub_str,  output_dir, run_numbers, "all", sum_runs, sum_subRuns, runfiles, true, run_increament);


				file = new GMFMFile(current_file.data());   
				file->Open();                          // Open Device
				file->Rewind();        // rewind run								
				a= new GUser(2, file);            // creat user treatement environement
				a->g_instance = a;
				a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
				if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
				//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
				if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
				a->InitUser();     // Do Init User()

			}else{
				file->SetDevice(current_file.data()); // change of run
				file->Open();                          // Open Device
				file->Rewind();        // rewind run								
				a->SetDevIn(file);
				a->g_instance = a;
			}

			if(number_of_events_to_process == "ALL")
				a->DoRun();
			else{
				int  nEvents = atoi(number_of_events_to_process.c_str());
				a->DoRun(nEvents);
			}

			cout<<"Current TTree file name: "<<treeFileName<<endl;
			cout<<"Current Histo file name: "<<histFileName<<endl;
			file->Close();
		}// runs summed
		else{//runs not summed
			std::string runN = extract_run_number_from_filename(runfiles[l]);
			std::string subRunN = extract_subrun_number_from_filename(runfiles[l]);
			current_runN = atoi(runN.c_str());
			current_subrunN = atoi(subRunN.c_str());
			if(l ==0){
				if(save_ttree == "YES")
					treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
				if(save_histo =="YES")		
					histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
			}
			else{
				if(current_runN == prev_runN){// have the same run number
					//first check if they are different runs with same run number
					if(current_subrunN == prev_subrunN){
						run_increament++;
						if(save_ttree == "YES")
							treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
						if(save_histo =="YES")
							histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
					}
					else{
						if(is_a_new_run(l, runfiles)){
							run_increament++;
							if(save_ttree == "YES")
								treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
							if(save_histo =="YES")
								histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
						}else{
							if(sum_subRuns == "NO"){
								if(save_ttree == "YES")
									treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, false, run_increament);
								if(save_histo =="YES")
									histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, false, run_increament);
							}

						}
					}
				}
				else{//new run
					run_increament =0;
					if(save_ttree == "YES")
						treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
					if(save_histo =="YES")
						histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
				}
			}
			//-----------------
			//  Gru part
			//  ---------------
			if(l ==0){
				file = new GMFMFile(current_file.data());   
				file->Open();                          // Open Device
				file->Rewind();        // rewind run								
				a= new GUser(2, file);            // creat user treatement environement
				a->g_instance = a;
				a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
				if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
				//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
				if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
				a->InitUser();     // Do Init User()
			}
			else{
				if(current_runN == prev_runN){// have the same run number
					//first check if they are different runs with same run number
					if(current_subrunN = prev_subrunN){
						//--------------
						// save old file
						//--------------				
						a->EndUser();  
						if(save_ttree == "YES")	a->SaveUserTTree();   
						if(save_histo == "YES")	a->SpeSave(prev_histFileName.c_str());
						delete (a);   
						a = NULL;
						delete file;
						file = NULL;
						//-----------------
						// open a new file
						//-----------------
						file = new GMFMFile(current_file.data());   
						file->Open();                          // Open Device
						file->Rewind();        // rewind run								
						a= new GUser(2, file);            // creat user treatement environement
						a->g_instance = a;
						a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
						if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
						//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
						if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
						a->InitUser();     // Do Init User()
					}
					else{//current file with diferent sub run no
						if(is_a_new_run(l, runfiles)){
							//--------------
							// save old file
							//--------------				
							a->EndUser();   
							if(save_ttree == "YES")	a->SaveUserTTree();  
							if(save_histo == "YES")	a->SpeSave(prev_histFileName.c_str());
							delete (a);   
							a = NULL;
							delete file;
							file = NULL;
							//-----------------
							// open a new file
							//-----------------
							file = new GMFMFile(current_file.data());   
							file->Open();                          // Open Device
							file->Rewind();        // rewind run								
							a= new GUser(2, file);            // creat user treatement environement
							a->g_instance = a;
							a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
							if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
							//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
							if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
							a->InitUser();     // Do Init User()
						}else{
							if(sum_subRuns == "NO"){
								//--------------
								//save old file
								//---------------
								a->EndUserRun();
								a->EndUser(); 
								if(save_ttree == "YES")	a->SaveUserTTree();
								if(save_histo == "YES") a->SpeSave(prev_histFileName.c_str());

								delete (a);  
								a = NULL;
								delete file;
								file = NULL;
								//------------------
								// open a new fiel
								//-------------------
								file = new GMFMFile(current_file.data());   
								file->Open();                          // Open Device
								file->Rewind();        // rewind run								
								a= new GUser(2, file);            // creat user treatement environement
								a->g_instance = a;
								a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
								if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
								//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
								if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
								a->InitUser(); 
							}else {
								file->SetDevice(current_file.data()); // change of run
								file->Open();                          // Open Device
								file->Rewind();        // rewind run								
								a->SetDevIn(file);
								a->g_instance = a;
							}
						}
					}
				}
				else{//new run
					//--------------
					// save old file
					//--------------				
					if(sum_subRuns == "YES"){
						a->EndUser();  
						if(save_ttree == "YES")	a->SaveUserTTree();   
						if(save_histo == "YES")	a->SpeSave(prev_histFileName.c_str());
						delete (a);   
						a = NULL;
						delete file;
						file = NULL;
					}
					file = new GMFMFile(current_file.data());   
					file->Open();                          // Open Device
					file->Rewind();        // rewind run								
					a= new GUser(2, file);            // creat user treatement environement
					a->g_instance = a;
					a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
					if(save_histo =="YES")a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
					//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
					if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
					a->InitUser();     // Do Init User()
				}
			}
			if(number_of_events_to_process == "ALL")
				a->DoRun();
			else{
				int  nEvents = atoi(number_of_events_to_process.c_str());
				a->DoRun(nEvents);
			}
			cout<<"Current TTree file name: "<<treeFileName<<endl;
			cout<<"Current Histo file name: "<<histFileName<<endl;
			file->Close();

			prev_runN = current_runN;
			prev_subrunN = prev_subrunN;
			prev_treeFileName = treeFileName;
			prev_histFileName = histFileName;
		}
	}//file loop
	//--------------
	// save last file
	//--------------				
	a->EndUser();     
	if(save_ttree == "YES")	a->SaveUserTTree();
	if(save_histo == "YES") a->SpeSave(histFileName.c_str());
	delete (a);   
	a = NULL;
	delete file;
	file = NULL;
	runfiles.clear();
	delete[] t_sub_str;
	delete[] h_sub_str;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//------------------------
// For Offline with vigru
// -----------------------
//! To perform offline analysis with Vigru enabled.
/*! In the Run.config file, set Acquisition Mode = offline+vigru. The parameters of this function are also specified in this file.
 * @param cmd list of commands for offline analysis.
 * @param port Net port number (useful for online)
 * @param buffer_size Net buffer size. 
 * @param server_port root server port number for Vigru.
 */
void Gru_Offline(std::string **cmd,int &port, int &buffer_size, int &server_port){  
	GNetClientNarval  *net = new GNetClientNarval("localhost"); //
	net->SetPort (port);
	net->SetBufferSize(buffer_size);

	std::string run_numbers                   =   *cmd[0];
	std::string subrun_numbers                =   *cmd[1];
	std::string input_dir                     =   *cmd[2];
	std::string output_dir                    =   *cmd[3];
	std::string save_ttree                    =   *cmd[4];
	std::string treeFileFormat                =   *cmd[5];
	std::string sum_subRuns                   =   *cmd[6];
	std::string save_histo                    =   *cmd[7];
	std::string histFileFormat                =   *cmd[8];
	std::string number_of_events_to_process   =   *cmd[9];
	std::string sum_runs                      =   *cmd[10];
	//remove trailling '/'
	if(input_dir[input_dir.length()-1]=='/'){

		input_dir.resize(input_dir.length()-1);
	}
	if(output_dir[output_dir.length()-1]=='/'){

		output_dir.resize(output_dir.length()-1);
	}
	// Extract run numbers
	std::vector<int> Runs= extract_run_numbers_from_input(run_numbers);
	//Check for double value if the user had made a mistake in defining the run numbers
	std::vector<int>common_runs;
	for(unsigned int n =0; n<Runs.size();n++){	
		unsigned int counter =0;
		for(unsigned int m =n; m<Runs.size();m++){
			if(n != m){

				if(Runs[n] == Runs[m]){counter++; common_runs.push_back(m);std::cout<<"m "<<m<<std::endl;}
			}
		}
		n += counter;
	}	
	//remove the common runs
	if(common_runs.size() >0){
		unsigned int idx =0;
		for(unsigned int k =0; k<common_runs.size();k++){ Runs.erase(Runs.begin()+common_runs[k]-idx);idx++;}
		common_runs.clear();
	}
	//Extract sub run numbers
	std::vector<int> subRuns = extract_run_numbers_from_input(subrun_numbers);
	//Look for files if they exist in the input directory
	vector<std::string> runfiles_temp =  get_existing_files(input_dir, Runs);
	vector<std::string> runfiles =  get_to_be_processed_files(runfiles_temp, subRuns);
	runfiles_temp.clear();
	Runs.clear();
	subRuns.clear();

	// variables for output file names
	std::string treeFileName;
	std::string prev_treeFileName;
	std::string histFileName;
	std::string prev_histFileName;
	std::string *t_sub_str = extract_oFileFormats_substrings(treeFileFormat);
	std::string *h_sub_str = extract_oFileFormats_substrings(histFileFormat);
	GUser *a;
	GMFMFile *file;
	GNetServerRoot * serv;
	int current_runN =0, prev_runN =0, current_subrunN =0, prev_subrunN =0, run_increament =0;
	//---------------
	// do file loop
	//-------------
	for(unsigned int l =0; l < runfiles.size(); l++){
		if(runfiles[l].empty())continue;
		std::string current_file = input_dir + "/" + runfiles[l];
		cout<<"file to be treated "<<current_file<<endl;
		if(sum_runs == "YES" && runfiles.size() > 1){
			if(l ==0){
				if(save_ttree == "YES")
					treeFileName = get_tree_filename(l, t_sub_str, output_dir, run_numbers, "all", sum_runs, sum_subRuns, runfiles, true, run_increament);
				if(save_histo =="YES")		
					histFileName = get_histo_filename( l,  h_sub_str,  output_dir, run_numbers, "all", sum_runs, sum_subRuns, runfiles, true, run_increament);


				file = new GMFMFile(current_file.data());   
				file->Open();                          // Open Device
				file->Rewind();        // rewind run								
				a= new GUser(2, file);            // creat user treatement environement
				a->g_instance = a;
				a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
				a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
				//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
				if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
				a->InitUser();     // Do Init User()
				serv = new GNetServerRoot(server_port, a);
				serv->StartServer();
			}else{
				file->SetDevice(current_file.data()); // change of run
				file->Open();                          // Open Device
				file->Rewind();        // rewind run								
				a->SetDevIn(file);
				a->g_instance = a;
				serv->SetAcq(a);
			}

			if(number_of_events_to_process == "ALL")
				a->DoRun();
			else{
				int  nEvents = atoi(number_of_events_to_process.c_str());
				a->DoRun(nEvents);
			}

			cout<<"Current TTree file name: "<<treeFileName<<endl;
			cout<<"Current Histo file name: "<<histFileName<<endl;
			file->Close();
		}// runs summed
		else{//runs not summed
			std::string runN = extract_run_number_from_filename(runfiles[l]);
			std::string subRunN = extract_subrun_number_from_filename(runfiles[l]);
			current_runN = atoi(runN.c_str());
			current_subrunN = atoi(subRunN.c_str());
			if(l ==0){
				if(save_ttree == "YES")
					treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
				if(save_histo =="YES")		
					histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
			}
			else{
				if(current_runN == prev_runN){// have the same run number
					//first check if they are different runs with same run number
					if(current_subrunN == prev_subrunN){
						// 2 consecutive filess with same run number and same sub run numbers must be 2 different run numbers 
						run_increament++;
						if(save_ttree == "YES")
							treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
						if(save_histo =="YES")
							histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
					}
					else{
						if(is_a_new_run(l, runfiles)){
							run_increament++;
							if(save_ttree == "YES")
								treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
							if(save_histo =="YES")
								histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
						}else{
							if(sum_subRuns == "NO"){
								if(save_ttree == "YES")
									treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, false, run_increament);
								if(save_histo =="YES")
									histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, false, run_increament);
							}
						}
					}
				}
				else{//new run
					run_increament =0;
					if(save_ttree == "YES")
						treeFileName = get_tree_filename(l, t_sub_str, output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
					if(save_histo =="YES")
						histFileName = get_histo_filename( l,  h_sub_str,  output_dir, runN, subRunN, sum_runs, sum_subRuns, runfiles, true, run_increament);
				}
			}

			//-----------------
			//  Gru part
			//  ---------------
			if(l ==0){
				file = new GMFMFile(current_file.data());   
				file->Open();                          // Open Device
				file->Rewind();        // rewind run								
				a= new GUser(2, file);            // creat user treatement environement
				a->g_instance = a;
				a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
				a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
				//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
				if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
				a->InitUser();     // Do Init User()
				serv = new GNetServerRoot(server_port, a);
				serv->StartServer();
			}
			else{
				if(current_runN == prev_runN){// have the same run number
					//first check if they are different runs with same run number
					if(current_subrunN == prev_subrunN){
						//--------------
						// save old file
						//--------------				
						a->EndUser();
						if(save_ttree == "YES")	a->SaveUserTTree();     
						if(save_histo == "YES")	a->SpeSave(prev_histFileName.c_str());

						delete (a);   
						a = NULL;
						delete file;
						file = NULL;
						//------------------
						// open a new file
						//--------------------
						file = new GMFMFile(current_file.data());   
						file->Open();                          // Open Device
						file->Rewind();        // rewind run								
						a= new GUser(2, file);            // creat user treatement environement
						a->g_instance = a;
						a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
						a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
						//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
						if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
						a->InitUser();     // Do Init User()
						serv->SetAcq(a);
					}
					else{//current file with diferent sub run no
						if(is_a_new_run(l, runfiles)){
							//--------------
							// save old file
							//--------------				
							a->EndUser();     
							if(save_ttree == "YES")	a->SaveUserTTree();
							if(save_histo == "YES")	a->SpeSave(prev_histFileName.c_str());
							delete (a);   
							a = NULL;
							delete file;
							file = NULL;
							//------------------
							// open a new file
							//--------------------
							file = new GMFMFile(current_file.data());   
							file->Open();                          // Open Device
							file->Rewind();        // rewind run								
							a= new GUser(2, file);            // creat user treatement environement
							a->g_instance = a;
							a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
							a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
							//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
							if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
							a->InitUser();     // Do Init User()
							serv->SetAcq(a);

						}else{
							if(sum_subRuns == "NO"){
								//----------------
								// Save old file
								// --------------
								a->EndUser();  
								if(save_ttree == "YES")	a->SaveUserTTree();   
								if(save_histo == "YES") a->SpeSave(prev_histFileName.c_str());
								delete (a);  
								a = NULL;
								delete file;
								file = NULL;
								//----------------                                                        
								//open new file
								//---------------
								file = new GMFMFile(current_file.data());   
								file->Open();                          // Open Device
								file->Rewind();        // rewind run								
								a= new GUser(2, file);            // creat user treatement environement
								a->g_instance = a;
								a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
								a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
								//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
								//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
								if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
								a->InitUser(); 
								serv->SetAcq(a);

							}else {
								file->SetDevice(current_file.data()); // change of run
								file->Open();                          // Open Device
								file->Rewind();        // rewind run								
								a->SetDevIn(file);
								a->g_instance = a;
								serv->SetAcq(a);
							}
						}

					}
				}
				else{//new run
					//--------------
					// save old file
					//--------------				
					a->EndUser(); 
					if(save_ttree == "YES")	a->SaveUserTTree();    
					if(save_histo == "YES")	a->SpeSave(prev_histFileName.c_str());
					delete (a);   
					a = NULL;
					delete file;
					file = NULL;
					//----------------
					// open a new file
					//----------------
					file = new GMFMFile(current_file.data());   
					file->Open();                          // Open Device
					file->Rewind();        // rewind run								
					a= new GUser(2, file);            // creat user treatement environement
					a->g_instance = a;
					a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
					a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
					//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
					if(save_ttree == "YES")	a->InitUserTTree(treeFileName.c_str());
					a->InitUser();     // Do Init User()
					serv->SetAcq(a);
				}
			}
			if(number_of_events_to_process == "ALL")
				a->DoRun();
			else{
				int  nEvents = atoi(number_of_events_to_process.c_str());
				a->DoRun(nEvents);
			}
			cout<<"Current TTree file name: "<<treeFileName<<endl;
			cout<<"Current Histo file name: "<<histFileName<<endl;
			file->Close();
			prev_runN = current_runN;
			prev_subrunN = prev_subrunN;
			prev_treeFileName = treeFileName;
			prev_histFileName = histFileName;
		}
	}//file loop
	//--------------
	// save last file
	//--------------				
	a->EndUser(); 
	if(save_ttree == "YES")	a->SaveUserTTree();
	if(save_histo == "YES") a->SpeSave(histFileName.c_str());
	delete (a);   
	a = NULL;
	delete file;
	file = NULL;
	runfiles.clear();
	delete[] t_sub_str;
	delete[] h_sub_str;
	delete serv;// Not deleting the following generate SysError in <TUnixSystem::DispatchOneEvent>: select: read error on 42 (Bad file descriptor)
	serv = NULL;
	net->Close();
	delete net;
	net = NULL;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//----------------------------------------
// function to convert to Upper case 
//----------------------------------------
/*! Converts a string of lower case characters string into upper case.
*/
void to_upper(std::string & str){
	std::string str1 = str;
	str.clear();

	for(std::string::size_type i = 0; i < str1.length(); i++){
		if(std::isalpha(str1[i])) str.append(1, std::toupper(str1[i]));
		else str.append(1, str1[i]);
	}
	str1.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//----------------------------------------------------------
// function to get the existing files in the input directory 
//----------------------------------------------------------
//! To get the existing file names of the listed run numbers given in the list from the input directory. 
/*! Since, neither popen nor Dir works with root version 5 in execution of a macro (without compilation), the nales of all the files in the input directory are written to 'file_names.txt' file using usr/bin/ls. This files is then read and only files having the right format "run_%04d.dat.%*" and specified run numbers are kept.
 * @param list a container for the run numbers specified in the Run.config file.
 * @param dir absolute path of the directory where the data is stored.
 */
std::vector<std::string> get_existing_files(std::string dir, vector<int> list){
	size_t nFiles = list.size();
	vector<std::string> runfiles;
	//Note neither popen nor Dir works with root version 5
	std::ifstream files_inDir;
	std::string command = "/usr/bin/ls " + dir + "  >  file_names.txt";
	std::string file;
	system(command.c_str());
	files_inDir.open("file_names.txt");
	if(files_inDir.is_open()){
		while(files_inDir.good() && !files_inDir.eof()){
			getline(files_inDir, file);
			if(file.empty())continue;
			for(size_t j = 0; j < nFiles; j++){
				int runNo= 0;
				if(sscanf(file.c_str(), "run_%04d.dat.%*", &runNo))
				{
					if(runNo == list[j]){
						runfiles.push_back(file);
					}
				}			
			}
		}
		files_inDir.close();
	}else{
		std::cout<<"filelist could not be opend."<<std::endl;
	}

	return runfiles;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//----------------------------------------------------------
// function to get the existing files in the input directory 
//----------------------------------------------------------
//! Overloaded function of get_existing_files(std::string input_dir, std::vector<int> list).
/*! To get the existing file names of the listed run numbers given in the list from the input directory.
 * @param list a container for the run numbers specified in the Run.config file.
 * @param dir absolute path of the directory where the data is stored.
 */
std::vector<std::string> get_existing_files(std::string dir, std::vector<std::string> list){
	size_t nFiles = list.size();
	vector<std::string> runfiles;
	//Note neither popen nor Dir works with root version 5
	std::ifstream files_inDir;
	std::string command = "/usr/bin/ls " + dir + "  >  file_names.txt";
	std::string file;
	system(command.c_str());
	files_inDir.open("file_names.txt");
	if(files_inDir.is_open()){
		while(files_inDir.good() && !files_inDir.eof()){
			getline(files_inDir, file);
			if(file.empty())continue;
			for(size_t j = 0; j < nFiles; j++){
				int runNo= 0;
				if(sscanf(file.c_str(), "run_%04d.dat.%*", &runNo))
				{
					if(file.c_str() == list[j]){
						runfiles.push_back(file);
					}
				}			
			}
		}
		files_inDir.close();
	}else{
		std::cout<<"filelist could not be opend."<<std::endl;
	}
	list.clear();
	return runfiles;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//------------------------------------------------------------------------------------
// function to get the run and sub run numbers from the specified format in Run.config 
//------------------------------------------------------------------------------------
//! This to extract the run numbers and sub run numbers.
/*! @param run_numbers can be specified in different formats(run number = '119, 120,...', '1-20', '1-20, 21, 40-45', 'all') in the Run.config file. Care must be taken when specifying 'all' as not all the run numbers have the same data format.
*/
std::vector<int> extract_run_numbers_from_input(std::string run_numbers){
	std::vector<int> Runs;
	to_upper(run_numbers);	
	if(run_numbers.compare("ALL") ==0){
		for(int i =0; i < 1000; i++)Runs.push_back(i);
	}
	else{
		while(run_numbers.length() > 0){
			std::size_t pos_coma = run_numbers.find(",");
			if(pos_coma!= std::string::npos){
				std::string val = run_numbers.substr(0,pos_coma); 
				std::size_t pos_dash = val.find("-");
				if(pos_dash !=std::string::npos){
					std::string val1 = val.substr(0,pos_dash);
					std::string val2 = val.substr(pos_dash+1, val.length());
					for(int i = atoi(val1.c_str()) ; i<= atoi(val2.c_str()); i++) Runs.push_back(i);
				}else{
					//single value
					int r = atoi(val.c_str());
					Runs.push_back(r);
				}

				run_numbers.erase(0,pos_coma+1);
			}else{
				//without comma last element
				std::size_t pos_dash = run_numbers.find("-");

				if(pos_dash !=std::string::npos){
					std::string val1 = run_numbers.substr(0,pos_dash);
					std::string val2 = run_numbers.substr(pos_dash+1, run_numbers.length());
					for(int i = atoi(val1.c_str()) ; i<= atoi(val2.c_str()); i++) Runs.push_back(i);
				}else{
					//lonely value
					int r = atoi(run_numbers.c_str());
					Runs.push_back(r);
				}

				run_numbers.erase(0,run_numbers.length());
			}
		}
	}
	return Runs;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//----------------------------------------
// function to get the files to processed 
//----------------------------------------
//! To get the name of the files to be processed. 
/*! get_existing_files() functions list all the files of the specified run numbers. This function then further filter the files specified using the subRuns parameter.
 * @param list = get_existing_files()
 * @param subRuns list of sub run numbers specified in the Run.config file.
 */ 
std::vector<std::string> get_to_be_processed_files(std::vector<std::string> list, std::vector<int> subRuns){
	vector<std::string> runfiles;
	bool subRunFound =0;
	//		for(int i =0; i < subRuns.size(); i++) cout<<"subruns to process: "<<subRuns[i]<<endl;
	for(unsigned int l =0; l < list.size(); l++){
		if(list[l].empty())continue;
		subRunFound = 0;
		std::string subRunN = list[l];
		size_t pos = subRunN.find_last_of("s");
		if(pos!=std::string::npos)subRunN.erase(0, pos+2);
		if(subRunN.empty()) subRunN = "0";
		for(unsigned int i = 0; i < subRuns.size(); i++){
			if( atoi(subRunN.c_str()) == subRuns[i]){
				subRunFound = true; break;
			}
		}
		if(subRunFound)	runfiles.push_back(list[l]);
	}
	if(runfiles.size() ==0)cout<<"Please specify run numbers and sub run numbers properly in Run.config"<<endl;
	//	for(int i =0; i < runfiles.size(); i++) cout<<"run to process: "<<runfiles[i]<<endl;
	return runfiles;

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//-----------------------------------------------
// function o extract run numbers from filenames 
//-----------------------------------------------
/*! To extract the run number from the file name.
*/
std::string extract_run_number_from_filename(std::string filename){
	std::string runN =  filename;
	std::size_t pos = runN.find_first_of("_");
	if(pos!=std::string::npos)runN.erase(0, pos+1);
	pos = runN.length();
	if(pos!=std::string::npos)runN.erase(4, pos);
	return runN;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//-----------------------------------------------
// function to extract subrun numbers from filenames 
//-----------------------------------------------
/*! To extract the sub run number from the file name.
*/
std::string extract_subrun_number_from_filename( std::string filename){
	std::string subRunN = filename;
	std::size_t pos = subRunN.find_last_of("s");
	if(pos!=std::string::npos)subRunN.erase(0, pos+2);
	if(subRunN.empty()) subRunN = "0";
	return subRunN;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//-----------------------------------------------
// function to get the range of sub run numbers 
//-----------------------------------------------
//! Get the true range of the sub run numbers.
/*! One could specify run number = 100-150 and sub run number = 0-100 in the Run.config, but, it may be possible that not all the runs have that many sub run files. This function extracts the true range of the sub run rumbers for each run. This is particularly useful in naming the TTree and histogram files.
*/
std::string* get_current_subrunRange(int l, std::vector<std::string> list){
	std::string *srange = new std::string[2];
	srange[0] = extract_subrun_number_from_filename(list[l]);
	srange[1] = srange[0]; // if for loop is not executed
	std::size_t r_length = list[l].length();
	int c0 = atoi(srange[0].c_str());
	int c1 =0;
	std::string temp;
	for(unsigned int i =0; i < list.size(); i++){
		if(i == l) continue;
		std::size_t found = list[i].find(list[l]);
		if(found!= std::string::npos){
			std::size_t l_length = list[i].length();
			if(r_length == l_length){
				srange[1]=srange[0];
			}else{
				temp = extract_subrun_number_from_filename(list[i]);
				c1 = atoi(temp.c_str());
				if(c1 > c0){
					c0 = c1;
					srange[1] = temp; 
				}
			}
		}
	}
	return srange;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//------------------------------------------------
//extract substrings from the given file format
//-------------------------------------------------
//! To extract the format of the output (TTree and histogram) files.
/*! In the Run.config file one could specify the name of the file format without removing the '(Run)' and '(subRun)'.
*/
std::string * extract_oFileFormats_substrings(std::string fileFormat){
	std::string * sub_str = new std::string[3];
	sub_str[0] = fileFormat;
	std::size_t pos = sub_str[0].find("(Run)");
	if(pos!=std::string::npos)sub_str[0] = sub_str[0].erase(pos,sub_str[0].length() );
	sub_str[1]  = fileFormat;
	std::size_t pos2 = sub_str[1].find("(subRun)");
	if(pos2!=std::string::npos)sub_str[1] = sub_str[1].erase( pos2, sub_str[1].length());
	if(pos!=std::string::npos)sub_str[1] = sub_str[1].erase( 0, pos+5);
	sub_str[2] = fileFormat;
	if(pos2!=std::string::npos)sub_str[2] = sub_str[2].erase(0, pos2+8);


	return sub_str;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//------------------------------------------------
// format output ttree file name
//----------------------------------------------
//! Get the name of the Ttree file name.
/*! Depending on the option chosesn in processing the data such as sum runs into one file, for each run, sum the sub runs, the name of the TTree file is generated. If you chose sum runs = all, then, the file name will take (RUN = run numbers), (subRun = all) in the TTree file format. 
*/
std::string get_tree_filename(int l, std::string *t_sub_str, std::string output_dir, std::string runN, std::string subRunN, std::string sum_runs, std::string sum_subRuns,std::vector<std::string> runfiles, bool new_run_flag, int run_increament){
	std::string treeFileName; 
	std::string *sr; 
	if(l ==0){
		if(sum_runs == "YES"){
			treeFileName = output_dir + "/" + t_sub_str[0] + runN + t_sub_str[1] + "all" + t_sub_str[2];
		}
		else{
			if(sum_subRuns == "NO")
				treeFileName = output_dir + "/" + t_sub_str[0] + runN + t_sub_str[1] + subRunN + t_sub_str[2];
			else{
				sr = get_current_subrunRange(l,runfiles);
				if(sr[0] != sr[1])					
					treeFileName = output_dir+ "/" + t_sub_str[0] + runN + t_sub_str[1] + sr[0]+ "-" + sr[1] + t_sub_str[2];
				else
					treeFileName = output_dir+ "/" + t_sub_str[0] + runN + t_sub_str[1] + sr[0] + t_sub_str[2];
				delete[] sr; sr = NULL;
			}
		}
	}else{
		if(!new_run_flag){// have the same run number
			if( sum_subRuns == "NO")
				treeFileName = output_dir + "/" + t_sub_str[0] + runN + t_sub_str[1] + subRunN + t_sub_str[2];
		}
		else{//new run
			if(run_increament ==0){	
				if(sum_subRuns == "NO")
					treeFileName = output_dir + "/" + t_sub_str[0] + runN + t_sub_str[1] + subRunN + t_sub_str[2];
				else{
					sr = get_current_subrunRange(l, runfiles);
					if(sr[0] != sr[1])					
						treeFileName =  output_dir + "/" + t_sub_str[0] + runN + t_sub_str[1] + sr[0] + "-" + sr[1] + t_sub_str[2];
					else 
						treeFileName =  output_dir + "/" + t_sub_str[0] + runN + t_sub_str[1] + sr[0] + t_sub_str[2];
					delete[] sr; sr = NULL;
				}
			}else{
				stringstream ss;
				ss << run_increament;
				string run_inc_str = ss.str();
				if(sum_subRuns == "NO")
					treeFileName = output_dir + "/" + t_sub_str[0] + runN + "+" + run_inc_str + t_sub_str[1] + subRunN + t_sub_str[2];
				else{
					sr = get_current_subrunRange(l, runfiles);
					if(sr[0] != sr[1])					
						treeFileName = output_dir + "/" + t_sub_str[0] + runN + "+" + run_inc_str + t_sub_str[1] + sr[0] + "-" + sr[1] + t_sub_str[2];
					else
						treeFileName = output_dir + "/" + t_sub_str[0] + runN + "+" + run_inc_str + t_sub_str[1] + sr[0]  + t_sub_str[2];
					delete[] sr; sr = NULL;
				}

			}
		}
	}
	return treeFileName;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//------------------------------------------------
// format output histo file name
//----------------------------------------------
//! Get the name of the histogram file name.
/*! Depending on the option chosesn in processing the data such as sum runs into one file, for each run, sum the sub runs, the name of the hisogram file is generated. If you chose sum runs = all, then, the file name will take (RUN = run numbers), (subRun = all) in the Histogram file format. 
*/
std::string get_histo_filename(int l, std::string *h_sub_str, std::string output_dir, std::string runN, std::string subRunN, std::string sum_runs, std::string sum_subRuns,std::vector<std::string> runfiles, bool new_run_flag, int run_increament){
	std::string histFileName;
	std::string *sr; 
	//-----	
	if(l ==0){
		if(sum_runs == "YES"){
			histFileName = output_dir + "/" + h_sub_str[0] + runN + h_sub_str[1] + subRunN + h_sub_str[2];
		}
		else{
			if(sum_subRuns == "NO")
				histFileName = output_dir + "/" + h_sub_str[0] + runN + h_sub_str[1] + subRunN + h_sub_str[2];
			else{
				sr = get_current_subrunRange(l, runfiles);
				if(sr[0] != sr[1])					
					histFileName = output_dir+ "/" + h_sub_str[0] + runN + h_sub_str[1] + sr[0]+ "-" + sr[1] + h_sub_str[2];
				else
					histFileName = output_dir+ "/" + h_sub_str[0] + runN + h_sub_str[1] + sr[0] + h_sub_str[2];
				delete[] sr; sr= NULL;
			}
		}
	}else{
		if(!new_run_flag){// have the same run number
			if(sum_subRuns == "NO")
				histFileName = output_dir + "/" + h_sub_str[0] + runN + h_sub_str[1] + subRunN + h_sub_str[2];
		}
		else{//new run
			if(run_increament ==0){
				if(sum_subRuns == "NO")
					histFileName = output_dir+ "/" + h_sub_str[0] + runN + h_sub_str[1] + subRunN + h_sub_str[2];
				else{
					sr = get_current_subrunRange(l, runfiles);
					if(sr[0] != sr[1])	
						histFileName = output_dir+ "/" + h_sub_str[0] + runN + h_sub_str[1] + sr[0] + "-" + sr[1] + h_sub_str[2];
					else
						histFileName = output_dir+ "/" + h_sub_str[0] + runN + h_sub_str[1] + sr[0] + h_sub_str[2];
					delete[] sr;sr = NULL;
				}
			}else{
				//Rename the files to avoid over writing
				stringstream ss;
				ss << run_increament;
				string run_inc_str = ss.str();
				if(sum_subRuns == "NO")
					histFileName= output_dir+ "/" + h_sub_str[0] + runN + "+" + run_inc_str + h_sub_str[1] + subRunN + h_sub_str[2];
				else{
					sr = get_current_subrunRange(l, runfiles);
					if(sr[0] != sr[1])					
						histFileName = output_dir + "/" + h_sub_str[0] + runN + "+" + run_inc_str + h_sub_str[1] + sr[0] + "-" + sr[1] + h_sub_str[2];
					else
						histFileName = output_dir + "/" + h_sub_str[0] + runN + "+" + run_inc_str + h_sub_str[1] + sr[0] + h_sub_str[2];
					delete[] sr; sr = NULL;
				}

			}
		}
	}
	return histFileName;
}	
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
//-----------------------
//Check if it is new run
//-----------------------
//! This is to check if the current run from the list of files to be processed is a new run. 
/*!It is useful because sometimes two or more files can have same run numbers.
*/
bool is_a_new_run(int l, std::vector<std::string> list){

	if(l == 0) return true;
	else if(l > 0){
		//--------------------------------------------------
		//compare with the previous file
		//---------------------------------------------------
		std::string prev_file;
		std::size_t pos = list[l-1].find_last_of("s");
		if(pos!=std::string::npos)prev_file = list[l-1].substr(0, pos+1);

		std::string current_file;
		pos = list[l].find_last_of("s");
		if(pos!=std::string::npos)current_file = list[l].substr(0, pos+1);

		if(current_file.compare(prev_file) == 0){ return false;}
		else{ return true;}
	}
	else {cout<<"put l >= 0"<<endl; return 0;}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------//
