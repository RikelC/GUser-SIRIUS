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


Double_t ff(Double_t *y, Double_t *par)
{
	Double_t func_cosh=par[0]/(TMath::Power(TMath::CosH(TMath::Pi()*(y[0]-par[1])/par[2]),2.));

	return func_cosh;
}

void sed_calibration(float *in,float *coef,int ns)
{

	for(int i=0;i<ns;i++)
	{
		if(i!=15)
			in[i]=in[i]*coef[22]/coef[i];
	}

	//pistes bizarres ou manquantes
	/*
	   for(int i=0;i<ns;i++)
	   {
	   if(i==12) in[i]=in[i-1]/2.+in[i+1]/2.;
	   }
	   */

}

void max_c(float *strip,float *t,float *max,int *imax,int *tma,int n)
{
	*max=0;
	*imax=0;
	for(int i=0;i<n;i++)
	{	       
		if(strip[i]>*max) 
		{
			*max=strip[i];
			*imax=i;
			*tma=t[i];
		}

	}
} 

void mult_c(float *strip,int imax,int n,float thresh,int *mult,float *sum)
{
	*mult=0;
	*sum=0;
	for(int i=imax;i<n;i++)
	{
		if(strip[i]>thresh) {
			(*mult)++;
			(*sum)+=strip[i];
		}
		else break;
	} 
	for(int i=imax-1;i>0;i--)
	{
		if(strip[i]>thresh) {
			(*mult)++;
			(*sum)+=strip[i];
		}
		else break;
	} 
}

