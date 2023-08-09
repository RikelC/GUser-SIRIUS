/*!
 * \file GUserHistograms.C
 * \author Rikel Chakme
 *
 */
#include "./GUser.h"

void GUser::CreateHistograms ()
{
	TString name;
	TString title;
	TString famname;
	TString sfamname;

	if(s1->NBOARDS_DSSD >0){	
		std::string dssd_boards_char[s1->NBOARDS_DSSD];
		for(int i = 0; i < s1->NBOARDS_DSSD;i++) dssd_boards_char[i] = std::to_string(s1->boardList_DSSD[i]);

		//hGain        = new TH1I**[NBOARDS];
		//hFeedBack    = new TH1I**[NBOARDS];
		hTrace_Sum     = new TH2F**[s1->NBOARDS_DSSD];
		hRaw           = new TH1F**[s1->NBOARDS_DSSD];
		hCalib         = new TH1F**[s1->NBOARDS_DSSD];
		hTrap          = new TH2F**[s1->NBOARDS_DSSD];
		gr_baseline    = new TGraph**[s1->NBOARDS_DSSD];
		hTrigger       = new TH1F**[s1->NBOARDS_DSSD];
		hBaseline      = new TH1F**[s1->NBOARDS_DSSD];
		hNoise         = new TH1F**[s1->NBOARDS_DSSD];
		hRisetime      = new TH1I**[s1->NBOARDS_DSSD];
		h_zeroCTime    = new TH1F**[s1->NBOARDS_DSSD];
		h_MaxPosTime   = new TH1I**[s1->NBOARDS_DSSD];
		hCFD           = new TH2F**[s1->NBOARDS_DSSD];
		grTimestamp    = new TGraph**[s1->NBOARDS_DSSD];
		evtCounter    = new ullint*[s1->NBOARDS_DSSD];

		for(int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
			//hGain[iboard]       = new TH1I*[s1->nChannel_DSSD[iboard]];
			//hFeedBack[iboard]   = new TH1I*[s1->nChannel_DSSD[iboard]];
			hTrace_Sum[iboard]    = new TH2F*[s1->nChannel_DSSD[iboard]];
			hRaw[iboard]          = new TH1F*[s1->nChannel_DSSD[iboard]];
			hCalib[iboard]        = new TH1F*[s1->nChannel_DSSD[iboard]];
			hTrap[iboard]         = new TH2F*[s1->nChannel_DSSD[iboard]];
			gr_baseline[iboard]   = new TGraph*[s1->nChannel_DSSD[iboard]];
			hTrigger[iboard]      = new TH1F*[s1->nChannel_DSSD[iboard]];
			hBaseline[iboard]     = new TH1F*[s1->nChannel_DSSD[iboard]];
			hNoise[iboard]        = new TH1F*[s1->nChannel_DSSD[iboard]];
			hRisetime[iboard]     = new TH1I*[s1->nChannel_DSSD[iboard]];
			h_zeroCTime[iboard]   = new TH1F*[s1->nChannel_DSSD[iboard]];
			h_MaxPosTime[iboard]  = new TH1I*[s1->nChannel_DSSD[iboard]];
			hCFD[iboard]          = new TH2F*[s1->nChannel_DSSD[iboard]];
			grTimestamp[iboard]    = new TGraph*[s1->nChannel_DSSD[iboard]];
			evtCounter[iboard]    = new ullint[s1->nChannel_DSSD[iboard]];

			famname.Form("Card_%d",s1->boardList_DSSD[iboard]);
			for (int channel =0;channel < s1->nChannel_DSSD[iboard];channel++){
				evtCounter[iboard][channel] = 0;
				/*  name.Form("Gain_%d_%d",boardList[iboard],channel);
				    hGain[iboard][channel] = new TH1I (name.Data(),name.Data(),sizehisto,0,maxhisto);
				    GetSpectra()->AddSpectrum(hGain[iboard][channel],famname);

				    name.Form("FeedBack_%d_%d",boardList[iboard],channel);
				    hFeedBack[iboard][channel] = new TH1I (name.Data(),name.Data(),sizehisto,0,maxhisto);
				    GetSpectra()->AddSpectrum(hFeedBack[iboard][channel],famname);
				    */
				//histogram for viewing traces

				sfamname.Form("%s/ZeroCrTime",famname.Data());
				name.Form("ZeroCrTime%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("ZeroCrTime%d_%d; sample number; Zero Crossing time (in ns)",s1->boardList_DSSD[iboard],channel);
				h_zeroCTime[iboard][channel] = new TH1F (name.Data(),title.Data(),s1->TRACE_SIZE*10,0,s1->TRACE_SIZE);
				GetSpectra()->AddSpectrum(h_zeroCTime[iboard][channel],sfamname);

				sfamname.Form("%s/MaxPosTime",famname.Data());
				name.Form("MaxPosTime%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("MaxPosTime%d_%d; sample number; Position of the Maximum (in sample number)",s1->boardList_DSSD[iboard],channel);
				h_MaxPosTime[iboard][channel] = new TH1I (name.Data(),title.Data(),s1->TRACE_SIZE,0,s1->TRACE_SIZE);
				GetSpectra()->AddSpectrum(h_MaxPosTime[iboard][channel],sfamname);



				sfamname.Form("%s/CFD",famname.Data());
				name.Form("CFD%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("CFD%d_%d; sample number; CFD signal ",s1->boardList_DSSD[iboard],channel);
				hCFD[iboard][channel] = new TH2F (name.Data(),title.Data(),s1->TRACE_SIZE,0,s1->TRACE_SIZE, 1000,-5000,5000);
				GetSpectra()->AddSpectrum(hCFD[iboard][channel],sfamname);

				sfamname.Form("%s/Trace",famname.Data());
				name.Form("Trace_Sum%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("Trace_Sum%d_%d; sample number; Signal height (ADC channel)",s1->boardList_DSSD[iboard],channel);
				hTrace_Sum[iboard][channel] = new TH2F (name.Data(), title.Data(),s1->TRACE_SIZE,0,s1->TRACE_SIZE,1700,-1000,25000);
				GetSpectra()->AddSpectrum(hTrace_Sum[iboard][channel],sfamname);
				//histograms for raw data = trapezodal height
				sfamname.Form("%s/RawHist",famname.Data());
				name.Form("RawData_%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("RawData_%d_%d; Energy (ADC channel); Counts",s1->boardList_DSSD[iboard],channel);
				hRaw[iboard][channel] = new TH1F(name.Data(), title.Data(),20000,0.,20000.);
				GetSpectra()->AddSpectrum(hRaw[iboard][channel],sfamname);
				//histograms for calibrated data = trapezodal height * gain + offset
				sfamname.Form("%s/CalibHist",famname.Data());
				name.Form("CalibData_%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("CalibData_%d_%d; Energy (keV); Counts",s1->boardList_DSSD[iboard],channel);
				hCalib[iboard][channel] = new TH1F(name.Data(), title.Data(),10000,0.,50000.);  // (2000,0.,100000) previously
				GetSpectra()->AddSpectrum(hCalib[iboard][channel],sfamname);
				//
				//histo for trapezoidal
				sfamname.Form("%s/Trapezoidal",famname.Data());
				name.Form("Trap_%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("Trap_%d_%d; sample number; LSB",s1->boardList_DSSD[iboard],channel);
				hTrap[iboard][channel] = new TH2F(name.Data(),title.Data(), s1->TRACE_SIZE, 0,s1-> TRACE_SIZE, 4000,-8000,8000);
				GetSpectra()->AddSpectrum(hTrap[iboard][channel],sfamname);

				//histo for trigger
				sfamname.Form("%s/Trigger",famname.Data());
				name.Form("hTrigger_%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("hTrigger_%d_%d; sample number; Counts",s1->boardList_DSSD[iboard],channel);
				hTrigger[iboard][channel] = new TH1F(name.Data(),title.Data(), 992,0, 992);
				GetSpectra()->AddSpectrum(hTrigger[iboard][channel],sfamname);

				//histo for baseline
				sfamname.Form("%s/BaselineHist",famname.Data());
				name.Form("hBaseline_%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("hBaseline_%d_%d; ADC Channel (LSB); Counts",s1->boardList_DSSD[iboard],channel);
				hBaseline[iboard][channel] = new TH1F(name.Data(),title.Data(), 4000,0,16000);
				GetSpectra()->AddSpectrum(hBaseline[iboard][channel],sfamname);

				//baseline as a function of event number
				sfamname.Form("%s/BaselineGraph",famname.Data());
				name.Form("baseline_%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("baseline_%d_%d; event number; Baseline (LSB)",s1->boardList_DSSD[iboard],channel);
				gr_baseline[iboard][channel] =  new TGraph();
				gr_baseline[iboard][channel]->SetNameTitle(name,title);
				//gr_baseline[iboard][channel]->GetXaxis()->SetTitle("event number");
				//gr_baseline[iboard][channel]->GetYaxis()->SetTitle("baseline");
				GetSpectra()->AddSpectrum( gr_baseline[iboard][channel], sfamname);

				//histo for Noise
				sfamname.Form("%s/Noise",famname.Data());
				name.Form("hNoise%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("hNoise%d_%d; Noise (LSB); Counts",s1->boardList_DSSD[iboard],channel);
				hNoise[iboard][channel] = new TH1F(name.Data(),title.Data(), 1000,0,100);
				GetSpectra()->AddSpectrum(hNoise[iboard][channel],sfamname);
				//histo for Risetime
				sfamname.Form("%s/Risetime",famname.Data());
				name.Form("hRisetime%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("hRisetime%d_%d; Risetime (10 ns); Counts",s1->boardList_DSSD[iboard],channel);
				hRisetime[iboard][channel] = new TH1I(name.Data(),title.Data(), 1000,0,1000);
				GetSpectra()->AddSpectrum(hRisetime[iboard][channel],sfamname);

				// timestamp
				sfamname.Form("%s/TimeStampGraph",famname.Data());
				name.Form("timestamp_%d_%d",s1->boardList_DSSD[iboard],channel);
				title.Form("timestamp_%d_%d; event number; timestamp",s1->boardList_DSSD[iboard],channel);
				grTimestamp[iboard][channel] =  new TGraph();
				grTimestamp[iboard][channel]->SetNameTitle(name,title);
				GetSpectra()->AddSpectrum( grTimestamp[iboard][channel], sfamname);

			}
		}
		//
		famname.Form("DssdSpectra");
		h_strip_trigger = new TH2F("h_strip_trigger"," ;strip number; Leading edge trigger (sample number)", 260,0,260,992,0,992);
		h_strip_zeroCrTime = new TH2F("h_strip_zeroCrTime","; strip number; CFD zero-crossing time (ns)", 260,0,260,992,0,992);
		h_strip_MaxPosTime= new TH2F("h_strip_MaxTime","; strip number; Position of the Maximum (sample number)", 260,0,260,992,0,992);
		h_board_zeroCrTime = new TH2F("h_board_zeroCrTime","; Board number; CFD zero-crossing time (ns)", 50,150,200,992,0,992);
		h_board_MaxPosTime= new TH2F("h_board_MaxTime","; Board number; Position of the Maximum (sample number)", 50,150,200,992,0,992);
		h_strip_SignalHeight = new TH2F("h_strip_SignalHeight","; strip number; signal height (LSB)", 260,0,260,1000,0,10000);

		GetSpectra()->AddSpectrum(h_strip_trigger,famname);
		GetSpectra()->AddSpectrum(h_strip_zeroCrTime,famname);
		GetSpectra()->AddSpectrum(h_strip_MaxPosTime,famname);
		GetSpectra()->AddSpectrum(h_board_zeroCrTime,famname);
		GetSpectra()->AddSpectrum(h_board_MaxPosTime,famname);
		GetSpectra()->AddSpectrum(h_strip_SignalHeight,famname);
		h_E_MaxPosTime = new TH2F("h_E_MaxPosTime", ";Energy; Position of the Maximum (sample number)", 1500,0,15000, 992,0,992);
		h_E_zeroCrTime = new TH2F("h_E_zeroCrTime", "Energy; CFD zero-crossing time(ns)", 1500,0,15000, 992,0,992);
		GetSpectra()->AddSpectrum(h_E_zeroCrTime,famname);
		GetSpectra()->AddSpectrum(h_E_MaxPosTime,famname);
		//- time difference b/w 2 front strips
		h_delT_ff =  new TH1I("h_delT_ff","#Delta T(front1-front2);ns", 1000,-50000,50000);
		GetSpectra()->AddSpectrum(h_delT_ff,famname);
		//- time difference b/w  front and back strips
		h_delT_fb =  new TH1I("h_delT_fb","#Delta T(front - back);ns",1000,-50000,50000);
		GetSpectra()->AddSpectrum(h_delT_fb,famname);
		//- time difference b/w back and front strips
		h_delT_bf =  new TH1I("h_delT_bf","#Delta T(back - front);ns",1000,-50000,50000);
		GetSpectra()->AddSpectrum(h_delT_bf,famname);
		//- time difference b/w 2 back strips
		h_delT_bb =  new TH1I("h_delT_bb","#Delta T(back1 - back2);ns",1000,-50000,50000);
		GetSpectra()->AddSpectrum(h_delT_bb,famname);
		//-raw energy vs strip number
		h_raw_strip = new TH2F("h_raw_strip",";E (ADC ch);strip number", 2000,0,100000,256,0,256);
		GetSpectra()->AddSpectrum( h_raw_strip,famname);
		//-calibrated energy vs strip number
		h_calib_strip = new TH2F("h_calib_strip",";E (keV);strip number", 2000,0,100000,256,0,256);
		GetSpectra()->AddSpectrum( h_calib_strip,famname);
		//-histo for front energy vs back energy
		h_E_frontBack = new TH2F("h_E_frontBack",";frontE;backE",1000,0,100000,1000,0,100000);
		GetSpectra()->AddSpectrum( h_E_frontBack,famname);
		//-hit pattern in the DSSD
		h_DSSD_XY_hit =  new TH2I("h_DSSD_XY_hit","hit pattern ;Front strip (X); Back strip (Y)",128,0,128,128,0,128);
		GetSpectra()->AddSpectrum( h_DSSD_XY_hit,famname);
		//-histo for viewing number of counts in each strip
		h_dssd_count_strip = new TH1I("h_dssd_count_strip",";strip number; counts", s1->NSTRIPS_DSSD,0,s1->NSTRIPS_DSSD);
		GetSpectra()->AddSpectrum( h_dssd_count_strip,famname);
		//histo for viewing counts per board
		h_dssd_count_board = new TH1I("h_dssd_count_board",";board number; counts", s1->NBOARDS_DSSD,0,s1->NBOARDS_DSSD);
		for(int i = 1; i <= s1->NBOARDS_DSSD; i++)h_dssd_count_board->GetXaxis()->SetBinLabel(i, dssd_boards_char[i-1].c_str());
		//	h_dssd_count_board->GetXaxis()->SetLabelSize(0.1);
		GetSpectra()->AddSpectrum( h_dssd_count_board,famname);
		//-event rates in sec in each strip of the DSSD
		h_dssdStrip_rate = new TH1I("h_dssdStrip_rate",Form(";strip number;counts/%d sec",s1->rate_calcul_time_lapse), s1->NSTRIPS_DSSD,0,s1->NSTRIPS_DSSD);
		GetSpectra()->AddSpectrum( h_dssdStrip_rate,famname);
		//-event rates in sec in each Board of the DSSD
		h_dssdBoard_rate = new TH1I("h_dssdBoard_rate",Form(";board number;counts/%d sec",s1->rate_calcul_time_lapse), s1->NBOARDS_DSSD,0,s1->NBOARDS_DSSD);
		for(int i = 1; i <= s1->NBOARDS_DSSD; i++)h_dssdBoard_rate->GetXaxis()->SetBinLabel(i, dssd_boards_char[i-1].c_str());
		//h_dssdBoard_rate->GetXaxis()->SetLabelSize(0.1);	
		GetSpectra()->AddSpectrum( h_dssdBoard_rate,famname);
		//Rates per board graphs
		sfamname.Form("%s/RatePerBoard",famname.Data());
		gr_rate_dssdBoard = new TGraph*[s1->NBOARDS_DSSD];
		for(int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
			sfamname.Form("%s/RatePerBoard",famname.Data());
			name.Form("rate_%d",s1->boardList_DSSD[iboard]);
			gr_rate_dssdBoard[iboard] =  new TGraph();
			gr_rate_dssdBoard[iboard]->SetName(name);
			gr_rate_dssdBoard[iboard]->SetTitle(Form("Rate %d; time(x %d sec);counts/%d sec", s1->boardList_DSSD[iboard], s1->rate_calcul_time_lapse,s1->rate_calcul_time_lapse));
			GetSpectra()->AddSpectrum(gr_rate_dssdBoard[iboard],sfamname);
		}	

		famname.Form("sumSpectra");
		h_sum_front = new TH1F("h_sum_front","h_sum_front; Energy; Counts",2000,0,20000);
		GetSpectra()->AddSpectrum(h_sum_front,famname);
		h_sum_back= new TH1F("h_sum_back","h_sum_back; Energy; Counts",2000,0,20000);
		GetSpectra()->AddSpectrum(h_sum_back,famname);


		famname.Form("CorrelationSpectra");
		h_E_frontsumCorr = new TH1F("h_E_frontsumCorr","h front sum correlated", 2000,0,20000);
		GetSpectra()->AddSpectrum(h_E_frontsumCorr,famname);
		//Tof
		h_strip_Tof1 = new TH2F("h_strip_Tof1"," DSSD vs CoBo;strip number;ToF (x10 ns)", 260,0,260, 1000,-500,500);
		GetSpectra()->AddSpectrum( h_strip_Tof1,famname);
		h_dssdE_Tof1 = new TH2F("h_dssdE_Tof1","DSSD vs CoBo;E;ToF (x10 ns)", 5000,0,30000, 1000,-500,500);
		GetSpectra()->AddSpectrum( h_dssdE_Tof1,famname);
		h_strip_Tof2 = new TH2F("h_strip_Tof2","DSSD vs SeD Numexo2; strip number;ToF (ns)", 260,0,260, 5000,-500,500);
		GetSpectra()->AddSpectrum( h_strip_Tof2,famname);
		h_dssdE_Tof2 = new TH2F("h_dssdE_Tof2","DSSD vs seD Numexo2; E; ToF (ns)", 5000,0,30000, 1000,-500,500);
		GetSpectra()->AddSpectrum( h_dssdE_Tof2,famname);
		h_strip_Tof3 = new TH2F("h_strip_Tof3","DSSD vs SeD Numexo2; strip number;ToF (x10 ns)", 260,0,260, 1000,-500,500);
		GetSpectra()->AddSpectrum( h_strip_Tof3,famname);
		h_dssdE_Tof3 = new TH2F("h_dssdE_Tof3","DSSD vs seD Numexo2; E; ToF (x10 ns)", 5000,0,30000, 1000,-500,500);
		GetSpectra()->AddSpectrum( h_dssdE_Tof3,famname);

		h_dssdEvt_Tof = new TH2F("h_dssdEvt_Tof","DSSD vs CoBo; E; ToF (x10 ns)", 5000,0,30000, 1000,-500,500);
		GetSpectra()->AddSpectrum( h_dssdEvt_Tof,famname);

		h_Esi_Esed = new TH2F("h_Esi_Esed","DSSD vs SeD Numexo2; E Si ; E SeD", 2000,10000,20000, 2000,0,10000);
		GetSpectra()->AddSpectrum( h_Esi_Esed,famname);
		h_strip_Esed = new TH2F("h_strip_Esed","DSSD vs SeD Numexo2; strip number; E SeD",260,0,260, 2000,0,10000);
		GetSpectra()->AddSpectrum( h_strip_Esed,famname);

		h_trackZSeDX = new TH2F("h_trackZSeDX", "Track ZX (X pos in SeD);  Z (cm); X in SeD (cm)",120,0,120,200,-10,10 );
		h_trackZSeDY = new TH2F("h_trackZSeDY", "Track ZY (Y pos in SeD);  Z (cm); Y in SeD (cm)",120,0,120,200,-10,10 );

		GetSpectra()->AddSpectrum( h_trackZSeDX, famname);
		GetSpectra()->AddSpectrum( h_trackZSeDY, famname);

		h_XDistance = new TH2F("h_XDistance", "X SeD Position vs track distance; X in SeD (cm); Track Distance (cm)",200,-10,10,2000,95,105);
		h_YDistance = new TH2F("h_YDistance", "Y SeD Position vs track distance; Y in SeD (cm); Track Distance (cm)",200,-10,10,2000,95,105);
		GetSpectra()->AddSpectrum( h_XDistance, famname);
		GetSpectra()->AddSpectrum( h_YDistance, famname);

		h_DSSD_XY_hit_recoil =  new TH2I("h_DSSD_XY_hit_recoil","hit pattern of recoils ;Front strip (X); Back strip (Y)",128,0,128,128,0,128);
		GetSpectra()->AddSpectrum( h_DSSD_XY_hit_recoil,famname);

		h_DSSD_XYPhysical_hit_recoil =  new TH2I("h_DSSD_XYPhysical_hit_recoil","hit pattern of recoils ;X (cm); Y (cm)",100,-5,5,100,-5,5);
		GetSpectra()->AddSpectrum( h_DSSD_XYPhysical_hit_recoil,famname);

		h_front_traceGS = new TH2F("h_front_traceGS","Front Gain switched traces; sample number; ADC Channel", 992,0,992,1000,0,16000);
		h_back_traceGS = new TH2F("h_back_traceGS","Back Gain switched traces; sample number; ADC Channel", 992,0,992,1000,0,16000);
		GetSpectra()->AddSpectrum(  h_front_traceGS   ,"");
		GetSpectra()->AddSpectrum(  h_back_traceGS   ,"");
		h_front_traceNGS = new TH2F("h_front_traceNGS","Front Gain not switched traces; sample number; ADC Channel", 992,0,992,1000,0,16000);
		h_back_traceNGS = new TH2F("h_back_traceNGS","Back Gain not switched traces; sample number; ADC Channel", 992,0,992,1000,0,16000);
		GetSpectra()->AddSpectrum(  h_front_traceNGS   ,"");
		GetSpectra()->AddSpectrum(  h_back_traceNGS   ,"");
		h_front_rGS = new TH2F("h_front_rGS","", 260,0,260,1000,0,16000);
		h_back_rGS = new TH2F("h_back_rGS","",260,0,260, 1000,0,16000);
		GetSpectra()->AddSpectrum(  h_front_rGS   ,"");
		GetSpectra()->AddSpectrum(  h_back_rGS   ,"");
		h_front_rNGS = new TH2F("h_front_rNGS","", 260,0,260,1000,0,16000);
		h_back_rNGS = new TH2F("h_back_rNGS","", 260,0,260,1000,0,16000);
		GetSpectra()->AddSpectrum(  h_front_rNGS   ,"");
		GetSpectra()->AddSpectrum(  h_back_rNGS   ,"");

	}
	//-----------------------------
	// tunnel
	//------------------------
	if(s1->NBOARDS_TUNNEL >0){	
		std::string tunnel_boards_char[s1->NBOARDS_TUNNEL];
		for(int i = 0; i < s1->NBOARDS_TUNNEL;i++) tunnel_boards_char[i] = std::to_string(s1->boardList_Tunnel[i]);

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
				title.Form("RawData_%d_%d; E (in LSB); Counts",s1->boardList_Tunnel[iboard], channel);
				h_tunnelRaw[iboard][channel] = new TH1I (name.Data(),title.Data(),8000,0,8000);
				GetSpectra()->AddSpectrum(h_tunnelRaw[iboard][channel],sfamname);
				//Calib data
				sfamname.Form("%s/CalibHist",famname.Data());
				name.Form("CalibData_%d_%d",s1->boardList_Tunnel[iboard], channel);
				title.Form("CalibData_%d_%d; E (in KeV); Counts",s1->boardList_Tunnel[iboard], channel);
				h_tunnelCalib[iboard][channel] = new TH1I (name.Data(),title.Data(),8000,0,8000);
				GetSpectra()->AddSpectrum(h_tunnelCalib[iboard][channel],sfamname);

			}
		}



		famname.Form("TunnelSpectra");
		//-raw energy vs strip number
		h_raw_pad = new TH2F("h_raw_pad",";E (ADC ch);pad number", 2000,0,100000,96,0,96);
		GetSpectra()->AddSpectrum( h_raw_pad,famname);
		//-calibrated energy vs strip number
		h_calib_pad = new TH2F("h_calib_pad",";E (keV);pad number", 2000,0,100000,96,0,96);
		GetSpectra()->AddSpectrum( h_calib_pad,famname);
		//-hit pattern in the TUNNEL

		h_TUNNEL_XY_hit = new TH2I*[s1->NDETECTOR_TUNNEL];
		for(int i =0; i < s1->NDETECTOR_TUNNEL;i++){
			name.Form("h_tunnel%d_XY_hit",i+1);
			h_TUNNEL_XY_hit[i] =  new TH2I(name,Form("Tunnel %d;X;Y",i+1),8,0,8,8,0,8);
			GetSpectra()->AddSpectrum( h_TUNNEL_XY_hit[i],famname);
		}
		//-histo for viewing number of counts in each pad
		h_tunnel_count_pad = new TH1I("h_tunnel_count_pad",";pad number; counts", s1->NofMacroPixels,0,s1->NofMacroPixels);
		GetSpectra()->AddSpectrum( h_tunnel_count_pad,famname);
		//histo for viewing counts per board
		h_tunnel_count_board = new TH1I("h_tunnel_count_board",";board number; counts", s1->NBOARDS_TUNNEL,0,s1->NBOARDS_TUNNEL);
		for(int i = 1; i <= s1->NBOARDS_TUNNEL; i++)h_tunnel_count_board->GetXaxis()->SetBinLabel(i, tunnel_boards_char[i-1].c_str());
		//	h_tunnel_count_board->GetXaxis()->SetLabelSize(0.1);
		GetSpectra()->AddSpectrum( h_tunnel_count_board,famname);


		//-event rates in sec in each strip of the DSSD
		h_tunnelPad_rate = new TH1I("h_tunnelPad_rate",Form(";pad number;counts/%d sec",s1->rate_calcul_time_lapse), s1->NofMacroPixels,0,s1->NofMacroPixels);
		GetSpectra()->AddSpectrum( h_tunnelPad_rate,famname);

		//-event rates in sec in each Board of the DSSD
		h_tunnelBoard_rate = new TH1I("h_tunnelBoard_rate",Form(";board number;counts/%d sec",s1->rate_calcul_time_lapse), s1->NBOARDS_TUNNEL,0,s1->NBOARDS_TUNNEL);
		for(int i = 1; i <= s1->NBOARDS_TUNNEL; i++)h_tunnelBoard_rate->GetXaxis()->SetBinLabel(i, tunnel_boards_char[i-1].c_str());
		//h_tunnelBoard_rate->GetXaxis()->SetLabelSize(0.1);	
		GetSpectra()->AddSpectrum( h_tunnelBoard_rate,famname);
		//Rate per board graphs
		gr_rate_tunnelBoard = new TGraph*[s1->NBOARDS_TUNNEL];
		sfamname.Form("%s/RatePerBoard",famname.Data());
		for(int iboard = 0;iboard <s1->NBOARDS_TUNNEL;iboard++){
			name.Form("rate_%d",s1->boardList_Tunnel[iboard]);
			gr_rate_tunnelBoard[iboard] =  new TGraph();
			gr_rate_tunnelBoard[iboard]->SetName(name);
			gr_rate_tunnelBoard[iboard]->SetTitle(Form("Rate %d; time(x %d sec);counts/%d sec", s1->boardList_Tunnel[iboard], s1->rate_calcul_time_lapse,s1->rate_calcul_time_lapse));
			GetSpectra()->AddSpectrum(gr_rate_tunnelBoard[iboard],sfamname);
		}

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
	}

	//histo definitions for SeD
	famname.Form("SedSpectra");
	hx=new TH1F("hx1","X",NX,0,NX);
	GetSpectra()->AddSpectrum(hx,famname);
	hy=new TH1F("hy1","Y",NY,0,NY);
	GetSpectra()->AddSpectrum(hy,famname);
	hyinit=new TH1F("hyinit","Y bin init",68,0,68);	
	GetSpectra()->AddSpectrum(hyinit,famname);
	hxinit=new TH1F("hxinit","X bin init",136,0,136);
	GetSpectra()->AddSpectrum(hxinit,famname);
	hxa=new TH1F("hxa","X",NX,0,NX);
	GetSpectra()->AddSpectrum(hxa,famname);
	hya=new TH1F("hya","Y",NY,0,NY); 
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

	himaxx=new TH1F("himaxx","",NX,0,NX);
	GetSpectra()->AddSpectrum(  himaxx   ,famname);
	himaxy=new TH1F("himaxy","",NY,0,NY);  
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

	hbar2DmP=new TH2F("bar2DmP","XvsY bar[mm]",1350,-200,200,1530,-170,170);
	GetSpectra()->AddSpectrum(  hbar2DmP   ,famname);
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

void GUser::ResetHistograms(){
	if(s1->NBOARDS_DSSD >0){
		for(int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
			for (int channel =0;channel <s1->nChannel_DSSD[iboard];channel++){
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
	}
	if(s1->NBOARDS_TUNNEL >0){
		for(int iboard = 0;iboard < s1->NBOARDS_TUNNEL;iboard++){
			for (int channel =0;channel <s1->nChannel_DSSD[iboard];channel++){
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
}

void GUser::DeleteHistograms()  {
	//-----------------------
	//delete histogram pointers
	//------------------------
	if(s1->NBOARDS_DSSD >0){
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

	}
	if(s1->NBOARDS_TUNNEL >0){
		delete [] gr_rate_tunnelBoard;
		for(int i = 0; i < s1->NBOARDS_TUNNEL; i++){
			delete [] h_tunnelRaw[i];
			delete [] h_tunnelCalib[i];
		}
		delete [] h_tunnelRaw;
		delete [] h_tunnelCalib;
		delete [] h_TUNNEL_XY_hit;
	}
}

