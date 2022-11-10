/*!
 * \file GUser.C
 * \author Luc Legeard
 * \brief  Class for User treatment
 * \details modified by Rikel CHAKMA
 */
#include "./GUser.h"
#include "TROOT.h"
//#include <TProfile.h>
#include "GEventMFM.h"

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
		std::cout<<"SIGINT signal = "<<signal_number<<" received!!!"<<std::endl;
	else if(signal_number == SIGTERM)
		std::cout<<"SIGTERM signal = "<<signal_number<<" received!!!"<<std::endl;

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
GUser::GUser (GDevice* _fDevIn, GDevice* _fDevOut):GAcq(_fDevIn,_fDevOut)
{
	s1                             = myGlobal::getInstance();
	fCoboframe                     = new MFMCoboFrame();
	fInsideframe                   = new MFMCommonFrame();
	fMergeframe                    = new MFMMergeFrame();
	fSiriusframe                   = new MFMSiriusFrame();
	fGenericframe                  = new MFMReaGenericFrame();
	filter                         = new digitalFilters();
	cfd                            = new digitalCFD();
	dData                          = new dssdData();
	dEvent                         = new dssdEvent();
	tData                          = new tunnelData();
	calib                          = new calibration();
	frameCounter                   = new ullint[8];
	tunnel_rate_pad                = new double[s1->NofMacroPixels];
	tunnel_rate_board              = new double[s1->NBOARDS_TUNNEL];
	dssd_rate_strip                = new double[s1->NSTRIPS_DSSD];
	dssd_rate_board                = new double[s1->NBOARDS_DSSD]; 
	rate_counterPoint_dssd         = new ullint[s1->NBOARDS_DSSD];
	rate_counterPoint_tunnel       = new ullint[s1->NBOARDS_TUNNEL];
	dssd_event_counter             = new ullint*[s1->NBOARDS_DSSD];
	dumpsize                       = 0;
	framesize                      = 0;
	maxdump                        = 128;
	channel                        = 0;
	board                          = 0;
	iboard                         = 0;
	value                          = 0;
	trace_size                     = 0;
	rate_counter_dssd              = 0;
	rate_counter_tunnel            = 0;
	dataSet_counter                = 0.;
	rate_calcul_timestamp_lapse    = static_cast<int64_t>(s1->rate_calcul_time_lapse *pow(10,8));
	//--------------
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
		dssd_event_counter[i] = new unsigned long long int[NCHANNELS];
	}
	//initialize some preset value
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			dssd_event_counter[i][j] =0;
		}
	}
	std::string dssd_boards_char[s1->NBOARDS_DSSD];
	for(int i = 0; i < s1->NBOARDS_DSSD;i++) dssd_boards_char[i] = std::to_string(s1->boardList_DSSD[i]);
	std::string tunnel_boards_char[s1->NBOARDS_TUNNEL];
	for(int i = 0; i < s1->NBOARDS_TUNNEL;i++) tunnel_boards_char[i] = std::to_string(s1->boardList_Tunnel[i]);

	TString name;
	TString famname;
	TString sfamname;
	//hGain        = new TH1I**[NBOARDS];
	//hFeedBack    = new TH1I**[NBOARDS];
	hTrace_Sum     = new TH2F**[s1->NBOARDS_DSSD];
	hRaw           = new TH1F**[s1->NBOARDS_DSSD];
	hCalib         = new TH1F**[s1->NBOARDS_DSSD];
	hTrap          = new TH2F**[s1->NBOARDS_DSSD];
	gr_baseline    = new TGraph**[s1->NBOARDS_DSSD];
	hBaseline      = new TH1F**[s1->NBOARDS_DSSD];
	hNoise         = new TH1F**[s1->NBOARDS_DSSD];
	hRisetime      = new TH1I**[s1->NBOARDS_DSSD];
	for(int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
		//hGain[iboard]       = new TH1I*[NUMEXO_NB_CHANNELS];
		//hFeedBack[iboard]   = new TH1I*[NUMEXO_NB_CHANNELS];
		hTrace_Sum[iboard]    = new TH2F*[NUMEXO_NB_CHANNELS];
		hRaw[iboard]          = new TH1F*[NUMEXO_NB_CHANNELS];
		hCalib[iboard]        = new TH1F*[NUMEXO_NB_CHANNELS];
		hTrap[iboard]         = new TH2F*[NUMEXO_NB_CHANNELS];
		gr_baseline[iboard]   = new TGraph*[NUMEXO_NB_CHANNELS];
		hBaseline[iboard]     = new TH1F*[NUMEXO_NB_CHANNELS];
		hNoise[iboard]        = new TH1F*[NUMEXO_NB_CHANNELS];
		hRisetime[iboard]     = new TH1I*[NUMEXO_NB_CHANNELS];
		//
		famname.Form("Card_%d",s1->boardList_DSSD[iboard]);
		for (int channel =0;channel <NUMEXO_NB_CHANNELS;channel++){

			/*  name.Form("Gain_%d_%d",boardList[iboard],channel);
			    hGain[iboard][channel] = new TH1I (name.Data(),name.Data(),sizehisto,0,maxhisto);
			    GetSpectra()->AddSpectrum(hGain[iboard][channel],famname);

			    name.Form("FeedBack_%d_%d",boardList[iboard],channel);
			    hFeedBack[iboard][channel] = new TH1I (name.Data(),name.Data(),sizehisto,0,maxhisto);
			    GetSpectra()->AddSpectrum(hFeedBack[iboard][channel],famname);
			    */
			//histogram for viewing traces
			sfamname.Form("%s/Trace",famname.Data());
			name.Form("Trace_Sum%d_%d",s1->boardList_DSSD[iboard],channel);
			hTrace_Sum[iboard][channel] = (TH2F*)gROOT->Get(name);
			if(!hTrace_Sum[iboard][channel]) hTrace_Sum[iboard][channel] = new TH2F (name.Data(),name.Data(),s1->TRACE_SIZE,0,s1->TRACE_SIZE,1700,-1000,16000);
			else hTrace_Sum[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hTrace_Sum[iboard][channel],sfamname);
			//histograms for raw data = trapezodal height
			sfamname.Form("%s/RawHist",famname.Data());
			name.Form("RawData_%d_%d",s1->boardList_DSSD[iboard],channel);
			hRaw[iboard][channel] = (TH1F*)gROOT->Get(name);
			if(!hRaw[iboard][channel]) hRaw[iboard][channel] = new TH1F(name.Data(), name.Data(),2000,0.,20000.);
			else hRaw[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hRaw[iboard][channel],sfamname);
			//histograms for calibrated data = trapezodal height * gain + offset
			sfamname.Form("%s/CalibHist",famname.Data());
			name.Form("CalibData_%d_%d",s1->boardList_DSSD[iboard],channel);
			hCalib[iboard][channel] = (TH1F*)gROOT->Get(name);
			if(!hCalib[iboard][channel]) hCalib[iboard][channel] = new TH1F(name.Data(), name.Data(),2000,0.,10000.);
			else hCalib[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hCalib[iboard][channel],sfamname);

			//histo for trapezoidal
			sfamname.Form("%s/Trapezoidal",famname.Data());
			name.Form("Trap_%d_%d",s1->boardList_DSSD[iboard],channel);
			hTrap[iboard][channel] = (TH2F*) gROOT->Get(name);
			if(!hTrap[iboard][channel]) hTrap[iboard][channel] = new TH2F(name.Data(),name.Data(), s1->TRACE_SIZE, 0,s1-> TRACE_SIZE, 4000,-8000,8000);
			else hTrap[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hTrap[iboard][channel],sfamname);

			//histo for baseline
			sfamname.Form("%s/BaselineHist",famname.Data());
			name.Form("hBaseline_%d_%d",s1->boardList_DSSD[iboard],channel);
			hBaseline[iboard][channel] = (TH1F*) gROOT->Get(name);
			if(!hBaseline[iboard][channel]) hBaseline[iboard][channel] = new TH1F(name.Data(),name.Data(), 4000,0,16000);
			else hBaseline[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hBaseline[iboard][channel],sfamname);

			//baseline as a function of event number
			sfamname.Form("%s/BaselineGraph",famname.Data());
			name.Form("Baseline_%d_%d",s1->boardList_DSSD[iboard],channel);
			gr_baseline[iboard][channel] = (TGraph*)gROOT->Get(name);
			if(!gr_baseline[iboard][channel]) {gr_baseline[iboard][channel] =  new TGraph();
				name.Form("baseline_%d_%d",s1->boardList_DSSD[iboard],channel);
				gr_baseline[iboard][channel]->SetNameTitle(name,name);
			}
			else gr_baseline[iboard][channel]->Set(0);
			//gr_baseline[iboard][channel]->GetXaxis()->SetTitle("event number");
			//gr_baseline[iboard][channel]->GetYaxis()->SetTitle("baseline");
			GetSpectra()->AddSpectrum( gr_baseline[iboard][channel], sfamname);

			//histo for Noise
			sfamname.Form("%s/Noise",famname.Data());
			name.Form("hNoise%d_%d",s1->boardList_DSSD[iboard],channel);
			hNoise[iboard][channel] = (TH1F*) gROOT->Get(name);
			if(!hNoise[iboard][channel]) hNoise[iboard][channel] = new TH1F(name.Data(),name.Data(), 1000,0,100);
			else hNoise[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hNoise[iboard][channel],sfamname);
			//histo for Risetime
			sfamname.Form("%s/Risetime",famname.Data());
			name.Form("hRisetime%d_%d",s1->boardList_DSSD[iboard],channel);
			hRisetime[iboard][channel] = (TH1I*) gROOT->Get(name);
			if(!hRisetime[iboard][channel]) hRisetime[iboard][channel] = new TH1I(name.Data(),name.Data(), 1000,0,1000);
			else hRisetime[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hRisetime[iboard][channel],sfamname);
		}
	}
	//
	famname.Form("DssdSpectra");
	//- time difference b/w 2 front strips
	h_delT_ff = (TH1I*)gROOT->Get("h_delT_ff");
	if(!h_delT_ff) h_delT_ff =  new TH1I("h_delT_ff","#Delta T(front1-front2);ns", 1000,-50000,50000);
	else h_delT_ff->Reset();
	GetSpectra()->AddSpectrum(h_delT_ff,famname);
	//- time difference b/w  front and back strips
	h_delT_fb = (TH1I*)gROOT->Get("h_delT_fb");
	if(!h_delT_fb) h_delT_fb =  new TH1I("h_delT_fb","#Delta T(front - back);ns",1000,-50000,50000);
	else h_delT_fb->Reset();
	GetSpectra()->AddSpectrum(h_delT_fb,famname);
	//- time difference b/w back and front strips
	h_delT_bf = (TH1I*)gROOT->Get("h_delT_bf");
	if(!h_delT_bf) h_delT_bf =  new TH1I("h_delT_bf","#Delta T(back - front);ns",1000,-50000,50000);
	else  h_delT_bf->Reset();
	GetSpectra()->AddSpectrum(h_delT_bf,famname);
	//- time difference b/w 2 back strips
	h_delT_bb = (TH1I*)gROOT->Get("h_delT_bb");
	if(!h_delT_bb) h_delT_bb =  new TH1I("h_delT_bb","#Delta T(back1 - back2);ns",1000,-50000,50000);
	else h_delT_bb->Reset();
	GetSpectra()->AddSpectrum(h_delT_bb,famname);
	//-raw energy vs strip number
	h_raw_strip = (TH2F*)gROOT->Get("h_raw_strip");
	if(!h_raw_strip) h_raw_strip = new TH2F("h_raw_strip",";E (ADC ch);strip number", 2000,0,10000,256,0,256);
	else h_raw_strip->Reset();
	GetSpectra()->AddSpectrum( h_raw_strip,famname);
	//-calibrated energy vs strip number
	h_calib_strip = (TH2F*)gROOT->Get("h_calib_strip");
	if(! h_calib_strip)h_calib_strip = new TH2F("h_calib_strip",";E (keV);strip number", 2000,0,10000,256,0,256);
	else  h_calib_strip->Reset();
	GetSpectra()->AddSpectrum( h_calib_strip,famname);
	//-histo for front energy vs back energy
	h_E_frontBack = (TH2F*)gROOT->Get("h_E_frontBack");
	if(!h_E_frontBack)h_E_frontBack = new TH2F("h_E_frontBack",";frontE;backE",1000,0,10000,1000,0,10000);
	else h_E_frontBack->Reset();
	GetSpectra()->AddSpectrum( h_E_frontBack,famname);
	//-hit pattern in the DSSD
	h_DSSD_XY_hit = (TH2I*)gROOT->Get("h_DSSD_XY_hit");
	if(!h_DSSD_XY_hit) h_DSSD_XY_hit =  new TH2I("h_DSSD_XY_hit","hit pattern ;X;Y",128,0,128,128,0,128);
	else h_DSSD_XY_hit->Reset();
	GetSpectra()->AddSpectrum( h_DSSD_XY_hit,famname);
	//-histo for viewing number of counts in each strip
	h_dssd_count_strip = (TH1I*)gROOT->Get("h_dssd_count_strip");
	if(!h_dssd_count_strip) h_dssd_count_strip = new TH1I("h_dssd_count_strip",";strip number; counts", s1->NSTRIPS_DSSD,0,s1->NSTRIPS_DSSD);
	else h_dssd_count_strip->Reset();
	GetSpectra()->AddSpectrum( h_dssd_count_strip,famname);
	//histo for viewing counts per board
	h_dssd_count_board = (TH1I*)gROOT->Get("h_dssd_count_board");
	if(!h_dssd_count_board) h_dssd_count_board = new TH1I("h_dssd_count_board",";board number; counts", s1->NBOARDS_DSSD,0,s1->NBOARDS_DSSD);
	else h_dssd_count_board->Reset();
	for(int i = 1; i <= s1->NBOARDS_DSSD; i++)h_dssd_count_board->GetXaxis()->SetBinLabel(i, dssd_boards_char[i-1].c_str());
	//	h_dssd_count_board->GetXaxis()->SetLabelSize(0.1);
	GetSpectra()->AddSpectrum( h_dssd_count_board,famname);

	//-event rates in sec in each strip of the DSSD
	h_dssdStrip_rate = (TH1I*)gROOT->Get("h_dssdStrip_rate");
	if(!h_dssdStrip_rate) h_dssdStrip_rate = new TH1I("h_dssdStrip_rate",Form(";strip number;counts/%d sec",s1->rate_calcul_time_lapse), s1->NSTRIPS_DSSD,0,s1->NSTRIPS_DSSD);
	else h_dssdStrip_rate->Reset();
	GetSpectra()->AddSpectrum( h_dssdStrip_rate,famname);
	//-event rates in sec in each Board of the DSSD
	h_dssdBoard_rate = (TH1I*)gROOT->Get("h_dssdBoard_rate");
	if(!h_dssdBoard_rate) h_dssdBoard_rate = new TH1I("h_dssdBoard_rate",Form(";board number;counts/%d sec",s1->rate_calcul_time_lapse), s1->NBOARDS_DSSD,0,s1->NBOARDS_DSSD);
	else h_dssdBoard_rate->Reset();
	for(int i = 1; i <= s1->NBOARDS_DSSD; i++)h_dssdBoard_rate->GetXaxis()->SetBinLabel(i, dssd_boards_char[i-1].c_str());
	//h_dssdBoard_rate->GetXaxis()->SetLabelSize(0.1);	
	GetSpectra()->AddSpectrum( h_dssdBoard_rate,famname);
	//Rates per board graphs
	sfamname.Form("%s/RatePerBoard",famname.Data());
	gr_rate_dssdBoard = new TGraph*[s1->NBOARDS_DSSD];
	for(int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
		sfamname.Form("%s/RatePerBoard",famname.Data());
		name.Form("rate_%d",s1->boardList_DSSD[iboard]);
		gr_rate_dssdBoard[iboard] = (TGraph*)gROOT->Get(name);
		if(!gr_rate_dssdBoard[iboard]) {
			gr_rate_dssdBoard[iboard] =  new TGraph();
			gr_rate_dssdBoard[iboard]->SetName(name);
			gr_rate_dssdBoard[iboard]->SetTitle(Form("Rate %d; time(x %d sec);counts/%d sec", s1->boardList_DSSD[iboard], s1->rate_calcul_time_lapse,s1->rate_calcul_time_lapse));
		}
		else gr_rate_dssdBoard[iboard]->Set(0);
		GetSpectra()->AddSpectrum(gr_rate_dssdBoard[iboard],sfamname);
	}	

	famname.Form("CorrelationSpectra");
	//Tof
	h_dssdE_Tof = (TH2F*)gROOT->Get("h_dssdE_Tof");
	if(!h_dssdE_Tof) h_dssdE_Tof = new TH2F("h_dssdE_Tof",";E;ToF", 5000,0,30000, 1000,0,20000);
	else h_dssdE_Tof->Reset();
	GetSpectra()->AddSpectrum( h_dssdE_Tof,famname);

	//---------tunnel
	h_tunnelRaw = new TH1I**[s1->NBOARDS_TUNNEL];
	h_tunnelCalib = new TH1I**[s1->NBOARDS_TUNNEL];
	for(int iboard = 0;iboard < s1->NBOARDS_TUNNEL;iboard++){
		h_tunnelRaw[iboard]    = new TH1I*[NUMEXO_NB_CHANNELS];
		h_tunnelCalib[iboard]    = new TH1I*[NUMEXO_NB_CHANNELS];
		famname.Form("Card_%d",s1->boardList_Tunnel[iboard]);
		for (int channel =0;channel <NUMEXO_NB_CHANNELS;channel++){
			//Raw data
			sfamname.Form("%s/RawHist",famname.Data());
			name.Form("RawData_%d_%d",s1->boardList_Tunnel[iboard], channel);
			h_tunnelRaw[iboard][channel] = (TH1I*)gROOT->Get(name);
			if(!h_tunnelRaw[iboard][channel]) h_tunnelRaw[iboard][channel] = new TH1I (name.Data(),name.Data(),8000,0,8000);
			else h_tunnelRaw[iboard][channel]->Reset(); 
			GetSpectra()->AddSpectrum(h_tunnelRaw[iboard][channel],sfamname);
			//Calib data
			sfamname.Form("%s/CalibHist",famname.Data());
			name.Form("CalibData_%d_%d",s1->boardList_Tunnel[iboard], channel);
			h_tunnelCalib[iboard][channel] = (TH1I*)gROOT->Get(name);
			if(!h_tunnelCalib[iboard][channel]) h_tunnelCalib[iboard][channel] = new TH1I (name.Data(),name.Data(),8000,0,8000);
			else h_tunnelCalib[iboard][channel]->Reset(); 
			GetSpectra()->AddSpectrum(h_tunnelCalib[iboard][channel],sfamname);

		}
	}
	famname.Form("TunnelSpectra");
	//-raw energy vs strip number
	h_raw_pad = (TH2F*)gROOT->Get("h_raw_pad");
	if(!h_raw_pad) h_raw_pad = new TH2F("h_raw_pad",";E (ADC ch);pad number", 2000,0,10000,96,0,96);
	else h_raw_pad->Reset();
	GetSpectra()->AddSpectrum( h_raw_pad,famname);
	//-calibrated energy vs strip number
	h_calib_pad = (TH2F*)gROOT->Get("h_calib_pad");
	if(! h_calib_pad)h_calib_pad = new TH2F("h_calib_pad",";E (keV);pad number", 2000,0,10000,96,0,96);
	else  h_calib_pad->Reset();
	GetSpectra()->AddSpectrum( h_calib_pad,famname);
	//-hit pattern in the TUNNEL

	h_TUNNEL_XY_hit = new TH2I*[s1->NDETECTOR_TUNNEL];
	for(int i =0; i < s1->NDETECTOR_TUNNEL;i++){
		name.Form("h_tunnel%d_XY_hit",i+1);
		h_TUNNEL_XY_hit[i] = (TH2I*)gROOT->Get(name);
		if(!h_TUNNEL_XY_hit[i]) h_TUNNEL_XY_hit[i] =  new TH2I(name,Form("Tunnel %d;X;Y",i+1),8,0,8,8,0,8);
		else h_TUNNEL_XY_hit[i]->Reset();
		GetSpectra()->AddSpectrum( h_TUNNEL_XY_hit[i],famname);
	}
	//-histo for viewing number of counts in each pad
	h_tunnel_count_pad = (TH1I*)gROOT->Get("h_tunnel_count_pad");
	if(!h_tunnel_count_pad) h_tunnel_count_pad = new TH1I("h_tunnel_count_pad",";pad number; counts", s1->NofMacroPixels,0,s1->NofMacroPixels);
	else h_tunnel_count_pad->Reset();
	GetSpectra()->AddSpectrum( h_tunnel_count_pad,famname);
	//histo for viewing counts per board
	h_tunnel_count_board = (TH1I*)gROOT->Get("h_tunnel_count_board");
	if(!h_tunnel_count_board) h_tunnel_count_board = new TH1I("h_tunnel_count_board",";board number; counts", s1->NBOARDS_TUNNEL,0,s1->NBOARDS_TUNNEL);
	else h_tunnel_count_board->Reset();
	for(int i = 1; i <= s1->NBOARDS_TUNNEL; i++)h_tunnel_count_board->GetXaxis()->SetBinLabel(i, tunnel_boards_char[i-1].c_str());
	//	h_tunnel_count_board->GetXaxis()->SetLabelSize(0.1);
	GetSpectra()->AddSpectrum( h_tunnel_count_board,famname);


	//-event rates in sec in each strip of the DSSD
	h_tunnelPad_rate = (TH1I*)gROOT->Get("h_tunnelPad_rate");
	if(!h_tunnelPad_rate) h_tunnelPad_rate = new TH1I("h_tunnelPad_rate",Form(";pad number;counts/%d sec",s1->rate_calcul_time_lapse), s1->NofMacroPixels,0,s1->NofMacroPixels);
	else h_tunnelPad_rate->Reset();
	GetSpectra()->AddSpectrum( h_tunnelPad_rate,famname);

	//-event rates in sec in each Board of the DSSD
	h_tunnelBoard_rate = (TH1I*)gROOT->Get("h_tunnelBoard_rate");
	if(!h_tunnelBoard_rate) h_tunnelBoard_rate = new TH1I("h_tunnelBoard_rate",Form(";board number;counts/%d sec",s1->rate_calcul_time_lapse), s1->NBOARDS_TUNNEL,0,s1->NBOARDS_TUNNEL);
	else h_tunnelBoard_rate->Reset();
	for(int i = 1; i <= s1->NBOARDS_TUNNEL; i++)h_tunnelBoard_rate->GetXaxis()->SetBinLabel(i, tunnel_boards_char[i-1].c_str());
	//h_tunnelBoard_rate->GetXaxis()->SetLabelSize(0.1);	
	GetSpectra()->AddSpectrum( h_tunnelBoard_rate,famname);
	//Rate per board graphs
	gr_rate_tunnelBoard = new TGraph*[s1->NBOARDS_TUNNEL];
	sfamname.Form("%s/RatePerBoard",famname.Data());
	for(int iboard = 0;iboard <s1->NBOARDS_TUNNEL;iboard++){
		name.Form("rate_%d",s1->boardList_Tunnel[iboard]);
		gr_rate_tunnelBoard[iboard] = (TGraph*)gROOT->Get(name);
		if(!gr_rate_tunnelBoard[iboard]) {
			gr_rate_tunnelBoard[iboard] =  new TGraph();
			gr_rate_tunnelBoard[iboard]->SetName(name);
			gr_rate_tunnelBoard[iboard]->SetTitle(Form("Rate %d; time(x %d sec);counts/%d sec", s1->boardList_Tunnel[iboard], s1->rate_calcul_time_lapse,s1->rate_calcul_time_lapse));
		}
		else gr_rate_tunnelBoard[iboard]->Set(0);
		GetSpectra()->AddSpectrum(gr_rate_tunnelBoard[iboard],sfamname);
	}

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
	  */	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		  delete [] dssd_event_counter[i];
	  }
	  delete [] dssd_event_counter;
	  //-----------------------
	  //delete histogram pointers
	  //------------------------
	  for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		  //  delete [] hGain[i];
		  //delete [] hFeedBack[i];
		  delete [] hTrace_Sum[i];
		  delete [] hRaw[i];
		  delete [] hCalib[i];
		  delete [] hTrap[i];
		  delete [] gr_baseline[i];
		  delete [] hBaseline[i];
		  delete [] hNoise[i];
		  delete [] hRisetime[i];
	  }
	  //delete [] hGain;
	  //delete [] hFeedBack;
	  delete [] hTrace_Sum;
	  delete [] hRaw;
	  delete [] hCalib;
	  delete [] hTrap;
	  delete [] hBaseline;
	  delete [] hNoise;
	  delete [] hRisetime;
	  delete [] gr_baseline;
	  delete [] gr_rate_dssdBoard;
	  delete [] gr_rate_tunnelBoard;
	  for(int i = 0; i < s1->NBOARDS_TUNNEL; i++){
		  delete [] h_tunnelRaw[i];
		  delete [] h_tunnelCalib[i];
	  }
	  delete [] h_tunnelRaw;
	  delete [] h_tunnelCalib;

	  dssdDataVec.clear();
	  dssdEventVec.clear();
	  delete [] h_TUNNEL_XY_hit;
	  delete [] tunnel_rate_pad;
	  delete [] tunnel_rate_board;
	  delete [] frameCounter;
	  delete [] dssd_rate_strip;
	  delete [] dssd_rate_board;
	  delete [] rate_counterPoint_dssd;
	  delete [] rate_counterPoint_tunnel;


	  if(fInsideframe) delete fInsideframe;
	  if(fMergeframe) delete fMergeframe;
	  if(fSiriusframe) delete fSiriusframe;
	  if(fGenericframe) delete fGenericframe;
	  if(fCoboframe) delete fCoboframe;
	  if(filter) delete filter;
	  if(cfd) delete cfd;
	  if(dEvent) delete dEvent;
	  if(calib) delete calib;
	  if(tData) delete tData;
	  if(dData) delete dData;

	  cout<<"GUser Desctructor called"<<endl;
	  gROOT->cd();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Initialisation for global  user treatement