void bar_c(float *strip,int imax,int ns,float thresh,float *bar)
{
	*bar=0;
	double sumbar=0;

	for(int i=imax-ns;i<=imax+ns;i++)
	{
		if(strip[i]>0)
		{
			(*bar)+=strip[i]*(i+0.5);
			sumbar+=strip[i];
		}
	}
	(*bar)/=sumbar;

}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! GUser Constructor
/*! Here, the variables are initialized. The hitograms are created here. If you create the histograms in the InitUser(), it may result in segmentation fault during premature termination.
*/
GUser::GUser (int mode, GDevice* _fDevIn, GDevice* _fDevOut):GAcq(_fDevIn,_fDevOut)
{
	readMode = mode;
	s1                             = myGlobal::getInstance(readMode);
	//s1                             = myGlobal::getInstance();
	fCoboframe                     = new MFMCoboFrame();
	fInsideframe                   = new MFMCommonFrame();
	fMergeframe                    = new MFMMergeFrame();
	fSiriusframe                   = new MFMSiriusFrame();
	fGenericframe                  = new MFMReaGenericFrame();
	fMutantframe                   = new MFMMutantFrame();
	fEbyedatframe                  = new MFMEbyedatFrame();

	filter                         = new digitalFilters();
	cfd                            = new digitalCFD();
	dData                          = new dssdData();
	dEvent                         = new dssdEvent();
	//trEvent				= new dssdEvent();
	tEvent                         = new tunnelEvent();
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
	rate_counter_dssd              = 0;
	rate_counter_tunnel            = 0;
	dataSet_counter                = 0.;
	rate_calcul_timestamp_lapse    = static_cast<int64_t>(s1->rate_calcul_time_lapse *pow(10,8));
	prev_padNo =-100;

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
	hTrigger      = new TH1F**[s1->NBOARDS_DSSD];
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
		hTrigger[iboard]     = new TH1F*[NUMEXO_NB_CHANNELS];
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
			if(!hTrace_Sum[iboard][channel]) hTrace_Sum[iboard][channel] = new TH2F (name.Data(),name.Data(),s1->TRACE_SIZE,0,s1->TRACE_SIZE,1700,-1000,25000);
			else hTrace_Sum[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hTrace_Sum[iboard][channel],sfamname);
			//histograms for raw data = trapezodal height
			sfamname.Form("%s/RawHist",famname.Data());
			name.Form("RawData_%d_%d",s1->boardList_DSSD[iboard],channel);
			hRaw[iboard][channel] = (TH1F*)gROOT->Get(name);
			if(!hRaw[iboard][channel]) hRaw[iboard][channel] = new TH1F(name.Data(), name.Data(),20000,0.,20000.);
			else hRaw[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hRaw[iboard][channel],sfamname);
			//histograms for calibrated data = trapezodal height * gain + offset
			sfamname.Form("%s/CalibHist",famname.Data());
			name.Form("CalibData_%d_%d",s1->boardList_DSSD[iboard],channel);
			hCalib[iboard][channel] = (TH1F*)gROOT->Get(name);
			if(!hCalib[iboard][channel]) hCalib[iboard][channel] = new TH1F(name.Data(), name.Data(),2000,0.,100000.);
			else hCalib[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hCalib[iboard][channel],sfamname);

			//histo for trapezoidal
			sfamname.Form("%s/Trapezoidal",famname.Data());
			name.Form("Trap_%d_%d",s1->boardList_DSSD[iboard],channel);
			hTrap[iboard][channel] = (TH2F*) gROOT->Get(name);
			if(!hTrap[iboard][channel]) hTrap[iboard][channel] = new TH2F(name.Data(),name.Data(), s1->TRACE_SIZE, 0,s1-> TRACE_SIZE, 4000,-8000,8000);
			else hTrap[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hTrap[iboard][channel],sfamname);

			//histo for trigger
			sfamname.Form("%s/Trigger",famname.Data());
			name.Form("hTrigger_%d_%d",s1->boardList_DSSD[iboard],channel);
			hTrigger[iboard][channel] = (TH1F*) gROOT->Get(name);
			if(!hTrigger[iboard][channel]) hTrigger[iboard][channel] = new TH1F(name.Data(),name.Data(), 992,0, 992);
			else hTrigger[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hTrigger[iboard][channel],sfamname);


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
	if(!h_raw_strip) h_raw_strip = new TH2F("h_raw_strip",";E (ADC ch);strip number", 2000,0,100000,256,0,256);
	else h_raw_strip->Reset();
	GetSpectra()->AddSpectrum( h_raw_strip,famname);
	//-calibrated energy vs strip number
	h_calib_strip = (TH2F*)gROOT->Get("h_calib_strip");
	if(! h_calib_strip)h_calib_strip = new TH2F("h_calib_strip",";E (keV);strip number", 2000,0,100000,256,0,256);
	else  h_calib_strip->Reset();
	GetSpectra()->AddSpectrum( h_calib_strip,famname);
	//-histo for front energy vs back energy
	h_E_frontBack = (TH2F*)gROOT->Get("h_E_frontBack");
	if(!h_E_frontBack)h_E_frontBack = new TH2F("h_E_frontBack",";frontE;backE",1000,0,100000,1000,0,100000);
	else h_E_frontBack->Reset();
	GetSpectra()->AddSpectrum( h_E_frontBack,famname);
	//-hit pattern in the DSSD
	h_DSSD_XY_hit = (TH2I*)gROOT->Get("h_DSSD_XY_hit");
	if(!h_DSSD_XY_hit) h_DSSD_XY_hit =  new TH2I("h_DSSD_XY_hit","hit pattern ;Front strip (X); Back strip (Y)",128,0,128,128,0,128);
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
	if(!h_dssdE_Tof) h_dssdE_Tof = new TH2F("h_dssdE_Tof",";E;ToF", 5000,0,30000, 1000,0,1000);
	else h_dssdE_Tof->Reset();
	GetSpectra()->AddSpectrum( h_dssdE_Tof,famname);


	h_dssdE_Tof2 = (TH2F*)gROOT->Get("h_dssdE_Tof2");
	if(!h_dssdE_Tof2) h_dssdE_Tof2 = new TH2F("h_dssdE_Tof2",";E;ToF", 5000,0,30000, 1000,0,20000);
	else h_dssdE_Tof2->Reset();
	GetSpectra()->AddSpectrum( h_dssdE_Tof2,famname);


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


	famname.Form("sumSpectra");
	h_sum_front = new TH1F("h_sum_front","h_sum_front",2000,0,20000);
	GetSpectra()->AddSpectrum(h_sum_front,famname);
	h_sum_back= new TH1F("h_sum_back","h_sum_back",2000,0,20000);
	GetSpectra()->AddSpectrum(h_sum_back,famname);


	famname.Form("TunnelSpectra");
	//-raw energy vs strip number
	h_raw_pad = (TH2F*)gROOT->Get("h_raw_pad");
	if(!h_raw_pad) h_raw_pad = new TH2F("h_raw_pad",";E (ADC ch);pad number", 2000,0,100000,96,0,96);
	else h_raw_pad->Reset();
	GetSpectra()->AddSpectrum( h_raw_pad,famname);
	//-calibrated energy vs strip number
	h_calib_pad = (TH2F*)gROOT->Get("h_calib_pad");
	if(! h_calib_pad)h_calib_pad = new TH2F("h_calib_pad",";E (keV);pad number", 2000,0,100000,96,0,96);
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

	grUnsortedT = new TGraph();
	grUnsortedT->SetNameTitle("grUnsortedT", "unsorted time increament test");
	GetSpectra()->AddSpectrum(grUnsortedT);

	grT = new TGraph();
	grT->SetNameTitle("grT", "time increament test");
	GetSpectra()->AddSpectrum(grT);
	grDT = new TGraph();
	grDT->SetNameTitle("grDT", "dt increament test");
	GetSpectra()->AddSpectrum(grDT);
	htunnel1_E1E2 = new TH2F("htunnel1_E1E2","E1 vs E2", 1200,0,6000,1200,0,6000);
	htunnel2_E1E2 = new TH2F("htunnel2_E1E2","E1 vs E2", 1200,0,6000,1200,0,6000);
	htunnel3_E1E2 = new TH2F("htunnel3_E1E2","E1 vs E2", 1200,0,6000,1200,0,6000);
	htunnel4_E1E2 = new TH2F("htunnel4_E1E2","E1 vs E2", 1200,0,6000,1200,0,6000);
	GetSpectra()->AddSpectrum(htunnel1_E1E2);
	GetSpectra()->AddSpectrum(htunnel2_E1E2);
	GetSpectra()->AddSpectrum(htunnel3_E1E2);
	GetSpectra()->AddSpectrum(htunnel4_E1E2);
	htunnel1_dt = new TH1I("htunnel1_dt","dt", 1000,-1000,1000);
	htunnel2_dt = new TH1I("htunnel2_dt","dt", 1000,-1000,1000);
	htunnel3_dt = new TH1I("htunnel3_dt","dt", 1000,-1000,1000);
	htunnel4_dt = new TH1I("htunnel4_dt","dt", 1000,-1000,1000);
	GetSpectra()->AddSpectrum(htunnel1_dt);
	GetSpectra()->AddSpectrum(htunnel2_dt);
	GetSpectra()->AddSpectrum(htunnel3_dt);
	GetSpectra()->AddSpectrum(htunnel4_dt);

	h_front_traceGS = new TH2F("h_front_traceGS","", 992,0,992,1000,0,16000);
	h_back_traceGS = new TH2F("h_back_traceGS","", 992,0,992,1000,0,16000);
	GetSpectra()->AddSpectrum(  h_front_traceGS   ,"");
	GetSpectra()->AddSpectrum(  h_back_traceGS   ,"");
	h_front_traceNGS = new TH2F("h_front_traceNGS","", 992,0,992,1000,0,16000);
	h_back_traceNGS = new TH2F("h_back_traceNGS","", 992,0,992,1000,0,16000);
	GetSpectra()->AddSpectrum(  h_front_traceNGS   ,"");
	GetSpectra()->AddSpectrum(  h_back_traceNGS   ,"");
	h_front_rGS = new TH1F("h_front_rGS","", 1000,0,16000);
	h_back_rGS = new TH1F("h_back_rGS","", 1000,0,16000);
	GetSpectra()->AddSpectrum(  h_front_rGS   ,"");
	GetSpectra()->AddSpectrum(  h_back_rGS   ,"");
	h_front_rNGS = new TH1F("h_front_rNGS","", 1000,0,16000);
	h_back_rNGS = new TH1F("h_back_rNGS","", 1000,0,16000);
	GetSpectra()->AddSpectrum(  h_front_rNGS   ,"");
	GetSpectra()->AddSpectrum(  h_back_rNGS   ,"");
	//gtacvsevt = new TGraph(10000);
	//gtacvsevt2 = new TGraph(1000); 
	h_tac = new TH1F("h_tac","", 4000,0,64000);
	GetSpectra()->AddSpectrum(  h_tac   ,"");




	ny=61;
	nx=81;

	cut=0;
	npb=3;
	threshold=20;
	nvoie = 272;
	chan=new int[nvoie];
	ind=new int[nvoie];


	ampliy=new float[ny];
	amplix=new float[nx];

	startx=new float[nx];
	starty=new float[ny];

	fitx = new TF1("fitx",ff,1,80);
	fity = new TF1("fity",ff,1,60);

	for (int i=0; i<nvoie;i++)
	{
		//baseline[i]=0;
		//sigbsline[i]=0.;
		amplitude[i]=0.;
		tmax[i]=0;
		//tstart[i]=0;
		//tt[i]=0;
		//rt[i]=0;
		nv[i]=0;
		naget[i]=0;
	}



	//fichier calib Get

	coefx=new float[nx]; 
	coefy=new float[ny];
	float bufc;

	ifstream in("calib240fC_260919");
	for(int i=0;i<nx;i++)
	{
		in>>bufc>>coefx[i]>>bufc;
	}
	for(int i=0;i<ny;i++)
	{
		in>>bufc>>coefy[i]>>bufc;
	}
	in.close();

	//lecture lookuptable 
	aget0=new int[68]; 
	aget1=new int[68]; 
	aget2=new int[68]; 
	aget3=new int[68];
	const int linesize=1024;
	char buf[linesize];
	ifstream infile("ConfigFiles/lookuptableV1");
	if(!infile)
	{
		cout<<"impossible d'ouvrir la table d'allocation"<<endl;
		exit(0);
	}
	infile.getline(buf,linesize);
	int l=1;

	//ICI reventilation channel aget vers channel detector

	while(l<=272)
	{
		if(l<=68) infile>>aget>>nc>>aget0[l-1];
		else if(l>68 && l<=136) {infile>>aget>>nc>>aget1[l-1-68];//cout<<aget1[l-1-68]<<endl;
		}
		else if(l>136 && l<=204) infile>>aget>>nc>>aget2[l-1-136];
		else infile>>aget>>nc>>aget3[l-1-204]; 
		l++;
	}
	infile.close();

	//histo definitions for SeD

	famname.Form("SedSpectra");

	hx=new TH1F("hx1","X",nx,0,nx);
	GetSpectra()->AddSpectrum(hx,famname);
	hy=new TH1F("hy1","Y",ny,0,ny);
	GetSpectra()->AddSpectrum(hy,famname);
	hyinit=new TH1F("hyinit","Y bin init",68,0,68);	
	GetSpectra()->AddSpectrum(hyinit,famname);
	hxinit=new TH1F("hxinit","X bin init",136,0,136);
	GetSpectra()->AddSpectrum(hxinit,famname);


	hxa=new TH1F("hxa","X",nx,0,nx);
	GetSpectra()->AddSpectrum(hxa,famname);
	hya=new TH1F("hya","Y",ny,0,ny); 
	GetSpectra()->AddSpectrum(hya,famname);
	hmaxx=new TH1F("hmaxx","",50,0,4000);
	GetSpectra()->AddSpectrum(hmaxx,famname);
	hmaxy=new TH1F("hmaxy","",50,0,4000);
	GetSpectra()->AddSpectrum(hmaxy,famname);
	hmultx=new TH1F("hmultx","",30,0,30);   
	GetSpectra()->AddSpectrum(hmultx,famname);
	hmulty=new TH1F("hmulty","",30,0,30); 
	GetSpectra()->AddSpectrum(hmulty,famname);

	hmaxx1d=new TH1F("hmaxx1d","",50,0,4000);
	GetSpectra()->AddSpectrum( hmaxx1d    ,famname);
	hmaxx2d=new TH1F("hmaxx2d","",50,0,4000); 
	GetSpectra()->AddSpectrum(  hmaxx2d   ,famname);
	hmaxx1g=new TH1F("hmaxx1g","",50,0,4000);
	GetSpectra()->AddSpectrum(  hmaxx1g   ,famname);
	hmaxx2g=new TH1F("hmaxx2g","",50,0,4000);  
	GetSpectra()->AddSpectrum(  hmaxx2g   ,famname);


	hmaxy1d=new TH1F("hmaxy1d","",50,0,4000);
	GetSpectra()->AddSpectrum( hmaxy1d    ,famname);
	hmaxy2d=new TH1F("hmaxy2d","",50,0,4000); 
	GetSpectra()->AddSpectrum( hmaxy2d    ,famname);
	hmaxy1g=new TH1F("hmaxy1g","",50,0,4000);
	GetSpectra()->AddSpectrum(  hmaxy1g   ,famname);
	hmaxy2g=new TH1F("hmaxy2g","",50,0,4000);  
	GetSpectra()->AddSpectrum(  hmaxy2g   ,famname);

	himaxx=new TH1F("himaxx","",nx,0,nx);
	GetSpectra()->AddSpectrum(  himaxx   ,famname);
	himaxy=new TH1F("himaxy","",ny,0,ny);  
	GetSpectra()->AddSpectrum(  himaxy   ,famname);
	htmaxx=new TH1F("htmaxx","",512,0,512);
	GetSpectra()->AddSpectrum(  htmaxx   ,famname);
	htmaxy=new TH1F("htmaxy","",512,0,512);  
	GetSpectra()->AddSpectrum(  htmaxy   ,famname);


	//histos for charge centroid fits
	hbarx=new TH1F("hbarx","X bar",162,0,81);
	GetSpectra()->AddSpectrum(  hbarx   ,famname);
	hbary=new TH1F("hbary","Y bar",122,0,61);
	GetSpectra()->AddSpectrum(  hbary   ,famname);
	hbarxm=new TH1F("hbarxm","X bar[mm]",450,0,225);	
	GetSpectra()->AddSpectrum(  hbarxm   ,famname);
	hbarym=new TH1F("hbarym","Y bar[mm]",340,0,170);	
	GetSpectra()->AddSpectrum(  hbarym   ,famname);
	hbar2Dm=new TH2F("bar2Dm","XvsY bar[mm]",1350,0,225,1530,0,170);
	GetSpectra()->AddSpectrum(  hbar2Dm   ,famname);

	/*
	//histos for cosh fits
	hcoshxm=new TH1F("hcosxm","X cosh[mm]",450,0,225);	
	GetSpectra()->AddSpectrum(  hcoshxm   ,famname);
	hcoshym=new TH1F("hcoshym","Y cosh[mm]",340,0,170);  
	GetSpectra()->AddSpectrum(  hcoshym   ,famname);
	hbarcosh2Dm=new TH2F("barcosh2Dm","XvsY bar[mm]",675,0,225,510,0,170);	
	GetSpectra()->AddSpectrum(  hbarcosh2Dm   ,famname);
	*/
	hsum_xy=new TH2F("hsum_xy","",100,0,15000,100,0,15000);
	GetSpectra()->AddSpectrum(  hsum_xy   ,famname);
	hmult_xy=new TH2F("hmult_xy","",20,0,20,20,0,20);;
	GetSpectra()->AddSpectrum(  hmult_xy   ,famname);
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
		  delete [] hTrigger[i];
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
	  delete [] hTrigger;
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
	  trackerNumexoDataVec.clear();
	  tunnelDataVec.clear();
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
	  if(fMutantframe) delete fMutantframe;
	  if(fEbyedatframe) delete fEbyedatframe;
	  if(filter) delete filter;
	  if(cfd) delete cfd;
	  if(dEvent) delete dEvent;
	  if(tEvent) delete tEvent;
	  if(calib) delete calib;
	  if(tData) delete tData;
	  if(dData) delete dData;


	  delete [] chan; 
	  delete [] ind;
	  delete [] ampliy;
	  delete [] amplix;
	  delete [] startx;
	  delete [] starty;
	  delete [] coefx; 
	  delete [] coefy;

	  delete [] aget0;
	  delete [] aget1; 
	  delete [] aget2; 
	  delete [] aget3;
	  cout<<"GUser Desctructor called"<<endl;
	  gROOT->cd();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Initialisation for global  user treatement
/*! The variables and histograms are initialized at the start.
*/
void GUser::InitUser()
{


	for(int i1=0;i1<NB_COBO;i1++)
		for(int i2=0;i2<NB_ASAD;i2++)
			for(int i3=0;i3<NB_AGET;i3++)
				for(int i4=0;i4<NB_CHANNEL;i4++)
				{
					amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;
					tmax[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;
					for(int i5=0;i5<NB_SAMPLES;i5++) TRACE[i1][i2][i3][i4][i5]=0;
				}

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
	//dssdEventVec = dEvent->construct(dssdDataVec,grUnsortedT,  grT, grDT);
	cout<<"tracker size "<<trackerNumexoDataVec.size()<<"  dssd event size "<<dssdEventVec.size()<<endl;
	for (std::vector<dssdPixel>::iterator it = dssdEventVec.begin() ; it != dssdEventVec.end(); ++it){				
		h_E_frontBack->Fill((*it).get_energyX(), (*it).get_energyY());
		cout<<"X : "<<(*it).get_X()<<"  Y "<<(*it).get_Y()<<endl;
		h_DSSD_XY_hit->Fill((*it).get_X(), (*it).get_Y());
	}





	//with cfd time
	llint tof =0;// tof2 =0;

	//int sed_counter =0;
	//int dssd_counter=0;
	ullint dssdTime =0;
	ullint maxTime =0;
	ullint sedTime =0;
	ullint dt =0;
	double maxE =0;
	dEvent->SortInTime(newDssdDataVec);
	//cout<<"new dssdDataVec size "<<newDssdDataVec.size()<<endl;

	dssdDataVec2.push_back(newDssdDataVec[0]);
	for(unsigned int j = 1; j < newDssdDataVec.size(); j++){

		dt = static_cast<ullint> (newDssdDataVec[j].get_time() - newDssdDataVec[j-1].get_time());
		//cout<<"new dssd strip : "<< newDssdDataVec[j].get_strip()<<"  Time "<< newDssdDataVec[j].get_time()<<"  E "<<newDssdDataVec[j].get_energy()<<"  dt "<<dt<<endl;
		if(dt > 10000){

			if(dssdDataVec2.size()> 1){
				//cout<<"new sub buffer newDssdDataVec size "<<dssdDataVec2.size()<<endl;
				dssdTime =0;
				maxTime =0;
				sedTime =0;
				maxE =0;
				tof = 0;
				for(unsigned int k =0; k < dssdDataVec2.size(); k++){
					//cout<<" toto strip : "<< dssdDataVec2[k].get_strip()<<"  Time "<< dssdDataVec2[k].get_time()<<"  E "<<dssdDataVec2[k].get_energy()<<endl;
					if(dssdDataVec2.at(k).get_strip() >200) sedTime = dssdDataVec2[k].get_time();
					else {dssdTime = dssdDataVec2[k].get_time();


						if(dssdDataVec2[k].get_energy() > maxE){
							maxE = dssdDataVec2[k].get_energy();
							maxTime = dssdTime;
						}//if
					}//else


				}//for
				//cout<<"Max E "<<max<<endl;
				tof = TMath::Abs(static_cast<llint> (sedTime - maxTime));
				if (tof > 0 && maxE > 100)h_dssdE_Tof2->Fill(maxE, tof);

			}//if size

			dssdDataVec2.clear();
			dssdDataVec2.push_back(newDssdDataVec[j]);
		}//if coinc


		else{
			dssdDataVec2.push_back(newDssdDataVec[j]);
		}

	}//for looop ends

	//last sub buffer

	if(dssdDataVec2.size()> 1){
		dssdTime =0;
		maxTime =0;
		sedTime =0;
		maxE =0;
		for(unsigned int k =0; k < dssdDataVec2.size(); k++){

			if(dssdDataVec2.at(k).get_strip() >200) sedTime = dssdDataVec2[k].get_time();
			else {dssdTime = dssdDataVec2[k].get_time();


				if(dssdDataVec2[k].get_energy() > maxE){
					maxE = dssdDataVec2[k].get_energy();
					maxTime = dssdTime;
				}//if
			}//else
		}
		tof = TMath::Abs(static_cast<llint> (sedTime - maxTime));
		if(tof > 0 && maxE > 100) h_dssdE_Tof2->Fill(maxE, tof);
	}//if size


	dssdDataVec2.clear();


	newDssdDataVec.clear();



	//with Sed Timestamp

	dEvent->SortInTime(dssdDataVec);
	//cout<<"dssdDataVec size "<<dssdDataVec.size()<<endl;

	dssdDataVec2.push_back(dssdDataVec[0]);
	for(unsigned int j = 1; j < dssdDataVec.size(); j++){

		dt = static_cast<ullint> (dssdDataVec[j].get_time() -dssdDataVec[j-1].get_time());
		//cout<<"strip : "<< dssdDataVec[j].get_strip()<<"  Time "<< dssdDataVec[j].get_time()<<"  E "<<dssdDataVec[j].get_energy()<<"  dt "<<dt<<endl;
		if(dt > 1000){

			if(dssdDataVec2.size()> 1){
				//cout<<"sub buffer dssdDataVec size "<<dssdDataVec2.size()<<endl;
				dssdTime =0;
				maxTime =0;
				sedTime =0;
				maxE =0;
				tof = 0;
				for(unsigned int k =0; k < dssdDataVec2.size(); k++){
					//cout<<" toto strip : "<< dssdDataVec2[k].get_strip()<<"  Time "<< dssdDataVec2[k].get_time()<<"  E "<<dssdDataVec2[k].get_energy()<<endl;
					if(dssdDataVec2.at(k).get_strip() ==1000) sedTime = dssdDataVec2[k].get_time();
					else {dssdTime = dssdDataVec2[k].get_time();


						if(dssdDataVec2[k].get_energy() > maxE){
							maxE = dssdDataVec2[k].get_energy();
							maxTime = dssdTime;
						}//if
					}//else


				}//for
				//cout<<"Max E "<<max<<endl;
				tof = TMath::Abs(static_cast<llint> (sedTime - maxTime));
				if (tof > 0 && maxE > 100)h_dssdE_Tof->Fill(maxE, tof);

			}//if size

			dssdDataVec2.clear();
			dssdDataVec2.push_back(dssdDataVec[j]);
		}//if coinc


		else{
			dssdDataVec2.push_back(dssdDataVec[j]);
		}

	}//for looop ends

	//last sub buffer

	if(dssdDataVec2.size()> 1){
		dssdTime =0;
		maxTime =0;
		sedTime =0;
		maxE =0;
		for(unsigned int k =0; k < dssdDataVec2.size(); k++){

			if(dssdDataVec2.at(k).get_strip() ==1000) sedTime = dssdDataVec2[k].get_time();
			else {dssdTime = dssdDataVec2[k].get_time();


				if(dssdDataVec2[k].get_energy() > maxE){
					maxE = dssdDataVec2[k].get_energy();
					maxTime = dssdTime;
				}//if
			}//else
		}
		tof = TMath::Abs(static_cast<llint> (sedTime - maxTime));
		if(tof > 0 && maxE > 100) h_dssdE_Tof->Fill(maxE, tof);
	}//if size


	dssdDataVec2.clear();


	dssdDataVec.clear();






	tEvent->construct(tunnelDataVec, htunnel1_E1E2,  htunnel2_E1E2, htunnel3_E1E2, htunnel4_E1E2, htunnel1_dt,htunnel2_dt, htunnel3_dt, htunnel4_dt);
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
		//cf->HeaderDisplay();
		//cf->DumpRaw(128,0);
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
		if(s1->fsaveTree)coboTTree->Fill();
	}
	else if (type == MFM_SIRIUS_FRAME_TYPE){
		UserSiriusFrame(commonframe);
		if(s1->fsaveTree)siriusTTree->Fill();
	}
	else if(type == MFM_REA_GENE_FRAME_TYPE){
		UserGenericFrame(commonframe);
		if(s1->fsaveTree)reaGenericTTree->Fill();
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
	timestamp = fMergeframe->GetTimeStamp();
	cout << nbinsideframe << " " << fMergeframe->GetEventNumber() << endl;



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
	//dssdDataVec.clear();
	//trackerNumexoDataVec.clear();
	for(i_insframe = 0; i_insframe < nbinsideframe; i_insframe++) {
		fMergeframe->ReadInFrame(fInsideframe);
		UserFrame(fInsideframe);

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
	reaGenericEnergy = fGenericframe->GetEnergy();
	reaGenericTime = fGenericframe->GetTime();
	timestamp = fGenericframe->GetTimeStamp();
	if(board !=211){
		iboard =  s1->boardIndex_Tunnel[board];
		tData->set_channelID(channel);
		tData->set_boardID(board);
		tData->set_boardIdx(iboard);
		tData->set_timestamp( timestamp);
		tData->set_eventnumber( eventnumber);
		tData->set_raw_energy( reaGenericEnergy);
		h_tunnelRaw[iboard][channel]->Fill (reaGenericEnergy);
		calibEnergy = calib->perform(tData);
		tData->set_calibrated_energy( calibEnergy);
		h_tunnelCalib[iboard][channel]->Fill (calibEnergy);
		tunnelBoardNo = tData->get_tunnelBoardNumber(&board); 
		tunnelPadNo = tData->get_macroPixelPhysicalNumber(&board, &channel);//gets the pixels
		tPoint.set_values(tData);
		tunnelDetectorNo = tData->get_tunnelDetectorNumber(&board, &channel);
		h_tunnel_count_board->Fill(tunnelBoardNo);
		h_tunnel_count_pad->Fill(tunnelPadNo);
		get_Count_Rates(tunnelBoardNo, tunnelPadNo, timestamp, 2);
	}
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

	/*tunnelDataVec.push_back(tPoint);
	  if(tunnelDataVec.size() ==2000){
	  tEvent->construct(tunnelDataVec, htunnel1_E1E2,  htunnel2_E1E2, htunnel3_E1E2, htunnel4_E1E2, htunnel1_dt,htunnel2_dt, htunnel3_dt, htunnel4_dt);

	  }*/

	if(board==211)h_tac->Fill(reaGenericTime);


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
	stripnumber = dData->get_stripnumber(&board, &channel);
	dssdBoardNo = dData->get_dssdBoardNumber(&board); 

	// set dssd data values for treatment
	dData->set_channelID(channel);
	dData->set_boardID(board);
	dData->set_boardIdx(iboard);
	dData->set_timestamp(timestamp);
	dData->set_eventnumber( eventnumber);
	dData->set_gain(gain);
	// get count rates per second
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
	// Computation
	// -----------------------

	dData->GetSignalInfo();

	//Energy  = dData->get_signalHeight();
	Energy = filter->perform(dData, hTrap[iboard][channel]);
	if(dData->gain_switched()) Energy += 10000; 
	calibEnergy = calib->perform(dData);//Calibration
	//get CFD time in (ns)
	Time = cfd->perform(dData);



	//Fill histograms
	h_raw_strip->Fill(Energy, stripnumber);
	hRaw[iboard][channel]->Fill(Energy);
	h_calib_strip->Fill(dData->get_calibrated_energy(),stripnumber);
	hCalib[iboard][channel]->Fill(calibEnergy);
	h_dssd_count_strip->Fill(stripnumber);
	h_dssd_count_board->Fill(dssdBoardNo);
	hTrigger[iboard][channel]->Fill(dData->get_Trigger());
	hBaseline[iboard][channel]->Fill(dData->get_Baseline());
	hNoise[iboard][channel]->Fill(dData->get_Noise());
	hRisetime[iboard][channel]->Fill(dData->get_RiseTime());




	if(dData->get_stripnumber() < 128)h_sum_front->Fill(Energy);
	else if(dData->get_stripnumber() > 127 && dData->get_stripnumber() < 256)h_sum_back->Fill(Energy);

	if(dData->gain_switched()){
		if(dData->get_stripnumber() < 128 ){
			for (int i = 0; i < s1->TRACE_SIZE; i++) {
				h_front_traceGS->Fill(i, dData->get_trace_value(i));
			}
			h_front_rGS->Fill(dData->get_sig_diff());
		}

		else if(dData->get_stripnumber() > 127 && dData->get_stripnumber() < 256){
			for (int i = 0; i < s1->TRACE_SIZE; i++) {
				h_back_traceGS->Fill(i, dData->get_trace_value(i));
			}

			h_back_rGS->Fill(dData->get_sig_diff());

		}
	}else{
		if(dData->get_stripnumber() < 128 ){
			for (int i = 0; i < s1->TRACE_SIZE; i++) {
				h_front_traceNGS->Fill(i, dData->get_trace_value(i));
			}

			h_front_rNGS->Fill(dData->get_sig_diff());
		}

		else if(dData->get_stripnumber() > 127 && dData->get_stripnumber() < 256){
			for (int i = 0; i < s1->TRACE_SIZE; i++) {
				h_back_traceNGS->Fill(i, dData->get_trace_value(i));
			}


			h_back_rNGS->Fill(dData->get_sig_diff());
		}


	}



	//Fill baseline graphs
	gr_baseline[iboard][channel]->SetPoint(dssd_event_counter[iboard][channel],dssd_event_counter[iboard][channel],dData->get_Baseline());
	dssd_event_counter[iboard][channel]++;

	//	if(Time < 1E15){
	//----this condition ensures that the time stamp is ok
	dPoint.set_time(timestamp);
	dPoint.set_strip(stripnumber);
	dPoint.set_energy(Energy);


	if(Energy > 600 && dData->get_stripnumber() < 128){
		dssdDataVec.push_back(dPoint);
		dssdDataPoint d(stripnumber, Time, Energy);
		newDssdDataVec.push_back(d);
	}


	if(board==166 && channel ==0){
		dssdDataPoint d(stripnumber, Time, Energy);
		newDssdDataVec.push_back(d);
	}

	//with cfd time
	if(newDssdDataVec.size() >= s1->buffer_size){
		llint tof =0;// tof2 =0;

		//int sed_counter =0;
		//int dssd_counter=0;
		ullint dssdTime =0;
		ullint maxTime =0;
		ullint sedTime =0;
		ullint dt =0;
		double maxE =0;
		dEvent->SortInTime(newDssdDataVec);
		//cout<<"new dssdDataVec size "<<newDssdDataVec.size()<<endl;

		dssdDataVec2.push_back(newDssdDataVec[0]);
		for(unsigned int j = 1; j < newDssdDataVec.size(); j++){

			dt = static_cast<ullint> (newDssdDataVec[j].get_time() - newDssdDataVec[j-1].get_time());
			//cout<<"new dssd strip : "<< newDssdDataVec[j].get_strip()<<"  Time "<< newDssdDataVec[j].get_time()<<"  E "<<newDssdDataVec[j].get_energy()<<"  dt "<<dt<<endl;
			if(dt > 10000){

				if(dssdDataVec2.size()> 1){
					//cout<<"new sub buffer newDssdDataVec size "<<dssdDataVec2.size()<<endl;
					dssdTime =0;
					maxTime =0;
					sedTime =0;
					maxE =0;
					tof = 0;
					for(unsigned int k =0; k < dssdDataVec2.size(); k++){
						//cout<<" toto strip : "<< dssdDataVec2[k].get_strip()<<"  Time "<< dssdDataVec2[k].get_time()<<"  E "<<dssdDataVec2[k].get_energy()<<endl;
						if(dssdDataVec2.at(k).get_strip() >200) sedTime = dssdDataVec2[k].get_time();
						else {dssdTime = dssdDataVec2[k].get_time();


							if(dssdDataVec2[k].get_energy() > maxE){
								maxE = dssdDataVec2[k].get_energy();
								maxTime = dssdTime;
							}//if
						}//else


					}//for
					//cout<<"Max E "<<max<<endl;
					tof = TMath::Abs(static_cast<llint> (sedTime - maxTime));
					if (tof > 0 && maxE > 100)h_dssdE_Tof2->Fill(maxE, tof);

				}//if size

				dssdDataVec2.clear();
				dssdDataVec2.push_back(newDssdDataVec[j]);
			}//if coinc


			else{
				dssdDataVec2.push_back(newDssdDataVec[j]);
			}

		}//for looop ends

		//last sub buffer

		if(dssdDataVec2.size()> 1){
			dssdTime =0;
			maxTime =0;
			sedTime =0;
			maxE =0;
			for(unsigned int k =0; k < dssdDataVec2.size(); k++){

				if(dssdDataVec2.at(k).get_strip() >200) sedTime = dssdDataVec2[k].get_time();
				else {dssdTime = dssdDataVec2[k].get_time();


					if(dssdDataVec2[k].get_energy() > maxE){
						maxE = dssdDataVec2[k].get_energy();
						maxTime = dssdTime;
					}//if
				}//else
			}
			tof = TMath::Abs(static_cast<llint> (sedTime - maxTime));
			if(tof > 0 && maxE > 100) h_dssdE_Tof2->Fill(maxE, tof);
		}//if size


		dssdDataVec2.clear();


		newDssdDataVec.clear();
	}




	//with Sed Timestamp
	if(dssdDataVec.size() >= s1->buffer_size){
		llint tof =0;// tof2 =0;

		//int sed_counter =0;
		//int dssd_counter=0;
		ullint dssdTime =0;
		ullint maxTime =0;
		ullint sedTime =0;
		ullint dt =0;
		double maxE =0;
		dEvent->SortInTime(dssdDataVec);
		//cout<<"dssdDataVec size "<<dssdDataVec.size()<<endl;

		dssdDataVec2.push_back(dssdDataVec[0]);
		for(unsigned int j = 1; j < dssdDataVec.size(); j++){

			dt = static_cast<ullint> (dssdDataVec[j].get_time() -dssdDataVec[j-1].get_time());
			//cout<<"strip : "<< dssdDataVec[j].get_strip()<<"  Time "<< dssdDataVec[j].get_time()<<"  E "<<dssdDataVec[j].get_energy()<<"  dt "<<dt<<endl;
			if(dt > 1000){

				if(dssdDataVec2.size()> 1){
					//cout<<"sub buffer dssdDataVec size "<<dssdDataVec2.size()<<endl;
					dssdTime =0;
					maxTime =0;
					sedTime =0;
					maxE =0;
					tof = 0;
					for(unsigned int k =0; k < dssdDataVec2.size(); k++){
						//cout<<" toto strip : "<< dssdDataVec2[k].get_strip()<<"  Time "<< dssdDataVec2[k].get_time()<<"  E "<<dssdDataVec2[k].get_energy()<<endl;
						if(dssdDataVec2.at(k).get_strip() ==1000) sedTime = dssdDataVec2[k].get_time();
						else {dssdTime = dssdDataVec2[k].get_time();


							if(dssdDataVec2[k].get_energy() > maxE){
								maxE = dssdDataVec2[k].get_energy();
								maxTime = dssdTime;
							}//if
						}//else


					}//for
					//cout<<"Max E "<<max<<endl;
					tof = TMath::Abs(static_cast<llint> (sedTime - maxTime));
					if (tof > 0 && maxE > 100)h_dssdE_Tof->Fill(maxE, tof);

				}//if size

				dssdDataVec2.clear();
				dssdDataVec2.push_back(dssdDataVec[j]);
			}//if coinc


			else{
				dssdDataVec2.push_back(dssdDataVec[j]);
			}

		}//for looop ends

		//last sub buffer

		if(dssdDataVec2.size()> 1){
			dssdTime =0;
			maxTime =0;
			sedTime =0;
			maxE =0;
			for(unsigned int k =0; k < dssdDataVec2.size(); k++){

				if(dssdDataVec2.at(k).get_strip() ==1000) sedTime = dssdDataVec2[k].get_time();
				else {dssdTime = dssdDataVec2[k].get_time();


					if(dssdDataVec2[k].get_energy() > maxE){
						maxE = dssdDataVec2[k].get_energy();
						maxTime = dssdTime;
					}//if
				}//else
			}
			tof = TMath::Abs(static_cast<llint> (sedTime - maxTime));
			if(tof > 0 && maxE > 100) h_dssdE_Tof->Fill(maxE, tof);
		}//if size


		dssdDataVec2.clear();


		dssdDataVec.clear();
	}
	/*
	   if(dData->get_stripnumber() == 252){
	   dssdDataVec.push_back(dPoint);sed_counter1++;
	//cout<<" Tracker time "<<timestamp <<"  Time "<<Time<<" stripnumber "<<stripnumber<<endl;
	//Timetracker = dPoint.get_time();
	}

	//cout << "tracker " << dssdDataVec[0].get_energyX() << endl;}
	if(dData->get_stripnumber() < 128)
	{
	//Timedssd = dPoint.get_time();
	if(dData->gain_switched())  {
	dssd_counter1++;
	dssdDataVec.push_back(dPoint);
	//cout<<" Fission time "<<timestamp<<" stripnumber "<<stripnumber<<endl;
	}
	}
	*/
	//cout << "delta " << Timetracker - Timedssd << endl;
	/*
	   if(s1->data_merged == 0){
	//dssdEventVec = dEvent->construct(dssdDataVec, h_delT_ff, h_delT_fb, h_delT_bf, h_delT_bb);
	trackerEventVec = trEvent->construct(trackerNumexoDataVec,grUnsortedT, grT, grDT);
	for (std::vector<dssdPixel>::iterator it = trackerEventVec.begin() ; it != trackerEventVec.end(); ++it){				
	h_dssdE_Tof->Fill((*it).get_energyX(), (*it).get_time());
	}



	}
	*/

	if(s1->data_merged == 0){
		/*if(dssdDataVec.size() == s1->buffer_size){
		//dssdEventVec = dEvent->construct(dssdDataVec, h_delT_ff, h_delT_fb, h_delT_bf, h_delT_bb);
		//dssdEventVec = dEvent->construct(dssdDataVec,grUnsortedT, grT, grDT);
		//cout<<"tracker size "<<trackerNumexoDataVec.size()<<"  dssd event size "<<dssdEventVec.size()<<endl;
		for (std::vector<dssdPixel>::iterator it = dssdEventVec.begin() ; it != dssdEventVec.end(); ++it){				
		h_E_frontBack->Fill((*it).get_energyX(), (*it).get_energyY());
		//cout<<"X : "<<(*it).get_X()<<"  Y "<<(*it).get_Y()<<endl;
		h_DSSD_XY_hit->Fill((*it).get_X(), (*it).get_Y());

		//cout << "delta " << Time - Time2 << endl;
		}
		}*/

		// here do the Tracker dssd correlations


		/*			for (std::vector<dssdData>::iterator it = trackerNumexoDataVec.begin() ; it != trackerNumexoDataVec.end(); ++it){				

					cout<<" time "<<
					}i*/



		//dssdEventVec.clear();
		//trackerNumexoDataVec.clear();
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
	int type=commonframe->GetFrameType();
	framesize=fCoboframe->GetFrameSize();
	eventnumber =fCoboframe->GetEventNumber();
	timestamp = (uint64_t)(fCoboframe->GetTimeStamp());
	nvoie=NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL;
	Time = timestamp;// time is in timestamp

	dPoint.set_time(timestamp);
	dPoint.set_strip(1000);
	dPoint.set_energy(0);


	dssdDataVec.push_back(dPoint);

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

	coboidx=fCoboframe->CoboGetCoboIdx();//cout<<"cobo: "<<coboidx<<endl;
	asadidx=fCoboframe->CoboGetAsaIdx();//cout<<"asad "<<asadidx<<endl;
	nbitems=fCoboframe->GetNbItems();//cout<<nbitems<<endl;



	if(coboidx<NB_COBO)
	{				
		/*	char* hPat0=fCoboframe->CoboGetHitPat(0);
			char* hPat1=fCoboframe->CoboGetHitPat(1);
			char* hPat2=fCoboframe->CoboGetHitPat(2);
			char* hPat3=fCoboframe->CoboGetHitPat(3);

			char* mult0=fCoboframe->CoboGetMultip(0);
			char* mult1=fCoboframe->CoboGetMultip(1);
			char* mult2=fCoboframe->CoboGetMultip(2);
			char* mult3=fCoboframe->CoboGetMultip(3);

			char* last0=fCoboframe->CoboGetLastCell(0);
			char* last1=fCoboframe->CoboGetLastCell(1);
			char* last2=fCoboframe->CoboGetLastCell(2);
			char* last3=fCoboframe->CoboGetLastCell(3);


			fCoboframe->SwapInt16((uint16_t*)(mult0));
			fCoboframe->SwapInt16((uint16_t*)(mult1));
			fCoboframe->SwapInt16((uint16_t*)(mult2));
			fCoboframe->SwapInt16((uint16_t*)(mult3));*/
		//MultCoBo[coboidx]+=(uint16_t)(*((uint16_t*)(mult0)))+(uint16_t)(*((uint16_t*)(mult1)))+(uint16_t)(*((uint16_t*)(mult2)))+(uint16_t)(*((uint16_t*)(mult3)));

		//MFMCommonFrame::SwapInt16((uint16_t*)(last0));
		/*fCoboframe->SwapInt16((uint16_t*)(last0));
		  fCoboframe->SwapInt16((uint16_t*)(last1));
		  fCoboframe->SwapInt16((uint16_t*)(last2));
		  fCoboframe->SwapInt16((uint16_t*)(last3));
		  lastCellRead[coboidx][asadidx][0]=*((uint16_t*)(last0));
		  lastCellRead[coboidx][asadidx][1]=*((uint16_t*)(last1));
		  lastCellRead[coboidx][asadidx][2]=*((uint16_t*)(last2));
		  lastCellRead[coboidx][asadidx][3]=*((uint16_t*)(last3));*/


		for(int i1=0;i1<NB_COBO;i1++)
			for(int i2=0;i2<NB_ASAD;i2++)
				for(int i3=0;i3<NB_AGET;i3++)
					for(int i4=0;i4<NB_CHANNEL;i4++)
					{
						//baseline[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;
						amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;
						tmax[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;
						//tstart[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;
						//rt[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;
						//tt[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4] = 0;

						for(int i5=0;i5<NB_SAMPLES;i5++) TRACE[i1][i2][i3][i4][i5]=0;
					}


		short iChan=0;
		short iBuck=0;
		short iAget=0;
		{
			for(unsigned int i2=0;i2<nbitems;i2++)
			{
				{	
					fCoboframe->CoboGetParameters(i2,&sample, &buckidx,&chanidx,&agetidx);
					//cout<<"sample: "<<sample<<"  buckidx : "<<buckidx<<" chanidx : "<<chanidx<<" agetidx: "<<agetidx<<endl; 
					if(type==MFM_COBO_FRAME_TYPE)
					{
						TRACE[coboidx][asadidx][agetidx][chanidx][buckidx]=sample;
					}

					else if(type==MFM_COBOF_FRAME_TYPE)
					{
						TRACE[coboidx][asadidx][agetidx][iChan][iBuck]=sample;

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
		}
		/*for(int i=0;i<NB_AGET;i++)
		  for(int j=0;j<NB_CHANNEL;j++)
		  for(int k=0;k<NB_SAMPLES;k++){if(TRACE[0][0][i][j][k]>3500) cout<<"what the fuck"<<endl;}*/
	}



	//	TreatBaseline();
	TreatPulseGet();	






	//Ana
	hxa->Reset();hya->Reset();
	for(int i=0;i<ny;i++) ampliy[i]=0;
	for(int i=0;i<nx;i++) amplix[i]=0;

	//ICI remplissage amplitude channel detector
	for(int j=68;j<NB_AGET*NB_CHANNEL;j++) 
	{ 
		if(amplitude[j]>threshold && j<136)  
		{
			hyinit->Fill(j-68);
			if(aget1[j-68]!=0 && aget1[j-68]!=1000)
			{
				hy->Fill(aget1[j-68]-1);
				ampliy[aget1[j-68]-1]=amplitude[j];
				starty[aget1[j-68]-1]=tmax[j];
				//	 cout<<aget1[j-68]-1<<endl;

			}
		} 
		if(amplitude[j]>threshold && j>=136)  
		{
			hxinit->Fill(j-136); 			    
		}  
		if(amplitude[j]>threshold && j>=136 && j<204)
			if(aget2[j-136]!=0 && aget2[j-136]!=1000)
			{
				hx->Fill(aget2[j-136]-1);
				amplix[aget2[j-136]-1]=amplitude[j];
				startx[aget2[j-136]-1]=tmax[j];

			}
		if(amplitude[j]>threshold && j>=204)
			if(aget3[j-204]!=0 && aget3[j-204]!=1000)
			{
				hx->Fill(aget3[j-204]-1);
				amplix[aget3[j-204]-1]=amplitude[j];
				startx[aget3[j-204]-1]=tmax[j];				
			}
	}


	//calib
	//sed_calibration(amplix,coefx,nx);
	//sed_calibration(ampliy,coefy,ny);	

	for(int i=0;i<nx;i++) hxa->SetBinContent(i+1,amplix[i]);			  		
	for(int i=0;i<ny;i++) hya->SetBinContent(i+1,ampliy[i]);

	//reconstruction

	//MAX
	int imaxx,imaxy,tmaxx,tmaxy;
	float maxx,maxy;
	max_c(amplix,startx,&maxx,&imaxx,&tmaxx,nx);	
	max_c(ampliy,starty,&maxy,&imaxy,&tmaxy,ny);

	//multiplicity
	int multx,multy;
	float sumx,sumy;
	mult_c(amplix,imaxx,nx,threshold,&multx,&sumx);
	mult_c(ampliy,imaxy,ny,threshold,&multy,&sumy);

	//barycentre
	float barx,bary,barxm,barym;	
	//modif charge X vs Y		
	//   amplix[imaxx]+=0.2*ampliy[imaxy];			  	     
	//   ampliy[imaxy]+=0.2*amplix[imaxx];

	bar_c(amplix,imaxx,npb,threshold,&barx);
	bar_c(ampliy,imaxy,npb,threshold,&bary);

	barxm=barx*2.77;
	barym=bary*2.79;



	//filling histos

	//if(maxy>0 && maxx>0 && maxy<3700 && maxx<3700 && multx>2 && multy>2 && sumx>10)
	//	if(sumx>1000 && sumy>1000) 
	if(true)
		//if(maxy<3700 && maxx<3700)
		//{

	{
		//hcoshxm->Fill(p1x*2.77);
		//hcoshym->Fill(p1y*2.79);
		//hbarcosh2Dm->Fill(p1x*2.77,p1y*2.79);

		hmaxx->Fill(maxx);
		hmaxy->Fill(maxy);

		hmaxx1d->Fill(amplix[imaxx+1]);
		hmaxx1g->Fill(amplix[imaxx-1]);	
		hmaxx2d->Fill(amplix[imaxx+2]);
		hmaxx2g->Fill(amplix[imaxx-2]);	

		hmaxy1d->Fill(ampliy[imaxy+1]);
		hmaxy1g->Fill(ampliy[imaxy-1]);	
		hmaxy2d->Fill(ampliy[imaxy+2]);
		hmaxy2g->Fill(ampliy[imaxy-2]);

		himaxx->Fill(imaxx);
		himaxy->Fill(imaxy);

		htmaxx->Fill(tmaxx);
		htmaxy->Fill(tmaxy);

		hmultx->Fill(multx);
		hmulty->Fill(multy);
		hmult_xy->Fill(multx,multy);
		hsum_xy->Fill(sumx,sumy);
		hmult_xy->Fill(multx,multy);	

		hbarx->Fill(barx);
		hbary->Fill(bary);
		hbarxm->Fill(barxm);	
		hbarym->Fill(barym);
		hbar2Dm->Fill(barxm,barym);


	}
	else cut++;

}


void GUser::TreatPulseGet()
{
	float max=0;
	int imax=0;
	int nag=0;
	int nch=0;


	//calcul max
	for(int i1=0;i1<NB_COBO;i1++)
		for(int i2=0;i2<NB_ASAD;i2++)
			for(int i3=0;i3<NB_AGET;i3++)
				for(int i4=0;i4<NB_CHANNEL;i4++)
				{
					for(int i5=BLCALC;i5<NB_SAMPLES;i5++)
					{

						//if(TRACE[i1][i2][i3][i4][i5]>max || TRACE[i1][i2][i3][i4][i5]>3500)
						if(TRACE[i1][i2][i3][i4][i5]>max)
						{
							max=TRACE[i1][i2][i3][i4][i5];
							imax=i5;
							nag=i3;
							nch=i4;		      
						}
					}
					//cout<<"imax: "<<imax<<" "<<"nch: "<<nch<<endl;
					amplitude[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=max;
					tmax[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=imax;
					naget[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=nag;
					nv[i1*NB_ASAD*NB_AGET*NB_CHANNEL + i2*NB_AGET*NB_CHANNEL + i3*NB_CHANNEL + i4]=nch;
					max=nag=nch=imax=0;
				}
}



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

void GUser::InitUserTTree(char* filename)
{
	treeFile = new TFile(filename, "RECREATE");
	siriusTTree = new TTree("siriusTTree", "siriusTTree");
	siriusTTree->Branch("Time", &Time, "Time/l");
	siriusTTree->Branch("Timestamp", &timestamp, "Timestamp/l");
	siriusTTree->Branch("EventNo",  &eventnumber, "EventNo/i");
	siriusTTree->Branch("TraceSize",  &s1->TRACE_SIZE, "TraceSize/s");
	siriusTTree->Branch("Trace",  dData->get_trace(), "Trace[TraceSize]/s");
	siriusTTree->Branch("Gain",  &gain, "Gain/s");
	siriusTTree->Branch("BoardID",  &board, "BoardID/s");
	siriusTTree->Branch("ChannelID",  &channel, "ChannelID/s");
	siriusTTree->Branch("Energy",  &Energy, "Energy/d");
	siriusTTree->Branch("Baseline",  dData->get_Baseline_address(), "Baseline/d");

	reaGenericTTree = new TTree("reaGenericTTree","reaGenericTTree");	
	reaGenericTTree->Branch("Timestamp", &timestamp, "Timestamp/l");
	reaGenericTTree->Branch("EventNo",  &eventnumber, "EventNo/i");
	reaGenericTTree->Branch("BoardID",  &board, "BoardID/s");
	reaGenericTTree->Branch("ChannelID",  &channel, "ChannelID/s");
	reaGenericTTree->Branch("Energy",  &reaGenericEnergy, "Energy/s");
	reaGenericTTree->Branch("Time",  &reaGenericTime, "Time/s");

	coboTTree = new TTree("coboTTree", "coboTTree");
	coboTTree->Branch("Timestamp", &timestamp, "Timestamp/l");
	coboTTree->Branch("EventNo",  &eventnumber, "EventNo/i");
	/*coboTTree->Branch("nvoie",&nvoie,"nvoie/I");
	  coboTTree->Branch("amplitude",amplitude,"amplitude[nvoie]/F");
	  coboTTree->Branch("tmax",tmax,"tmax[nvoie]/I");
	  coboTTree->Branch("nv",nv,"nv[nvoie]/I");
	  coboTTree->Branch("naget",naget,"naget[nvoie]/I");*/
}


void GUser::SaveUserTTree(){
	if(treeFile){
		treeFile->cd();
		siriusTTree->Write();
		reaGenericTTree->Write();
		coboTTree->Write();
		treeFile->Close();
		cout<<"TTree saved.."<<endl;
	}
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
