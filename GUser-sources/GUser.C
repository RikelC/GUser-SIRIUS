/*!
 * \file GUser.C
 * \author Luc Legeard
 * \brief  Class for User treatment
 * \details modified by Rikel CHAKMA
 *          Original Tracker part of the code written by Julien Pancin and Thomas Roger
 *          was added in this program on 01/03/2023
 *
 */
#include "./GUser.h"
#include "TROOT.h"
//#include <TProfile.h>
#include "GEventMFM.h"

#include "DssdParameters.h"
using namespace DSSD_PARAMETERS;
namespace DSSD = DSSD_PARAMETERS;
ClassImp (GUser);
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! A static instance of the GUser class.
/*! This is needed for clearing the memory in the event of premature interruption. 
*/
GUser* GUser::g_instance = 0;
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! signal handler function
/*! When SIGINT, SIGTERM signals are caught, the meamory is cleared before exiting the program.
*/
void signal_handler(int signal_number){

	if(signal_number == SIGINT)
		std::cout<<RED<<"SIGINT signal = "<<signal_number<<" received!!!"<<RESET<<std::endl;
	else if(signal_number == SIGTERM)
		std::cout<<RED<<"SIGTERM signal = "<<signal_number<<" received!!!"<<RESET<<std::endl;
	GUser* a = GUser::g_instance;
	if(a){
		a->EndUser();
		delete (a);
		a = NULL;
	}
	gROOT->Reset();	
	exit(signal_number);

}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! GUser Constructor
/*! Here, the variables are initialized. The hitograms are created here. If you create the histograms in the InitUser(), it may result in segmentation fault during premature termination.
*/
GUser::GUser (int mode, GDevice* _fDevIn, GDevice* _fDevOut):GAcq(_fDevIn,_fDevOut)
{
	readMode = mode;
	s1                             = MyGlobal::GetInstance(readMode);
	//s1                             = myGlobal::getInstance();
	fCoboframe                     = new MFMCoboFrame();
	fInsideframe                   = new MFMCommonFrame();
	fMergeframe                    = new MFMMergeFrame();
	fSiriusframe                   = new MFMSiriusFrame();
	fGenericframe                  = new MFMReaGenericFrame();
	fMutantframe                   = new MFMMutantFrame();
	fEbyedatframe                  = new MFMEbyedatFrame();

	filter                         = new DigitalFilters();
	cfd                            = new DigitalCFD();
	dData                          = new DssdData();
	dEvent                         = new MakeDssdEvents();
	tEvent                         = new MakeTunnelEvents();
	tData                          = new TunnelData();
	calib                          = new Calibration();
	coboData                       = new TrackerCoBoData();
	timeAlign                      = new TimeAlignment();
	correlation                    = new Correlation();
	userTree                       = new UTTree();
	//	fRecoilEvent = new RecoilEvent();
	//	fDecayEvent = new DecayEvent();
	frameCounter                   = new ullint[8];
	tunnel_rate_pad                = new double[s1->NofMacroPixels];
	tunnel_rate_board              = new double[s1->NBOARDS_TUNNEL];
	dssd_rate_strip                = new double[s1->NSTRIPS_DSSD];
	dssd_rate_board                = new double[s1->NBOARDS_DSSD]; 
	rate_counterPoint_dssd         = new ullint[s1->NBOARDS_DSSD];
	rate_counterPoint_tunnel       = new ullint[s1->NBOARDS_TUNNEL];
	//----------------- initializaton -----------------//
	rate_calcul_timestamp_lapse    = static_cast<int64_t>(s1->rate_calcul_time_lapse *pow(10,8));
	framesize                      = 0;
	maxdump                        = 128;
	channel                        = 0;
	board                          = 0;
	iboard                         = 0;
	value                          = 0;
	rate_counter_dssd              = 0;
	rate_counter_tunnel            = 0;
	dataSet_counter                = 0.;
	prev_padNo                     = -100;
	dataSet_counter                = 0;
	eventnumber 		       = 0;
	timestamp                      = 0;
	old_timeStamp                  = 0;
	rate_counter_dssd              = 0;
	rate_counter_tunnel            = 0;
	deltaT_rates                   = 0;
	gain                           = 0;
	stripnumber                    = 0;
	dssdBoardNo                    = 0;
	tunnelBoardNo                  = 0;
	tunnelPadNo                    = 0;
	tunnelDetectorNo               = 0;
	Energy                         = 0.;
	calibEnergy                    = 0.;
	Time                           = 0.;
	sed_tot_counter                = 0;
	dssd_tot_counter               = 0;	
	nCoboFrame                     = 0;
	sed_counter1                   = 0;
	dssd_counter1                  = 0;
	reaGenericEnergy               = 0;
	reaGenericTime                 = 0;
	belong_to_a_merge_frame        = false;
	reached_eof                    = false; 
	jitter =0;
	for(int i = 0; i < 8; i++)frameCounter[i] = 0;
	for(int i = 0; i < s1->NSTRIPS_DSSD; i++){
		dssd_rate_strip[i] =0.;
	}
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		rate_counterPoint_dssd[i]=0;	
		dssd_rate_board[i] =0.;
	}
	for(int i = 0; i < s1->NofMacroPixels; i++){
		tunnel_rate_pad[i] =0.;
	}
	for(int i = 0; i < s1->NBOARDS_TUNNEL; i++){
		rate_counterPoint_tunnel[i]=0;	
		tunnel_rate_board[i] =0.;
	}

	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			dssd_event_counter[i][j] =0;
		}
	}
	CreateAnalysisHistograms();
	//----------------- Create histograms -----------------
	if(s1->fsaveHisto || s1->acquisitionMode.compare("ONLINE") == 0)
		CreateHistograms();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! GUser Destructor