/*! The variables and histograms are initialized at the start.
*/
void GUser::InitUser()
{
	for(int i = 0; i < 8;i++)frameCounter[i] =0;
	rate_counter_dssd=0;
	rate_counter_tunnel=0;
	dataSet_counter = 0.;
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
	for(int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
		for (int channel =0;channel <NUMEXO_NB_CHANNELS;channel++){
			if(hTrace_Sum[iboard][channel]) hTrace_Sum[iboard][channel]->Reset();
			if(hRaw[iboard][channel]) hRaw[iboard][channel]->Reset();
			if(hCalib[iboard][channel]) hCalib[iboard][channel]->Reset();
			if(hTrap[iboard][channel]) hTrap[iboard][channel]->Reset();
			if(gr_baseline[iboard][channel]) gr_baseline[iboard][channel]->Set(0);
		}
	}
	if(h_delT_ff) h_delT_ff->Reset();
	if(h_delT_fb) h_delT_fb->Reset();
	if(h_delT_bf) h_delT_bf->Reset();
	if(h_delT_bb) h_delT_bb->Reset();
	if(h_raw_strip) h_raw_strip->Reset();
	if(h_calib_strip) h_calib_strip->Reset();
	if(h_E_frontBack) h_E_frontBack->Reset();
	if(h_DSSD_XY_hit) h_DSSD_XY_hit->Reset();
	if(h_dssd_count_strip) h_dssd_count_strip->Reset();
	if(h_dssd_count_board) h_dssd_count_board->Reset();
	if(h_dssdStrip_rate) h_dssdStrip_rate->Reset();
	if(h_dssdBoard_rate) h_dssdBoard_rate->Reset();
	for(int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
		if(gr_rate_dssdBoard[iboard]) gr_rate_dssdBoard[iboard]->Set(0);
	}	
	if(h_dssdE_Tof) h_dssdE_Tof->Reset();
	for(int iboard = 0;iboard < s1->NBOARDS_TUNNEL;iboard++){
		for (int channel =0;channel <NUMEXO_NB_CHANNELS;channel++){
			if(h_tunnelRaw[iboard][channel]) h_tunnelRaw[iboard][channel]->Reset(); 
			if(h_tunnelCalib[iboard][channel]) h_tunnelCalib[iboard][channel]->Reset(); 
		}
	}
	if(h_raw_pad) h_raw_pad->Reset();
	if( h_calib_pad) h_calib_pad->Reset();
	for(int i =0; i < s1->NDETECTOR_TUNNEL;i++){
		if(h_TUNNEL_XY_hit[i]) h_TUNNEL_XY_hit[i]->Reset();
	}
	if(h_tunnelPad_rate)h_tunnelPad_rate->Reset();

	if(h_tunnelBoard_rate) h_tunnelBoard_rate->Reset();
	for(int iboard = 0;iboard <s1->NBOARDS_TUNNEL;iboard++){
		if(gr_rate_tunnelBoard[iboard])  gr_rate_tunnelBoard[iboard]->Set(0);
	}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Initializarion for each run
/*! If you want to reset some variables or histograms at the start of each file.
*/
void GUser::InitUserRun()
{
	/*dataSet_counter = 0.;
	  rate_counter_dssd=0;
	  rate_counter_tunnel=0;
	  for(int i =0; i< 256;i++){
	  dssd_rate_strip[i] =0.;
	  }
	  for(int i =0; i< 16;i++){
	  rate_counterPoint_dssd[i]=0;	
	  dssd_rate_board[i] =0.;
	  }
	  for(int i =0; i< 24;i++){
	  tunnel_rate_pad[i] =0.;
	  }
	  for(int i =0; i< 2;i++){
	  rate_counterPoint_tunnel[i]=0;	
	  tunnel_rate_board[i] =0.;
	  }

	  for (int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
	  for (int channel =0;channel <NUMEXO_NB_CHANNELS;channel++){

	//hGain[iboard][channel]->Reset();
	// hFeedBack[iboard][channel]->Reset();

	hTrace_Sum[iboard][channel]->Reset();
	hRaw[iboard][channel]->Reset();
	hTrap[iboard][channel]->Reset();
	dssd_event_counter[iboard][channel] =0;
	}
	}

	h_delT_ff->Reset();
	h_delT_fb->Reset();
	h_delT_bf->Reset();
	h_delT_bb->Reset();
	h_E_frontBack->Reset();
	h_DSSD_XY_hit->Reset();
	h_dssd_count_strip->Reset();
	h_dssd_count_board->Reset();
	h_calib_strip->Reset();
	h_raw_strip->Reset();
	h_dssdStrip_rate->Reset();
	h_dssdBoard_rate->Reset();
	h_tunnelBoard_rate->Reset();
	h_tunnelPad_rate->Reset();*/
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! end of each run
/*! Called at the end of each run. Here DSSD pixels need to constructed or correlations between events must be made to treat the last buffer.
*/
void GUser::EndUserRun()
{
	dssdEventVec = dEvent->construct(dssdDataVec);
	for (std::vector<dssdPixel>::iterator it = dssdEventVec.begin() ; it != dssdEventVec.end(); ++it){				
		h_E_frontBack->Fill((*it).get_energyX(), (*it).get_energyY());
		h_DSSD_XY_hit->Fill((*it).get_X(), (*it).get_Y());
	}
	cout<< "calling GUser::EndUserRun()"<<endl;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! End of global run
/*! This method must be called explicitly. Prints the total number of different data frames at the end of the program. 
*/
//-------------------
void GUser::EndUser()
{
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

	switch (type) {
		case MFM_COBOF_FRAME_TYPE:
		case MFM_COBO_FRAME_TYPE:
			frameCounter[0]++;
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
			frameCounter[5]++;
			break;
		case MFM_SIRIUS_FRAME_TYPE:
			frameCounter[6]++;
			break;
		default:
			nberror++;
			frameCounter[7]++;
			break;
	}// end of switch

	if (cf == NULL) {
		fError.TreatError(1,0,"in GUser::User() return null frfReduce_factorame in GetEvent())->GetFrame()");
	}else {
		UserFrame(cf);
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Treatment of MFM frames
void GUser::UserFrame(MFMCommonFrame* commonframe){

	int type =  commonframe->GetFrameType();

	if ((type == MFM_MERGE_EN_FRAME_TYPE)or(type == MFM_MERGE_TS_FRAME_TYPE)) {
		UserMergeFrame(commonframe);
	}  
	else if ((type == MFM_COBO_FRAME_TYPE) or (type== MFM_COBOF_FRAME_TYPE)) {
		UserCoboFrame(commonframe);
	}
	else if (type == MFM_SIRIUS_FRAME_TYPE){
		UserSiriusFrame(commonframe);
	}
	else if(type == MFM_REA_GENE_FRAME_TYPE){
		UserGenericFrame(commonframe);
	}

}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Treatment of merged frames
/*! The frames within the defined time window are groupped into merged frames. Normally, if everything is ok, the DSSD pixel construction and establishing correlations among the events from different detector are done here. The only catch of enabling the Merger is it consumes a lot of memory.   
*/ 
void GUser::UserMergeFrame(MFMCommonFrame* commonframe){
	int i_insframe = 0;
	int nbinsideframe = 0;
	int dumpsize = 16;

	fMergeframe->SetAttributs(commonframe->GetPointHeader());

	nbinsideframe = fMergeframe->GetNbItems();
	//cout<<"------------------- ninside frame = "<<nbinsideframe<<endl;
	framesize= fMergeframe->GetFrameSize();
	if (s1->fverbose >= 3){
		fMergeframe->HeaderDisplay();
	}
	if (s1->fverbose >= 4){
		commonframe->HeaderDisplay();
	}
	if (s1->fverbose >= 5){
		if (framesize < maxdump) dumpsize = framesize;
		else dumpsize = maxdump;
		fMergeframe->DumpRaw(dumpsize, 0);
	}
	fMergeframe->ResetReadInMem();
	//Reset vectors
	dssdDataVec.clear();
	for(i_insframe = 0; i_insframe < nbinsideframe; i_insframe++) {
		//fMergeframe->ReadInFrame(fInsideframe);
		fMergeframe->ReadInFrame(commonframe);
		UserFrame(commonframe);
		//UserFrame(fInsideframe);

	}

	// At this point you can do treatement inter frames


}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Treatment of Generic frames (for the Tunnel detectors)
/*! The energy and timestamps are obtained from the data. To get the board number, macro pixel number and the detector number, use the methods from the tunnelData class. The macro pixel configurations are specified in the Run.config. The configuration can be changed (for example ABCD to BADC) keeping the macro pixels definitions constant i.e., if pixels 1 and 2 are grouped as A, you should not define it as B. If you do so, remember to update in tunnelData class, otherwise, the correct picture (2D hit pattern) will not be displayed.
 * To calibrate do Calibration::perform(tunnelData* const data).
 * To get the board number use tunnelData::get_tunnelBoardNumber(int* p_board).
 * To get the physical macro pixel number use tunnelData::get_macroPixelPhysicalNumber(int* p_board, int* p_channel). This method also detemines the position of the macro pixel.
 * To get the detector number use  tunnelData::get_tunnelDetectorNumber(int* p_board, int* p_channel).
 */
void GUser::UserGenericFrame(MFMCommonFrame* commonframe)
{
	fGenericframe->SetAttributs(commonframe->GetPointHeader());
	framesize=commonframe->GetFrameSize();
	channel =fGenericframe->GetChannelId();
	board =  fGenericframe->GetBoardId();
	iboard =  s1->boardIndex_Tunnel[board];
	value = fGenericframe->GetEnergy();
	trace_size =0;
	timestamp = fGenericframe->GetTimeStamp();
	tData->set_channelID(channel);
	tData->set_boardID(board);
	tData->set_boardIdx(iboard);
	tData->set_timestamp( timestamp);
	tData->set_eventnumber( eventnumber);
	h_tunnelRaw[iboard][channel]->Fill (value);
	Energy = static_cast<Double_t> (value);
	calibEnergy = calib->perform(tData);
	h_tunnelCalib[iboard][channel]->Fill (Energy);
	Time = timestamp;
	tunnelBoardNo = tData->get_tunnelBoardNumber(&board); 
	tunnelPadNo = tData->get_macroPixelPhysicalNumber(&board, &channel);//gets the pixels
	tunnelDetectorNo = tData->get_tunnelDetectorNumber(&board, &channel);
	h_tunnel_count_board->Fill(tunnelBoardNo);
	h_tunnel_count_pad->Fill(tunnelPadNo);
	get_Count_Rates(tunnelBoardNo, tunnelPadNo, timestamp, 2);

	for(unsigned int i = 0; i< tData->get_macroPixel(&board,&channel).pixels.size();i++){
		h_TUNNEL_XY_hit[tunnelDetectorNo-1]->Fill(tData->get_macroPixel(&board,&channel).pixels[i].get_X(),tData->get_macroPixel(&board,&channel).pixels[i].get_Y());
	}
	if(s1->fverbose >= 3){
		cout<<"-------Generic Frame-------\n";
		printf (" board = %d , channel =%d , energy=%d , timestamp = %llu\n", board, channel, value, timestamp);
	}
	if (s1->fverbose >= 4){
		commonframe->HeaderDisplay();
	}
	if (s1->fverbose >= 4){
		if (framesize < maxdump) dumpsize = framesize;else dumpsize = maxdump;
		commonframe->DumpRaw(dumpsize, 0);
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
	channel = (int)fSiriusframe->GetChannelId();
	board = (int)fSiriusframe->GetBoardId();
	iboard =  (int)s1->boardIndex_DSSD[board];
	timestamp = fSiriusframe->GetTimeStamp();
	eventnumber = fSiriusframe->GetEventNumber();
	gain = fSiriusframe->GetGain();
	trace_size =s1->TRACE_SIZE;
	dData->set_channelID(channel);
	dData->set_boardID(board);
	dData->set_boardIdx(iboard);
	dData->set_timestamp( timestamp);
	dData->set_eventnumber( eventnumber);
	dData->set_gain(gain);
	stripnumber = dData->get_stripnumber(&board, &channel);
	h_dssd_count_strip->Fill(stripnumber );
	dssdBoardNo = dData->get_dssdBoardNumber(&board); 
	h_dssd_count_board->Fill(dssdBoardNo);
	get_Count_Rates(dssdBoardNo, stripnumber, timestamp, 1);
	if(s1->fverbose >= 3){
		cout<<"-------Sirius Frame-------\n";
		cout<<"boardId:  "<<board<<" , channel:  "<<channel<<"  strip "<<stripnumber<<" TimeStamp:  "<<timestamp<<" , EventNumber: "<<eventnumber<<endl;
	}
	if (s1->fverbose >= 4){
		commonframe->HeaderDisplay();
	}

	if (s1->fverbose >= 5){
		if (framesize < maxdump) dumpsize = framesize;else dumpsize = maxdump;
		commonframe->DumpRaw(dumpsize, 0);
	}

	// hGain[iboard][channel]->Fill(gain);
	// hFeedBack[iboard][channel]->Fill(fSiriusframe->GetFeedBack(0));
	// NbItems= fSiriusframe->GetNbItems();
	int j =0;	
	for (int i = s1->nStart_trace; i < 992 - s1->nEnd_trace; i++) {
		fSiriusframe->GetParameters(i, &value);
		j = i - s1->nStart_trace;
		hTrace_Sum[iboard][channel]->Fill(j,value);
		if(i < s1->TRACE_SIZE)
			dData->set_trace_value(j, value);
	}
	//---------------------------
	dData->GetSignalInfo();
	//Fill histograms
	hBaseline[iboard][channel]->Fill(dData->get_Baseline());
	hNoise[iboard][channel]->Fill(dData->get_Noise());
	hRisetime[iboard][channel]->Fill(dData->get_RiseTime());
	Energy = filter->perform(dData, hTrap[iboard][channel]);
	//Energy  = dData->get_signalHeight();
	h_raw_strip->Fill(Energy, stripnumber);
	hRaw[iboard][channel]->Fill(Energy);
	calibEnergy = calib->perform(dData);//Calibration
	h_calib_strip->Fill(dData->get_calibrated_energy(),stripnumber);
	hRaw[iboard][channel]->Fill(dData->get_raw_energy());
	//get CFD time in (ns)
	Time = cfd->perform(dData);

	//Fill baseline graphs
	gr_baseline[iboard][channel]->SetPoint(dssd_event_counter[iboard][channel],dssd_event_counter[iboard][channel],dData->get_Baseline());
	dssd_event_counter[iboard][channel]++;

	//	if(Time < 1E15){
	//----this condition ensures that the time stamp is ok
	dPoint.set_time(Time);
	dPoint.set_strip(stripnumber);
	dPoint.set_energy(dData->get_calibrated_energy());
	dssdDataVec.push_back(dPoint);
	if(s1->data_merged == 0){
		if(dssdDataVec.size() == s1->buffer_size){
			dssdEventVec = dEvent->construct(dssdDataVec, h_delT_ff, h_delT_fb, h_delT_bf, h_delT_bb);
			for (std::vector<dssdPixel>::iterator it = dssdEventVec.begin() ; it != dssdEventVec.end(); ++it){				
				h_E_frontBack->Fill((*it).get_energyX(), (*it).get_energyY());
				h_DSSD_XY_hit->Fill((*it).get_X(), (*it).get_Y());
			}
		}
	}
	//}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//!  Treatment of Cobo frames
/*!
 *
 *
 *
 */
void GUser::UserCoboFrame(MFMCommonFrame* commonframe){
	fCoboframe->SetAttributs(commonframe->GetPointHeader());
	framesize=fCoboframe->GetFrameSize();
	eventnumber =fCoboframe->GetEventNumber();
	timestamp = (uint64_t)(fCoboframe->GetTimeStamp());
	trace_size =0;
	if(s1->fverbose >=3){
		cout<<"-------Cobo Frame-------\n";
		cout << " EN = "<<eventnumber <<" TS = " << timestamp <<endl;
	}
	if (s1->fverbose>=4){
		fCoboframe->HeaderDisplay();
	}
	if (s1->fverbose>=5){
		if (framesize < maxdump) dumpsize = framesize;
		else dumpsize = maxdump;
		fCoboframe->DumpRaw(dumpsize, 0);
	}

}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Setting up Root Tree
/*!
 *  The pointer to the Root Ttree is accessed by calling GAcq::GetTree(). Then, the name, branches and leaves can be set up as usual.
 * GetTree()->SetName("rawDataTree");
 * GetTree()->Branch("Time", &Time, "Time/l");
 *
 */
void GUser::InitTTreeUser()
{
	GetTree()->SetName("rawDataTree");
	GetTree()->Branch("Time", &Time, "Time/l");
	GetTree()->Branch("EventNo",  &eventnumber, "EventNo/i");
	GetTree()->Branch("TraceSize",  &trace_size, "trace_size/s");
	GetTree()->Branch("Trace",  dData->get_trace(), "Trace[trace_size]/s");
	GetTree()->Branch("Gain",  &gain, "Gain/s");
	GetTree()->Branch("BoardID",  &board, "BoardID/s");
	GetTree()->Branch("ChannelID",  &channel, "ChannelID/s");
	GetTree()->Branch("Energy",  &Energy, "Energy/d");
	GetTree()->Branch("Baseline",  dData->get_Baseline_address(), "Baseline/d");
	GetTree()->Branch("Noise",  dData->get_Noise_address(), "Noise/d");
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//---------------
// Count rates
//--------------
void GUser::get_Count_Rates(int &board,  int &stripNumber, ullint &current_timeStamp, int type){
	deltaT_rates = static_cast<int64_t>(current_timeStamp - old_timeStamp);

	if(deltaT_rates >= rate_calcul_timestamp_lapse)
	{
		//--------------
		// DSSD rate
		//-------------
		if(type == DSSD_DETECTOR){
			//per strip	
			for(UShort_t i = 0; i < s1->NSTRIPS_DSSD;i++)
			{
				//if(dssd_rate_strip[i] > 0){//udate only if there is a new value
				h_dssdStrip_rate->SetBinContent(i+1, dssd_rate_strip[i]);
				dssd_rate_strip[i] =0;
				//}
			}
			// per board
			for(UShort_t b = 0; b < s1->NBOARDS_DSSD;b++)
			{
				//if(dssd_rate_board[b] > 0){//Update only if there is a new value			
				h_dssdBoard_rate->SetBinContent(b+1, dssd_rate_board[b]);
				gr_rate_dssdBoard[b]->SetPoint(rate_counterPoint_dssd[b], rate_counterPoint_dssd[b], dssd_rate_board[b]);
				rate_counterPoint_dssd[b]++;
				dssd_rate_board[b] =0;
				//}
			}
			//rate_counter_dssd++;
		}
		//---------------
		// Tunnel rate
		//--------------		
		if(type == TUNNEL_DETECTOR){
			//per pad
			for(UShort_t i = 0; i < s1->NofMacroPixels;i++)
			{
				//if(tunnel_rate_pad[i] > 0){//udate only if there is a new value
				h_tunnelPad_rate->SetBinContent(i+1, tunnel_rate_pad[i]);
				tunnel_rate_pad[i] = 0;			
				//}
			}
			// per board
			for(UShort_t b = 0; b < s1->NBOARDS_TUNNEL;b++)
			{
				//if(tunnel_rate_board[b]>0){
				h_tunnelBoard_rate->SetBinContent(b+1, tunnel_rate_board[b]);
				gr_rate_tunnelBoard[b]->SetPoint(rate_counterPoint_tunnel[b], rate_counterPoint_tunnel[b], tunnel_rate_board[b]);
				rate_counterPoint_tunnel[b]++;
				tunnel_rate_board[b] =0;				
				//}
			}
			//rate_counter_tunnel++;		
		}
		// reset time

		old_timeStamp = current_timeStamp;
	}

	if(type == DSSD_DETECTOR){
		dssd_rate_strip[stripNumber]++;
		dssd_rate_board[board]++;
	}

	if(type == TUNNEL_DETECTOR){
		tunnel_rate_pad[stripNumber]++;
		tunnel_rate_board[board]++;
	}
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