/*! Delete instances of all the classes and clears the heap memory.
*/
GUser::~GUser()  {
	/*ULong64_t tot =0;
	  for(int i = 0; i < s1->NBOARDS_DSSD; i++){

	  for(int j = 0; j < NCHANNELS; j++){
	  tot += dssd_event_counter[i][j];
	  cout<<"board: "<<s1->boardList_DSSD[i] <<"  channel: "<<j<<"  counts: "<< dssd_event_counter[i][j]<<endl;
	  }
	  }
	  cout<<"tot "<<tot<<endl;
	  */

	if(s1->fsaveHisto || s1->acquisitionMode.compare("ONLINE") == 0){
		DeleteHistograms();
		delete [] evtCounter;
	}

	delete [] frameCounter;
	delete [] tunnel_rate_pad;
	delete [] tunnel_rate_board;
	delete [] dssd_rate_strip;
	delete [] dssd_rate_board;
	delete [] rate_counterPoint_dssd;
	delete [] rate_counterPoint_tunnel;


	dssdDataPointVec.clear();
	dssdDataPointVec_merged.clear();

	trackerNumexo2EventVec.clear();
	trackerNumexo2EventVec_merged.clear();

	trackerCoBoEventVec.clear();
	trackerCoBoEventVec_merged.clear();

	tunnelDataVec.clear();
	tunnelDataVec_merged.clear();

	dssdEventVec.clear();
	dssdEventVec_merged.clear();

	recoilTypeEvents.clear();
	decayTypeEvents.clear();

	if(fCoboframe) delete fCoboframe;
	if(fInsideframe) delete fInsideframe;
	if(fMergeframe) delete fMergeframe;
	if(fSiriusframe) delete fSiriusframe;
	if(fGenericframe) delete fGenericframe;
	if(fMutantframe) delete fMutantframe;
	if(fEbyedatframe) delete fEbyedatframe;

	if(filter) delete filter;
	if(cfd) delete cfd;
	if(dData) delete dData;
	if(dEvent) delete dEvent;
	if(tEvent) delete tEvent;
	if(tData) delete tData;
	if(calib) delete calib;
	if(coboData) delete coboData;
	if(timeAlign) delete timeAlign;
	if(correlation) delete correlation;
	if(userTree) delete userTree;
	//if(fRecoilEvent) delete fRecoilEvent;
	//if(fDecayEvent) delete fDecayEvent;
	cout<<"GUser Desctructor called"<<endl;
	gROOT->cd();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Initialisation for global  user treatement
/*! The variables and histograms are initialized at the start.
*/
void GUser::InitUser()
{
	//dData->PrintMapping();
	for(int i = 0; i < 8;i++)frameCounter[i] =0;
	rate_counter_dssd=0;
	rate_counter_tunnel=0;
	dataSet_counter = 0.;
	belong_to_a_merge_frame = false;
	for(int i =0; i< s1->NSTRIPS_DSSD;i++){
		dssd_rate_strip[i] =0.;
	}
	for(int i =0; i< s1->NBOARDS_DSSD;i++){
		rate_counterPoint_dssd[i]=0;	
		dssd_rate_board[i] =0.;
	}
	for(int i =0; i< s1->NofMacroPixels;i++){
		tunnel_rate_pad[i] =0.;
	}
	for(int i =0; i< s1->NBOARDS_TUNNEL;i++){
		rate_counterPoint_tunnel[i]=0;	
		tunnel_rate_board[i] =0.;
	}
	//initialize some preset value
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			dssd_event_counter[i][j] =0;
		}
	}

	if(s1->fsaveHisto || s1->acquisitionMode.compare("ONLINE") == 0)
		ResetHistograms();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Initializarion for each run
/*! If you want to reset some variables or histograms at the start of each file.
*/
void GUser::InitUserRun()
{
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! end of each run
/*! Called at the end of each run. Here DSSD pixels need to Constructed or correlations between events must be made to treat the last buffer.
*/
void GUser::EndUserRun()
{
	reached_eof = true; 
	FindCorrelations("");
	//correlations->Clear();
	tEvent->Construct(tunnelDataVec, htunnel1_E1E2,  htunnel2_E1E2, htunnel3_E1E2, htunnel4_E1E2, htunnel1_dt,htunnel2_dt, htunnel3_dt, htunnel4_dt);

	cout<< "calling GUser::EndUserRun()"<<endl;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! End of global run
/*! This method must be called explicitly. Prints the total number of different data frames at the end of the program. 
*/
//-------------------
void GUser::EndUser()
{
	cout<<"sed tot  counter "<<sed_tot_counter<<"  dssd tot couner "<<dssd_tot_counter<<endl;
	cout<<"####################end of run "<<endl;
	for(int i = 0; i < 8;i++)cout<<"Frame: "<<s1->frameName[i] <<"  no: "<<frameCounter[i]<<endl;
	cout<< "calling GUser::EndUser"<<endl;

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Read frames from the files.
void GUser::User()
{
	std::signal(SIGINT,signal_handler);
	std::signal(SIGTERM,signal_handler);
	MFMCommonFrame * cf;
	static long int nb=0 , nberror=0;
	nb++;
	cf = (MFMCommonFrame*)(((GEventMFM*)GetEvent())->GetFrame());
	int type = cf->GetFrameType();
	//cf->SetAttributs();
	switch (type) {
		case MFM_COBOF_FRAME_TYPE:
		case MFM_COBO_FRAME_TYPE:
			break;
		case MFM_EBY_EN_FRAME_TYPE:
		case MFM_EBY_TS_FRAME_TYPE:
		case MFM_EBY_EN_TS_FRAME_TYPE:
			frameCounter[1]++;
			break;					     
		case MFM_HELLO_FRAME_TYPE:
			frameCounter[2]++;
			break;
		case MFM_XML_FILE_HEADER_FRAME_TYPE:
			frameCounter[3]++;
			break;
		case MFM_REA_TRACE_FRAME_TYPE:
			frameCounter[4]++;
			break;
		case MFM_REA_GENE_FRAME_TYPE:
			break;
		case MFM_SIRIUS_FRAME_TYPE:
			break;
		case MFM_MERGE_EN_FRAME_TYPE:
		case MFM_MERGE_TS_FRAME_TYPE:
			break;
		default:
			nberror++;
			frameCounter[8]++;
			break;
	}// end of switch

	if (cf == NULL) {
		fError.TreatError(1,0,"in GUser::User() return null frfReduce_factorame in GetEvent())->GetFrame()");
	}else {
		//cf->HeaderDisplay();
		//cf->DumpRaw(128,0);
		UserFrame(cf);
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Treatment of MFM frames
void GUser::UserFrame(MFMCommonFrame* commonframe){

	int type =  commonframe->GetFrameType();
	//commonframe->SetAttributs();
	PrintInfo(commonframe);
	if ((type == MFM_MERGE_EN_FRAME_TYPE)or(type == MFM_MERGE_TS_FRAME_TYPE)) {
		frameCounter[7]++;
		UserMergeFrame(commonframe);
	}  
	else if ((type == MFM_COBO_FRAME_TYPE) or (type== MFM_COBOF_FRAME_TYPE)) {
		frameCounter[0]++;
		UserCoboFrame(commonframe);
		//userTree->FillCoBoFrames();
	}
	else if (type == MFM_SIRIUS_FRAME_TYPE){
		frameCounter[6]++;
		UserSiriusFrame(commonframe);
		//userTree->FillSiriusFrames();
	}
	else if(type == MFM_REA_GENE_FRAME_TYPE){
		frameCounter[5]++;
		UserGenericFrame(commonframe);
		//	userTree->FillReaGenericFrames();
	}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Treatment of merged frames
/*! The frames within the defined time window are groupped into merged frames. Normally, if everything is ok, the DSSD pixel Construction and establishing correlations among the events from different detector are done here. The only catch of enabling the Merger is it consumes a lot of memory.   
*/ 
void GUser::UserMergeFrame(MFMCommonFrame* commonframe){
	int i_insframe = 0;
	int nbinsideframe = 0;

	fMergeframe->SetAttributs(commonframe->GetPointHeader());

	nbinsideframe = fMergeframe->GetNbItems();
	timestamp = fMergeframe->GetTimeStamp();
	//cout << nbinsideframe << " " << fMergeframe->GetEventNumber() << endl;

	//	cout<<"------------------- ninside frame = "<<nbinsideframe<<endl;
	framesize= fMergeframe->GetFrameSize();
	fMergeframe->ResetReadInMem();
	//Reset vectors
	for(i_insframe = 0; i_insframe < nbinsideframe; i_insframe++) {
		fMergeframe->ReadInFrame(fInsideframe);
		belong_to_a_merge_frame = true;
		UserFrame(fInsideframe);

	}
	belong_to_a_merge_frame = false;
	// At this point you can do treatement inter frames
	FindCorrelations("Merged");
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Treatment of Generic frames (for the Tunnel detectors)
/*! The energy and timestamps are obtained from the data. To get the board number, macro pixel number and the detector number, use the methods from the tunnelData class. The macro pixel configurations are specified in the Run.config. The configuration can be changed (for example ABCD to BADC) keeping the macro pixels definitions constant i.e., if pixels 1 and 2 are grouped as A, you should not define it as B. If you do so, remember to update in tunnelData class, otherwise, the correct picture (2D hit pattern) will not be displayed.
 * To calibrate do Calibration::Perform(tunnelData* const data).
 * To get the board number use tunnelData::get_tunnelBoardNumber(int* p_board).
 * To get the physical macro pixel number use tunnelData::GetMacroPixelPhysicalNumber(int* p_board, int* p_channel). This method also detemines the position of the macro pixel.
 * To get the detector number use  tunnelData::get_tunnelDetectorNumber(int* p_board, int* p_channel).
 */
void GUser::UserGenericFrame(MFMCommonFrame* commonframe)
{
	fGenericframe->SetAttributs(commonframe->GetPointHeader());
	framesize=commonframe->GetFrameSize();
	channel =fGenericframe->GetChannelId();
	board =  fGenericframe->GetBoardId();
	reaGenericEnergy = fGenericframe->GetEnergy();
	reaGenericTime = fGenericframe->GetTimeStamp();
	timestamp = fGenericframe->GetTimeStamp();
	iboard =  s1->boardIndex_Tunnel[board];
	tData->SetChannel(channel);
	tData->SetBoard(board);
	tData->SetBoardIndex(iboard);
	tData->SetTimeStamp( timestamp);
	tData->SetEventNumber( eventnumber);
	tData->SetRawEnergy( reaGenericEnergy);
	h_tunnelRaw[iboard][channel]->Fill (reaGenericEnergy);
	calibEnergy = calib->Perform(tData);
	tData->SetCalibratedEnergy( calibEnergy);
	h_tunnelCalib[iboard][channel]->Fill (calibEnergy);
	tunnelBoardNo = tData->GetTunnelBoardNumber(&board); 
	tunnelPadNo = tData->GetMacroPixelPhysicalNumber(&board, &channel);//gets the pixels
	tPoint.SetValues(tData);
	tunnelDetectorNo = tData->GetTunnelDetectorNumber(&board, &channel);
	h_tunnel_count_board->Fill(tunnelBoardNo);
	h_tunnel_count_pad->Fill(tunnelPadNo);

	//-------------------
	if(s1->fsaveHisto || s1->acquisitionMode.compare("ONLINE") == 0){
		//rate calulation		
		tunnel_rate_pad[tunnelPadNo]++;
		tunnel_rate_board[iboard]++;
		get_Count_Rates(2);
		for(unsigned int i = 0; i< tData->GetMacroPixel(&board,&channel).pixels.size();i++){
			h_TUNNEL_XY_hit[tunnelDetectorNo-1]->Fill(tData->GetMacroPixel(&board,&channel).pixels[i].GetX(),tData->GetMacroPixel(&board,&channel).pixels[i].GetY());
		}

	}
	if(s1->data_merged == 0 || !belong_to_a_merge_frame){
		tunnelDataVec.push_back(tPoint);
		if(tunnelDataVec.size() ==2000){
			if(s1->fsaveHisto || s1->acquisitionMode.compare("ONLINE") == 0)
				tEvent->Construct(tunnelDataVec, htunnel1_E1E2,  htunnel2_E1E2, htunnel3_E1E2, htunnel4_E1E2, htunnel1_dt,htunnel2_dt, htunnel3_dt, htunnel4_dt);
		}
	}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Treatment of Sirius frames
/*!
 *
 *
 */
void GUser::UserSiriusFrame( MFMCommonFrame* commonframe){
	fSiriusframe->SetAttributs(commonframe->GetPointHeader());
	framesize =fSiriusframe->GetFrameSize();
	board = (int)fSiriusframe->GetBoardId();
	channel = (int)fSiriusframe->GetChannelId();
	iboard =  (int)s1->boardIndex_DSSD[board];
	timestamp = fSiriusframe->GetTimeStamp();
	eventnumber = fSiriusframe->GetEventNumber();
	gain = fSiriusframe->GetGain();
	stripnumber = dData->GetStripNumber(&board, &channel);
	dssdBoardNo = dData->GetDssdBoardNumber(&board); 
	//		cout<<"Before time alignment boardId:  "<<board<<" , channel:  "<<channel<<"  strip "<<stripnumber<<" TimeStamp:  "<<timestamp<<" , EventNumber: "<<eventnumber<<endl;
	//timeAlign->AlignTimeStamp(timestamp, board, channel);//align either timestamp or the CFD time
	//		cout<<"After time alignment boardId:  "<<board<<" , channel:  "<<channel<<"  strip "<<stripnumber<<" TimeStamp:  "<<timestamp<<" , EventNumber: "<<eventnumber<<endl;



	//Align time stamp again
	/*
	   jitter = static_cast<llint>(timestamp - prev_timestamp);
	   if(TMath::Abs(jitter) < 5){//within 2 ticks then correct
	   timestamp = timestamp - jitter;
	   }
	   prev_timestamp = timestamp;*/
	// set dssd data values for treatment
	dData->SetChannel(channel);
	dData->SetBoard(board);
	dData->SetBoardIndex(iboard);
	dData->SetTimeStamp(timestamp);
	dData->SetEventNumber( eventnumber);
	dData->SetGain(gain);

	int j =0;	
	for (int i = s1->nStart_trace; i < 992 - s1->nEnd_trace; i++) {
		fSiriusframe->GetParameters(i, &value);
		j = i - s1->nStart_trace;
		if(i < s1->TRACE_SIZE)
			dData->SetTraceValue(j, value);
	}
	//---------------------------
	// Computation
	// -----------------------
	dData->GetSignalInfo();
	if(s1->fsaveHisto)Energy = filter->Perform(dData, hTrap[iboard][channel]);
	else Energy = filter->Perform(dData, NULL);
	if(dData->GainSwitched()) Energy += 10000; 
	calibEnergy = calib->Perform(dData);//Calibration
	//get CFD time in (ns)
	if(board==s1->trackerNumexoBoard && channel == s1->trackerNumexoChannel)Energy = dData->GetSignalHeight();
	//if(dData->GainSwitched()){
	if(board== s1->trackerNumexoBoard && channel == s1->trackerNumexoChannel){
		if(s1->fsaveHisto) Time = cfd->Perform(dData,"SED", hCFD[iboard][channel]);
		else Time = cfd->Perform(dData,"SED", NULL);

	}
	else{
		if(s1->fsaveHisto) Time = cfd->Perform(dData,"DSSD", hCFD[iboard][channel]);
		else Time = cfd->Perform(dData,"DSSD", NULL);

	}
	timeAlign->AlignTime(Time, board, channel);
	double zeroCr = cfd->GetZeroCrossingSample();
	ushort Mpos = dData->GetMaximumPosition();
	//timeAlign->align_time( zeroCr, board, channel);
	//timeAlign->align_time( Mpos, board, channel);
	//Time = (double)timestamp;
	dPoint.SetTimeStamp(timestamp);
	dPoint.SetCFDTime(Time);
	dPoint.SetStrip(stripnumber);
	dPoint.SetEnergy(Energy);
	dPoint.SetBoard(board);
	dPoint.SetChannel(channel);
	dPoint.SetBoardIndex(iboard);
	//dPoint.SetTrace(dData->GetTrace(), s1->TRACE_SIZE);


	if(Energy > 0. && stripnumber >= 0 && stripnumber < 256){//For pixel Construction

		for (int i = s1->nStart_trace; i < 992 - s1->nEnd_trace; i++) {
			fSiriusframe->GetParameters(i, &value);
			j = i - s1->nStart_trace;
			if(i < s1->TRACE_SIZE)
				dPoint.SetTraceValue(j, value);
		}
		if(belong_to_a_merge_frame) dssdDataPointVec_merged.push_back(dPoint);
		else dssdDataPointVec.push_back(dPoint);
	}

	//if(board==164 && channel ==4 && TMath::Abs(dData->GetSignalHeight()) > 0 && TMath::Abs(dData->GetSignalHeight()) < 2000 && dData -> GetMaximumPosition() > 470 && dData -> GetMaximumPosition() < 500){
	if(board== s1->trackerNumexoBoard && channel == s1->trackerNumexoChannel){
		sedNumexo2Event.SetTimeStamp(timestamp);
		sedNumexo2Event.SetCFDTime(Time);
		sedNumexo2Event.SetEnergy(Energy);
		for (int i = s1->nStart_trace; i < 992 - s1->nEnd_trace; i++) {
			fSiriusframe->GetParameters(i, &value);
			j = i - s1->nStart_trace;
			if(i < s1->TRACE_SIZE)
				sedNumexo2Event.SetTraceValue(j, value);
		}
		//sedNumexo2Event.SetTrace(dData->GetTrace(), s1->TRACE_SIZE);

		if(belong_to_a_merge_frame)trackerNumexo2EventVec_merged.push_back(sedNumexo2Event);
		else trackerNumexo2EventVec.push_back(sedNumexo2Event);
	}

	//Fill histograms
	if(s1->fsaveHisto || s1->acquisitionMode.compare("ONLINE") == 0){
		evtCounter[iboard][channel]++;

		grTimestamp[iboard][channel]->SetPoint(evtCounter[iboard][channel], evtCounter[iboard][channel], timestamp);
		// get count rates per second
		dssd_rate_strip[stripnumber]++;
		dssd_rate_board[iboard]++;
		get_Count_Rates(1);
		int j =0;	
		//	if(dData->GetStripNumber() == 228 && dData->GetSignalHeight() > 6000 && dData->GetSignalHeight() < 8000){	
		for (int i = s1->nStart_trace; i < 992 - s1->nEnd_trace; i++) {
			fSiriusframe->GetParameters(i, &value);
			j = i - s1->nStart_trace;
			hTrace_Sum[iboard][channel]->Fill(j,value);
		}
		//}

		h_strip_SignalHeight->Fill(stripnumber, dData->GetSignalHeight());
		h_zeroCTime[iboard][channel]->Fill(zeroCr);
		h_MaxPosTime[iboard][channel]->Fill(Mpos);
		h_strip_zeroCrTime->Fill(stripnumber, zeroCr);
		h_strip_MaxPosTime->Fill(stripnumber,Mpos);
		h_strip_trigger->Fill(stripnumber,dData->GetTrigger());

		h_board_zeroCrTime->Fill(board, zeroCr);
		h_board_MaxPosTime->Fill(board,Mpos);
		if(stripnumber < 128){
			h_E_MaxPosTime->Fill(Energy, Mpos);
			h_E_zeroCrTime->Fill(Energy, zeroCr);
		}	//Fill histograms
		h_raw_strip->Fill(Energy, stripnumber);
		hRaw[iboard][channel]->Fill(Energy);
		h_calib_strip->Fill(dData->GetCalibratedEnergy(),stripnumber);
		hCalib[iboard][channel]->Fill(calibEnergy);
		h_dssd_count_strip->Fill(stripnumber);
		h_dssd_count_board->Fill(dssdBoardNo);
		hTrigger[iboard][channel]->Fill(dData->GetTrigger());
		hBaseline[iboard][channel]->Fill(dData->GetBaseline());
		hNoise[iboard][channel]->Fill(dData->GetNoise());
		hRisetime[iboard][channel]->Fill(dData->GetRiseTime());

		if(dData->GetStripNumber() < 128)h_sum_front->Fill(Energy);
		else if(dData->GetStripNumber() > 127 && dData->GetStripNumber() < 256)h_sum_back->Fill(Energy);

		if(dData->GainSwitched()){
			if(dData->GetStripNumber() < 128 ){
				h_front_rGS->Fill(stripnumber, dData->GetSignalHeight());
				for (int i = s1->nStart_trace; i < 992 - s1->nEnd_trace; i++) {
					fSiriusframe->GetParameters(i, &value);
					j = i - s1->nStart_trace;
					h_front_traceGS->Fill(i, value);
				}
			}

			else if(dData->GetStripNumber() > 127 && dData->GetStripNumber() < 256){

				h_back_rGS->Fill(stripnumber, dData->GetSignalHeight());
				for (int i = s1->nStart_trace; i < 992 - s1->nEnd_trace; i++) {
					fSiriusframe->GetParameters(i, &value);
					j = i - s1->nStart_trace;
					h_back_traceGS->Fill(i, value);
				}
			}
		}else{
			if(dData->GetStripNumber() < 128 ){

				h_front_rNGS->Fill(stripnumber, dData->GetSignalHeight());
				for (int i = s1->nStart_trace; i < 992 - s1->nEnd_trace; i++) {
					fSiriusframe->GetParameters(i, &value);
					j = i - s1->nStart_trace;
					h_front_traceNGS->Fill(i, value);
				}
			}

			else if(dData->GetStripNumber() > 127 && dData->GetStripNumber() < 256){
				h_back_rNGS->Fill(stripnumber, dData->GetSignalHeight());
				for (int i = s1->nStart_trace; i < 992 - s1->nEnd_trace; i++) {
					fSiriusframe->GetParameters(i, &value);
					j = i - s1->nStart_trace;
					h_back_traceNGS->Fill(i, value);
				}
			}

		}
		//Fill baseline graphs
		gr_baseline[iboard][channel]->SetPoint(dssd_event_counter[iboard][channel],dssd_event_counter[iboard][channel],dData->GetBaseline());
		dssd_event_counter[iboard][channel]++;

	}

	FindCorrelations("Unmerged");
}


//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//!  Treatment of Cobo frames
/*!
 *
 *
 *
 */

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void GUser::UserCoboFrame(MFMCommonFrame* commonframe){

	fCoboframe->SetAttributs(commonframe->GetPointHeader());
	int type=commonframe->GetFrameType();
	framesize=fCoboframe->GetFrameSize();
	eventnumber =fCoboframe->GetEventNumber();
	timestamp = (uint64_t)(fCoboframe->GetTimeStamp());

	coboData->SetEventNumber(eventnumber);
	coboData->SetTimeStamp(timestamp);
	coboData->SetCoboIdx(fCoboframe->CoboGetCoboIdx());
	coboData->SetAsaIdx(fCoboframe->CoboGetAsaIdx());
	coboData->SetNItems(fCoboframe->GetNbItems());

	if(coboData->GetCoboIdx() < NB_COBO)
	{				

		coboData->Reset();// Warning do not remove
		//-----------------------------
		short iChan=0;
		short iBuck=0;
		short iAget=0;
		for(unsigned int i2=0;i2<coboData->GetNItems();i2++)
		{	
			fCoboframe->CoboGetParameters(
					i2,
					coboData->GetSampleAddress(),
					coboData->GetBuckIdxAddress(),
					coboData->GetChanIdxAddress(),
					coboData->GetAgetIdxAddress()
					);
			//cout<<"sample: "<<sample<<"  buckidx : "<<buckidx<<" chanidx : "<<chanidx<<" agetidx: "<<agetidx<<endl; 
			if(type==MFM_COBO_FRAME_TYPE)
			{
				coboData->SetTrace();
			}

			else if(type==MFM_COBOF_FRAME_TYPE)
			{
				coboData->SetChanIdx(iChan);
				coboData->SetBuckIdx(iBuck);
				//coboData->setAgetIdx(iAget);
				coboData->SetAsaIdx(fCoboframe->CoboGetAsaIdx());
				coboData->SetTrace();

				//cout<<"toto sample: "<<sample<<"  iBuck : "<<iBuck<<" iChan : "<<iChan<<" agetidx: "<<agetidx<<endl; 
				iChan++;
				if(i2%2==1)
				{
					iAget++;
					iChan-=2;
				}
				if(iAget>=NB_AGET)
				{
					iAget=0;
					iChan+=2;
				}
				if(iChan>=NB_CHANNEL)
				{
					iBuck++;
					iChan=0;
				}
			}
		}
	}



	coboData->TreatPulseGet();	
	coboData->PerformAnalysis(hxinit, hyinit, hx, hy);	


	// store in a vector to check if there are multiple events correlated by the merger
	//TrackerCoBoEvent evt( barxm, barym, sumx, sumy, multx, multy, timestamp);
	TrackerCoBoEvent evt(
			coboData->GetBarXmProjection(),
			coboData->GetBarYmProjection(),
			coboData->GetBarZmProjection(),
			coboData->GetSumX(),
			coboData->GetSumY(),
			coboData->GetMultX(),
			coboData->GetMultY(),
			coboData->GetTimeStamp()
			);
	if(belong_to_a_merge_frame)trackerCoBoEventVec_merged.push_back(evt);
	else trackerCoBoEventVec.push_back(evt);

	//filling histos

	//if(maxy>0 && maxx>0 && maxy<3700 && maxx<3700 && multx>2 && multy>2 && sumx>10)
	//	if(sumx>1000 && sumy>1000) 
	//if(true)
	if(s1->fsaveHisto || s1->acquisitionMode.compare("ONLINE") == 0)
		//if(maxy<3700 && maxx<3700)
		//{

	{
		hxa->Reset();hya->Reset();
		for(int i=0;i<NX;i++) hxa->SetBinContent(i+1,coboData->GetAmpliX(i));	 
		for(int i=0;i<NY;i++) hya->SetBinContent(i+1,coboData->GetAmpliY(i));
		//hcoshxm->Fill(p1x*2.77);
		//hcoshym->Fill(p1y*2.79);
		//hbarcosh2Dm->Fill(p1x*2.77,p1y*2.79);

		hmaxx->Fill(coboData->GetMaxX());
		hmaxy->Fill(coboData->GetMaxY());

		hmaxx1d->Fill(coboData->GetAmpliX(coboData->GetImaxX()+1));
		hmaxx1g->Fill(coboData->GetAmpliX(coboData->GetImaxX()-1));	
		hmaxx2d->Fill(coboData->GetAmpliX(coboData->GetImaxX()+2));
		hmaxx2g->Fill(coboData->GetAmpliX(coboData->GetImaxX()-2));	

		hmaxy1d->Fill(coboData->GetAmpliY(coboData->GetImaxY()+1));
		hmaxy1g->Fill(coboData->GetAmpliY(coboData->GetImaxY()-1));	
		hmaxy2d->Fill(coboData->GetAmpliY(coboData->GetImaxY()+2));
		hmaxy2g->Fill(coboData->GetAmpliY(coboData->GetImaxY()-2));

		himaxx->Fill(coboData->GetImaxX());
		himaxy->Fill(coboData->GetImaxY());

		htmaxx->Fill(coboData->GetTmaxX());
		htmaxy->Fill(coboData->GetTmaxY());

		hmultx->Fill(coboData->GetMultX());
		hmulty->Fill(coboData->GetMultY());
		hmult_xy->Fill(coboData->GetMultX(),coboData->GetMultY());
		hsum_xy->Fill(coboData->GetSumX(),coboData->GetSumY());
		hmult_xy->Fill(coboData->GetMultX(),coboData->GetMultY());	

		hbarx->Fill(coboData->GetBarX());
		hbary->Fill(coboData->GetBarY());
		hbarxm->Fill(coboData->GetBarXm());	
		hbarym->Fill(coboData->GetBarYm());
		hbar2Dm->Fill(coboData->GetBarXm(),coboData->GetBarYm());
		hbar2DmP->Fill(coboData->GetBarXmProjection(),coboData->GetBarYmProjection());

	}


}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void GUser::UserEbyedatframeFrame(MFMCommonFrame * commonframe)
{
	fEbyedatframe->SetAttributs(frame->GetPointHeader());
	eventnumber = fEbyedatframe->GetEventNumber();
	timestamp = fEbyedatframe->GetTimeStamp();
	uint16_t label, value;


	for(int i=0;i<fEbyedatframe->GetNbItems();i++)
	{
		fEbyedatframe->EbyedatGetParameters(i,&label,&value);

	}
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void GUser::UserMutantFrame(MFMCommonFrame * commonframe){
	fMutantframe->SetAttributs(frame->GetPointHeader());
	eventnumber = fMutantframe->GetEventNumber();
	timestamp = fMutantframe->GetTimeStamp();
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Setting up Root Tree using GRU lib
/*!
 *  The pointer to the Root Ttree is accessed by calling GAcq::GetTree(). Then, the name, branches and leaves can be set up as usual.
 * GetTree()->SetName("rawDataTree");
 * GetTree()->Branch("Time", &Time, "Time/l");
 * One must call in thee GruScript.C guser->SetTTreeMode(3);
 *
 */
void GUser::InitTTreeUser()
{
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
void GUser::InitUserTTree(char* filename)
{
	//userTree->Initialize(filename, dData, tData, coboData);
	userTree->Initialize(filename, fRecoilEvent, fDecayEvent);
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

void GUser::SaveUserTTree(){
	userTree->Save();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//---------------
// Count rates
//--------------
void GUser::get_Count_Rates(int type){
	deltaT_rates = static_cast<int64_t>(timestamp - old_timeStamp);

	if(deltaT_rates >= rate_calcul_timestamp_lapse)
	{
		switch (type){
			case Dssd_Detector :
				for(UShort_t i = 0; i < s1->NSTRIPS_DSSD;i++)
				{
					if(dssd_rate_strip[i] >0){
						h_dssdStrip_rate->SetBinContent(i+1, dssd_rate_strip[i]);
						dssd_rate_strip[i] =0;
					}
				}
				// per board
				for(UShort_t b = 0; b < s1->NBOARDS_DSSD;b++)
				{
					if(dssd_rate_board[b] >0){					
						h_dssdBoard_rate->SetBinContent(b+1, dssd_rate_board[b]);
						gr_rate_dssdBoard[b]->SetPoint(rate_counterPoint_dssd[b], rate_counterPoint_dssd[b], dssd_rate_board[b]);
						rate_counterPoint_dssd[b]++;
						dssd_rate_board[b] =0;
					}
				}
				break;
			case Tunnel_Detector :
				//per pad
				for(UShort_t i = 0; i < s1->NofMacroPixels;i++)
				{
					if(tunnel_rate_pad[i] >0){					
						h_tunnelPad_rate->SetBinContent(i+1, tunnel_rate_pad[i]);
						tunnel_rate_pad[i] = 0;			
					}
				}
				// per board
				for(UShort_t b = 0; b < s1->NBOARDS_TUNNEL;b++)
				{
					if(tunnel_rate_board[b] >0){					
						h_tunnelBoard_rate->SetBinContent(b+1, tunnel_rate_board[b]);
						gr_rate_tunnelBoard[b]->SetPoint(rate_counterPoint_tunnel[b], rate_counterPoint_tunnel[b], tunnel_rate_board[b]);
						rate_counterPoint_tunnel[b]++;
						tunnel_rate_board[b] =0;				
					}
				}
				break;
			case Tracker_Detector :
				break;
			case ExoGam_Detector:
				break;
			case Veto_Detector :
				break;

		}// end of switch statement
		// reset time
		old_timeStamp = timestamp;
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

void GUser::FindCorrelations(std::string mode=""){
	if(mode.compare("Merged")==0)FindCorrelationsIn(dssdDataPointVec_merged, dssdEventVec_merged, trackerNumexo2EventVec_merged, trackerCoBoEventVec_merged);
	else if(mode.compare("Unmerged")==0)FindCorrelationsIn(dssdDataPointVec, dssdEventVec, trackerNumexo2EventVec, trackerCoBoEventVec);
	else{
		FindCorrelationsIn(dssdDataPointVec_merged, dssdEventVec_merged, trackerNumexo2EventVec_merged, trackerCoBoEventVec_merged);
		FindCorrelationsIn(dssdDataPointVec, dssdEventVec, trackerNumexo2EventVec, trackerCoBoEventVec);
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

void GUser::FindCorrelationsIn(std::vector<DssdDataPoint> &dssdDataPoints, std::vector<DssdEvent> &dssdEvents, std::vector<TrackerNumexo2Event> &trackerNumexo2Events, std::vector<TrackerCoBoEvent> &trackerCoBoEvents){


	if((dssdDataPoints.size() >= s1->buffer_size) || reached_eof){
		//Do this before making pixels as the vector is emptied afterwards
		double tof =0.;
		for(unsigned int i =0; i< dssdDataPoints.size(); i++){
			iboard =  dssdDataPoints[i].GetBoardIndex();
			channel = dssdDataPoints[i].GetChannel();
			for(unsigned int j =0; j< trackerNumexo2Events.size(); j++){
				tof = dssdDataPoints[i].GetCFDTime() - trackerNumexo2Events[j].GetCFDTime();
				if(tof > -500 && tof < 500)
					hCFDTimeDiff[iboard]->Fill(channel, tof);

			}
		}

		//----------pixel construction---------

		dssdEvents = dEvent->Construct("FB",dssdDataPoints);
		if(reached_eof){
			std::vector<DssdEvent> dssdEvents2 = dEvent->GetUnCorrelatedEvents("FB");
			dssdEvents.insert(dssdEvents.begin(), dssdEvents2.begin(), dssdEvents2.end()); 
			//sorting is done in correlations
			//cout<<"dssdPoints: "<<dssdDataPoints.size()<<"  dssdEvents: "<<dssdEvents.size()<<"  dssdEvent2: "<<dssdEvents2.size()<<"  trackerNumexo2Events: "<<trackerNumexo2Events.size()<<"  trackerCoBoEvents:  "<<trackerCoBoEvents.size()<<endl;
			dssdEvents2.clear();
		}
		//if(dssdEvents.size() > 0 && trackerCoBoEvents.size() && trackerNumexo2Events.size() >0){	
		//----------------------
		//cout<<"here 1"<<endl;
		if(!reached_eof)correlation->Find(dssdEvents, trackerNumexo2Events, trackerCoBoEvents, 0);
		else correlation->Find(dssdEvents, trackerNumexo2Events, trackerCoBoEvents, true);
		//---------------------
		// Decays
		//-----------------
		decayTypeEvents = correlation->GetDecays();
		recoilTypeEvents = correlation->GetRecoils();

		//cout<<"here 2"<<endl;
		if(decayTypeEvents.size() >0){
			for (std::vector<DecayEvent>::iterator decay = decayTypeEvents.begin() ; decay != decayTypeEvents.end(); ++decay){				

				//----------do something here----------------
				fDecayEvent = (*decay);
				userTree->FillDecayTTree();
				//----------plot histograms----------------
				if(s1->fsaveHisto || s1->acquisitionMode.compare("ONLINE") == 0){
					h_E_frontBack->Fill((*decay).GetDssdEvent().GetEnergyX(), (*decay).GetDssdEvent().GetEnergyY());
					h_DSSD_XY_hit->Fill((*decay).GetDssdEvent().GetPixel().GetX(), (*decay).GetDssdEvent().GetPixel().GetY());
				}
			}
		}
		//----------------
		// Recoils
		//---------------				
		//cout<<"here 3"<<endl;
		if(correlation->GetRecoils().size()>0){
			z2 = 100.;//distance in cm
			for ( std::vector<RecoilEvent>::iterator recoil = recoilTypeEvents.begin() ; recoil != recoilTypeEvents.end(); ++recoil){				
				//-----------------
				// Trajectory
				//---------------
				//position 1 : at the SED x, y, z// here they are the projection of 45 degree foil on the XY plane perpendicular to the beam axis (Z) 
				if((*recoil).HasACoBoEvent() && (*recoil).HasATrackerNumexo2Event() && 
						(*recoil).GetDssdEvent().GetEnergyX() > 100. && (*recoil).GetDssdEvent().GetEnergyY() > 100. ){

					x1 =(*recoil).GetTrackerCoBoEvent().GetBaryCenterXm()/10.;
					y1 =(*recoil).GetTrackerCoBoEvent().GetBaryCenterYm()/10.;
					z1 =(*recoil).GetTrackerCoBoEvent().GetBaryCenterZm()/10.;
					//position 2 : at the DSSD
					x2 = (((*recoil).GetDssdEvent().GetPixel().GetX() + 1.) *(10./128.)) - 5.;
					y2 = (((*recoil).GetDssdEvent().GetPixel().GetY() + 1.) *(10./128.)) - 5.;

					x21 = x2 - x1;
					y21 = y2 - y1;
					z21 = z2 - z1;

					distance = TMath::Sqrt(x21*x21 + y21*y21 + z21*z21);
					slopeXZ = x21/z21;
					slopeYZ = y21/z21;
					//----------plot histograms----------------
					if(s1->fsaveHisto || s1->acquisitionMode.compare("ONLINE") == 0){
						h_XDistance->Fill(x1, distance);
						h_YDistance->Fill(y1, distance);

						hbar2DmPRecoil->Fill((*recoil).GetTrackerCoBoEvent().GetBaryCenterXm(), (*recoil).GetTrackerCoBoEvent().GetBaryCenterYm() );
						h_DSSD_XYPhysical_hit_recoil->Fill(x2,y2);
						h_DSSD_XY_hit_recoil->Fill((*recoil).GetDssdEvent().GetPixel().GetX(), (*recoil).GetDssdEvent().GetPixel().GetY());


						//-------------track fill part starts here--------
						for(double z = z1; z <= z2; z += 1.){
							x = (z -z1)*slopeXZ + x1;
							y = (z -z1)*slopeYZ + y1;
							h_trackZSeDX->Fill(z,x);
							h_trackZSeDY->Fill(z,y);
					//		h_trackXYZ->Fill(z, x, y);
						}
						//-------------track fill part ends here--------
						fRecoilEvent = (*recoil);
						userTree->FillRecoilTTree();
						//--------------------------
						// ToF infor from Numexo2 SeD
						//-------------------------
						h_FrontE_ToFNumexo->Fill((*recoil).GetDssdEvent().GetEnergyX(), (*recoil).GetToFNumexo2());
						h_BackE_ToFNumexo->Fill((*recoil).GetDssdEvent().GetEnergyY(), (*recoil).GetToFNumexo2());
						h_stripX_ToFNumexo->Fill((*recoil).GetDssdEvent().GetPixel().GetX(), (*recoil).GetToFNumexo2());
						h_stripY_ToFNumexo->Fill((*recoil).GetDssdEvent().GetPixel().GetY(), (*recoil).GetToFNumexo2());

						h_Efront_Esed->Fill((*recoil).GetDssdEvent().GetEnergyX(), (*recoil).GetTrackerNumexo2Event().GetEnergy());
						h_Eback_Esed->Fill((*recoil).GetDssdEvent().GetEnergyY(), (*recoil).GetTrackerNumexo2Event().GetEnergy());
						h_FrontE_CFDToFNumexo->Fill((*recoil).GetDssdEvent().GetEnergyX(), (*recoil).GetCFDToFNumexo2());
						h_BackE_CFDToFNumexo->Fill((*recoil).GetDssdEvent().GetEnergyY(), (*recoil).GetCFDToFNumexo2());
						h_stripX_CFDToFNumexo->Fill((*recoil).GetDssdEvent().GetPixel().GetX(), (*recoil).GetCFDToFNumexo2());
						h_stripY_CFDToFNumexo->Fill((*recoil).GetDssdEvent().GetPixel().GetY(), (*recoil).GetCFDToFNumexo2());
						h_stripX_Esed->Fill((*recoil).GetDssdEvent().GetPixel().GetX(), (*recoil).GetTrackerNumexo2Event().GetEnergy());
						h_stripY_Esed->Fill((*recoil).GetDssdEvent().GetPixel().GetY(), (*recoil).GetTrackerNumexo2Event().GetEnergy());




						//-------------------
						// ToF from CoBo
						// ----------------
						h_FrontE_ToFCobo->Fill((*recoil).GetDssdEvent().GetEnergyX(), (*recoil).GetToFCoBo());
						h_BackE_ToFCobo->Fill((*recoil).GetDssdEvent().GetEnergyY(), (*recoil).GetToFCoBo());
						h_stripX_ToFCobo->Fill((*recoil).GetDssdEvent().GetPixel().GetX(), (*recoil).GetToFCoBo());
						h_stripY_ToFCobo->Fill((*recoil).GetDssdEvent().GetPixel().GetY(), (*recoil).GetToFCoBo());
						//
						h_E_frontBack->Fill((*recoil).GetDssdEvent().GetEnergyX(), (*recoil).GetDssdEvent().GetEnergyY());
						h_DSSD_XY_hit->Fill((*recoil).GetDssdEvent().GetPixel().GetX(), (*recoil).GetDssdEvent().GetPixel().GetY());
					}
				}
			}				
			//}

			//-------------
			// Clear memory
			//-------------
			dssdDataPoints.clear();
			dssdEvents.clear();
			trackerNumexo2Events.clear();
			trackerCoBoEvents.clear();
	}
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

void GUser::PrintInfo(MFMCommonFrame *commonframe){
	switch (s1->fverbose)
	{
		case 1 :
			commonframe->HeaderDisplay();
			break;
		case 2 :
			commonframe->HeaderDisplay();
			if (framesize < dumpsize1) commonframe->DumpRaw(framesize, 0);
			else commonframe->DumpRaw(dumpsize1, 0);
			break;
		case 3 :
			commonframe->HeaderDisplay();
			if (framesize < dumpsize2) commonframe->DumpRaw(framesize, 0);
			else commonframe->DumpRaw(dumpsize2, 0);
			break;
		case 4 :
			commonframe->HeaderDisplay();
			if (framesize < dumpsize3) commonframe->DumpRaw(framesize, 0);
			else commonframe->DumpRaw(dumpsize3, 0);
			break;
		case 5 :
			commonframe->HeaderDisplay();
			if (framesize < dumpsize4) commonframe->DumpRaw(framesize, 0);
			else commonframe->DumpRaw(dumpsize4, 0);
			break;
		case 6 :
			commonframe->HeaderDisplay();
			if (framesize < dumpsize5) commonframe->DumpRaw(framesize, 0);
			else commonframe->DumpRaw(dumpsize5, 0);
			break;
		case 7 :
			commonframe->HeaderDisplay();
			if (framesize < dumpsize6) commonframe->DumpRaw(framesize, 0);
			else commonframe->DumpRaw(dumpsize6, 0);
			break;
		case 8 :
			commonframe->HeaderDisplay();
			if (framesize < dumpsize7) commonframe->DumpRaw(framesize, 0);
			else commonframe->DumpRaw(dumpsize7, 0);
			break;
		case 9 :
			commonframe->HeaderDisplay();
			if (framesize < dumpsize8) commonframe->DumpRaw(framesize, 0);
			else commonframe->DumpRaw(dumpsize8, 0);
			break;
		case 10 :
			commonframe->HeaderDisplay();
			commonframe->DumpRaw(framesize, 0);
			break;
		default:
			break;
	}
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
