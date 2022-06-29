// File : GUser.C
// Author: Luc Legeard
//////////////////////////////////////////////////////////////////////////////
//
// Class GUser
//
// Class for User treatment
//
////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// ***************************************************************************
// *                                                                         *
// *   This program is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// ***************************************************************************/
// modified by: Rikel CHAKMA
//
#include "./GUser.h"
#include "TROOT.h"
#include <TProfile.h>
#include "GEventMFM.h"

ClassImp (GUser);
//---------------------------------------------------
// functions for sorting algorithms in energy and time
// ----------------------------------------------------
Bool_t compareEnergy(DataPoint i, DataPoint j)
{
	Double_t Ei, Ej;
	Ei = i.energy;
	Ej = j.energy;
	return (Ei > Ej);
}
Bool_t compareTime(DataPoint i, DataPoint j)
{
	ULong64_t Ti, Tj;
	Ti = i.time;
	Tj = j.time;
	return (Ti < Tj);
}
//-------------------------
// GUser Constructor
//--------------------------
GUser::GUser (GDevice* _fDevIn, GDevice* _fDevOut):GAcq(_fDevIn,_fDevOut)
{

	s1   = myGlobal::getInstance();
	fCoboframe    = new MFMCoboFrame();
	fInsideframe  = new MFMCommonFrame();
	fMergeframe   = new MFMMergeFrame();
	fSiriusframe  = new MFMSiriusFrame();
	fGenericframe = new MFMReaGenericFrame();

	filter = new digitalFilters();
	cfd = new digitalCFD();
	dData = new dssdData();
	dumpsize = 0;
	framesize =0;
	maxdump =128;
	channel=0;
	board=0;
	iboard=0;
	NbItems =0;
	value = 0;
}
//------------------------------
//GUser Destructor
//------------------------------
GUser::~GUser()  {
	cout<<"GUser Desctructor called"<<endl;
	if (fInsideframe) delete (fInsideframe);
	if (fMergeframe)  delete (fMergeframe);
	if (fSiriusframe)  delete (fSiriusframe);
	if (fGenericframe) delete fGenericframe;
	if (fCoboframe)   delete (fCoboframe);
	if(filter)delete filter;
	if(cfd)delete cfd;
	if(dData)delete dData;
	gROOT->cd();

}
//--------------------------------------------
// Initialisation for global  user treatement
//--------------------------------------------
void GUser::InitUser()
{
	//frame counter initialization
	for(int i = 0; i < 8;i++)frameCounter[i] =0;

	//set the DSSD gain in pF
	filter->set_DSSD_gain(s1->FPCSA_GAIN);
	//--------------
	dataSet_counter = 0.;
	for(int i =0; i< 256;i++){
		dssd_rate_counter[i] =0.;
		dssd_rate_CMA[i] = 0.;
	}

	dssd_event_counter = new unsigned long long int*[s1->NBOARDS_DSSD];

	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		dssd_event_counter[i] = new unsigned long long int[NCHANNELS];
	}

	//initialize some preset value
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		for(int j = 0; j < NCHANNELS; j++){
			dssd_event_counter[i][j] =0;
		}
	}
	TString name;
	TString famname;

	//
	// hGain      = new TH1I**[NBOARDS];
	//hFeedBack  = new TH1I**[NBOARDS];
	hTrace_Sum = new TH2F**[s1->NBOARDS_DSSD];
	hRaw    = new TH1F**[s1->NBOARDS_DSSD];
	hTrap = new TH2F**[s1->NBOARDS_DSSD];
	gr_baseline = new TGraph**[s1->NBOARDS_DSSD];
	for(int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
		//  hGain[iboard]     = new TH1I*[NUMEXO_NB_CHANNELS];
		//hFeedBack[iboard] = new TH1I*[NUMEXO_NB_CHANNELS];
		hTrace_Sum[iboard]= new TH2F*[NUMEXO_NB_CHANNELS];
		hRaw[iboard]    = new TH1F*[NUMEXO_NB_CHANNELS];
		hTrap[iboard]   = new TH2F*[NUMEXO_NB_CHANNELS];
		gr_baseline[iboard] = new TGraph*[NUMEXO_NB_CHANNELS];
		//
		famname.Form("Card_%d",s1->fBoardList_DSSD[iboard]);

		//
		for (int channel =0;channel <NUMEXO_NB_CHANNELS;channel++){

			/*  name.Form("Gain_%d_%d",fBoardList[iboard],channel);
			    hGain[iboard][channel] = new TH1I (name.Data(),name.Data(),sizehisto,0,maxhisto);
			    GetSpectra()->AddSpectrum(hGain[iboard][channel],famname);

			    name.Form("FeedBack_%d_%d",fBoardList[iboard],channel);
			    hFeedBack[iboard][channel] = new TH1I (name.Data(),name.Data(),sizehisto,0,maxhisto);
			    GetSpectra()->AddSpectrum(hFeedBack[iboard][channel],famname);
			    */
			//histogram for viewing traces
			name.Form("Trace_Sum%d_%d",s1->fBoardList_DSSD[iboard],channel);

			hTrace_Sum[iboard][channel] = (TH2F*)gROOT->Get(name);
			if(!hTrace_Sum[iboard][channel]) hTrace_Sum[iboard][channel] = new TH2F (name.Data(),name.Data(),s1->TRACE_SIZE,0,s1->TRACE_SIZE,1700,-1000,16000);
			else hTrace_Sum[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hTrace_Sum[iboard][channel],famname);
			//histograms for raw data = trapezodal height
			name.Form("RawData_%d_%d",s1->fBoardList_DSSD[iboard],channel);
			hRaw[iboard][channel] = (TH1F*)gROOT->Get(name);
			if(!hRaw[iboard][channel]) hRaw[iboard][channel] = new TH1F(name.Data(), name.Data(),2000,0.,8000.);
			else hRaw[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hRaw[iboard][channel],famname);
			//histo for trapezoidal
			name.Form("Trap_%d_%d",s1->fBoardList_DSSD[iboard],channel);
			hTrap[iboard][channel] = (TH2F*) gROOT->Get(name);
			if(!hTrap[iboard][channel]) hTrap[iboard][channel] = new TH2F(name.Data(),name.Data(), s1->TRACE_SIZE, 0,s1-> TRACE_SIZE, 4000,-8000,8000);
			else hTrap[iboard][channel]->Reset();
			GetSpectra()->AddSpectrum(hTrap[iboard][channel],famname);
			//baseline as a function of event number
			name.Form("Baseline_%d_%d",s1->fBoardList_DSSD[iboard],channel);
			gr_baseline[iboard][channel] = (TGraph*)gROOT->Get(name);
			if(!gr_baseline[iboard][channel]) {gr_baseline[iboard][channel] =  new TGraph();
				name.Form("baseline_%d_%d",s1->fBoardList_DSSD[iboard],channel);
				gr_baseline[iboard][channel]->SetNameTitle(name,name);
			}
			//gr_baseline[iboard][channel]->GetXaxis()->SetTitle("event number");
			//gr_baseline[iboard][channel]->GetYaxis()->SetTitle("baseline");
			GetSpectra()->AddSpectrum( gr_baseline[iboard][channel],famname);

		}
	}

	//
	famname.Form("spectra");
	//- time difference b/w 2 front strips
	h_delT_ff = (TH1I*)gROOT->Get("h_delT_ff");
	if(!h_delT_ff)
		h_delT_ff =  new TH1I("h_delT_ff","#Delta T(front1-front2);ns", 1000,-50000,50000);
	else h_delT_ff->Reset();
	GetSpectra()->AddSpectrum(h_delT_ff,famname);
	//- time difference b/w  front and back strips
	h_delT_fb = (TH1I*)gROOT->Get("h_delT_fb");
	if(!h_delT_fb)
		h_delT_fb =  new TH1I("h_delT_fb","#Delta T(front - back);ns",1000,-50000,50000);
	else h_delT_fb->Reset();
	GetSpectra()->AddSpectrum(h_delT_fb,famname);
	//- time difference b/w back and front strips
	h_delT_bf = (TH1I*)gROOT->Get("h_delT_bf");
	if(!h_delT_bf)
		h_delT_bf =  new TH1I("h_delT_bf","#Delta T(back - front);ns",1000,-50000,50000);
	else  h_delT_bf->Reset();
	GetSpectra()->AddSpectrum(h_delT_bf,famname);
	//- time difference b/w 2 back strips
	h_delT_bb = (TH1I*)gROOT->Get(" h_delT_bb");
	if(!h_delT_bb)
		h_delT_bb =  new TH1I("h_delT_bb","#Delta T(back1 - back2);ns",1000,-50000,50000);
	else h_delT_bb->Reset();
	GetSpectra()->AddSpectrum(h_delT_bb,famname);

	//-histo for front energy vs back energy
	h_E_frontBack = (TH2F*)gROOT->Get("h_E_frontBack");
	if(!h_E_frontBack)h_E_frontBack = new TH2F("h_E_frontBack",";frontE;backE",1000,0,10000,1000,0,10000);
	else h_E_frontBack->Reset();
	GetSpectra()->AddSpectrum( h_E_frontBack,famname);
	//-hit pattern in the DSSD
	h_DSSD_XY_hit = (TH2I*)gROOT->Get("h_DSSD_XY_hit");
	if(!h_DSSD_XY_hit)
		h_DSSD_XY_hit =  new TH2I("h_DSSD_XY_hit","hit pattern ;X;Y",128,0,128,128,0,128);
	else h_DSSD_XY_hit->Reset();
	GetSpectra()->AddSpectrum( h_DSSD_XY_hit,famname);
	//-histo for viewing number of counts in each strip
	h_dssd_count = (TH1I*)gROOT->Get("h_dssd_count");
	if(!h_dssd_count) h_dssd_count = new TH1I("DSSD_count",";strip number; counts", 256+ s1->DSSD_FrontBackGap,0,256+s1->DSSD_FrontBackGap);
	else h_dssd_count->Reset();
	GetSpectra()->AddSpectrum( h_dssd_count,famname);
	//-calibrated energy vs strip number
	h_calib_strip = (TH2F*)gROOT->Get("h_calib_strip");
	if(! h_calib_strip)h_calib_strip = new TH2F("Ecalib_vs_stripNo",";E (keV);strip number", 1000,0,10000,256+s1->DSSD_FrontBackGap,0,256+s1->DSSD_FrontBackGap);
	else  h_calib_strip->Reset();
	GetSpectra()->AddSpectrum( h_calib_strip,famname);
	//-raw energy vs strip number
	h_raw_strip = (TH2F*)gROOT->Get("h_raw_strip");
	if(!h_raw_strip) h_raw_strip = new TH2F("Eraw_vs_stripNo",";E (ADC ch);strip number", 2000,0,10000,256+s1->DSSD_FrontBackGap,0,256+s1->DSSD_FrontBackGap);
	else h_raw_strip->Reset();
	GetSpectra()->AddSpectrum( h_raw_strip,famname);
	//-event rates in sec in each strip of the DSSD
	h_dssd_rate = (TH1I*)gROOT->Get("h_dssd_rate");
	if(!h_dssd_rate)  
		h_dssd_rate = new TH1I("DSSD_rate",";strip number;events/s", 256+s1->DSSD_FrontBackGap,0,256+s1->DSSD_FrontBackGap);
	else h_dssd_rate->Reset();
	GetSpectra()->AddSpectrum( h_dssd_rate,famname);

	//---------tunnel
	h_tunnel = new TH1I**[s1->NBOARDS_TUNNEL];
	for(int iboard = 0;iboard < s1->NBOARDS_TUNNEL;iboard++){
		h_tunnel[iboard]    = new TH1I*[NUMEXO_NB_CHANNELS];
		famname.Form("Card_%d",s1->fBoardList_Tunnel[iboard]);
		for (int channel =0;channel <NUMEXO_NB_CHANNELS;channel++){
			name.Form("RawData_%d_%d",s1->fBoardList_Tunnel[iboard], channel);
			h_tunnel[iboard][channel] = new TH1I (name.Data(),name.Data(),8000,0,8000);
			GetSpectra()->AddSpectrum(h_tunnel[iboard][channel],famname);
		}
	}  

}


//---------------------------------
// Initializarion for each run
//---------------------------------

void GUser::InitUserRun()
{

	dataSet_counter = 0.;
	for(int i =0; i< 256;i++){
		dssd_rate_counter[i] =0.;
		dssd_rate_CMA[i] = 0.;
	}

	for (int iboard = 0;iboard <s1->NBOARDS_DSSD;iboard++){
		for (int channel =0;channel <NUMEXO_NB_CHANNELS;channel++){
			/*
			   hGain[iboard][channel]->Reset();
			   hFeedBack[iboard][channel]->Reset();
			   */
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
	h_dssd_count->Reset();
	h_calib_strip->Reset();
	h_raw_strip->Reset();
	h_dssd_rate->Reset();
}
//---------------------------
// end of each run
//------------------------

void GUser::EndUserRun()
{
	cout<< "calling GUser::EndUserRun()"<<endl;
	reconstruct_events(dataVec);
}
//--------------------
// End of global run
// must be explicitly called!
//-------------------
void GUser::EndUser()
{
	cout<<"####################end of run "<<endl;

	for(int i = 0; i < s1->NBOARDS_DSSD; i++){

		delete [] dssd_event_counter[i];
	}

	delete [] dssd_event_counter;

	//delete histogram pointers
	for(int i = 0; i < s1->NBOARDS_DSSD; i++){
		//  delete [] hGain[i];
		//delete [] hFeedBack[i];
		delete [] hTrace_Sum[i];
		delete [] hRaw[i];
		delete [] hTrap[i];
		delete [] gr_baseline[i];
	}
	//delete [] hGain;
	//delete [] hFeedBack;
	delete [] hTrace_Sum;
	delete [] hRaw;
	delete [] hTrap;
	delete [] gr_baseline;

	dataVec.clear();
	myEvent.clear();
	myEvent_front.clear();
	myEvent_back.clear();

	for(int i = 0; i < 8;i++)cout<<"Frame: "<<s1->frameName[i] <<"  no: "<<frameCounter[i]<<endl;

	cout<< "calling GUser::EndUser"<<endl;
}

//------------------------------
// Read frames
//-----------------------------
void GUser::User()
{
	MFMCommonFrame * cf;
	static long int nb=0 , nberror=0;
	nb++;
	cf = (MFMCommonFrame*)(((GEventMFM*)GetEvent())->GetFrame());
	int type = cf->GetFrameType();
	switch (type) {
		case MFM_COBOF_FRAME_TYPE:
		case MFM_COBO_FRAME_TYPE: {
						  //      printf(" It is a Cobo frame\n");
						  frameCounter[0]++;
						  break;
					  }
		case MFM_EBY_EN_FRAME_TYPE:
		case MFM_EBY_TS_FRAME_TYPE:
		case MFM_EBY_EN_TS_FRAME_TYPE: {
						       //printf(" It is a Eby frame\n");
						       frameCounter[1]++;
						       break;
					       }
		case MFM_HELLO_FRAME_TYPE: {
						   //      printf(" It is a Hello frame\n");
						   frameCounter[2]++;
						   break;
					   }
		case MFM_XML_FILE_HEADER_FRAME_TYPE: {
							     //printf(" It is a Header information frame\n");
							     frameCounter[3]++;
							     break;
						     }

		case MFM_REA_TRACE_FRAME_TYPE: {
						       //printf(" It is a Header information frame\n");
						       frameCounter[4]++;
						       break;
					       }
		case MFM_REA_GENE_FRAME_TYPE: {
						      // printf(" It is a Header information frame\n");
						      frameCounter[5]++;
						      break;
					      }
		case MFM_SIRIUS_FRAME_TYPE: {
						    //ReadSiriusFrame(commonframe);
						    frameCounter[6]++;
						    //      cf->HeaderDisplay();
						    //cf->DumpRaw(32,0);
						    break;
					    }

		default: {
				 //cf->HeaderDisplay();
				 //cf->DumpRaw(32,0);
				 nberror++;
				 //printf(" FT=%d?? %ld / %ld",type,nberror, nb);
				 frameCounter[7]++;
				 break;
			 }

	}// end of switch


	if (cf == NULL) {
		fError.TreatError(1,0,"in GUser::User() return null frfReduce_factorame in GetEvent())->GetFrame()");
	}else {
		UserFrame(cf);
	}
}

//-------------------------------
//  Treatment of merged frames
//--------------------------------
void GUser::UserMergeFrame(MFMCommonFrame* commonframe){
	int i_insframe=0;
	int nbinsideframe =0;
	int dumpsize = 16;

	fMergeframe->SetAttributs(commonframe->GetPointHeader());

	nbinsideframe = fMergeframe->GetNbItems();
	framesize= fMergeframe->GetFrameSize();
	if (fVerbose>3){
		fMergeframe->HeaderDisplay();
	}
	if (fVerbose>5){
		if (framesize < maxdump) dumpsize = framesize;
		else dumpsize = maxdump;
		fMergeframe->DumpRaw(dumpsize, 0);
	}
	fMergeframe->ResetReadInMem();

	for(i_insframe = 0; i_insframe < nbinsideframe; i_insframe++) {
		fMergeframe->ReadInFrame(fInsideframe);
		UserFrame(fInsideframe);

	}

	// At this point you can do treatement inter frames


}
//-------------------------
//
//--------------------------
void GUser::UserFrame(MFMCommonFrame* commonframe){

	if (fVerbose >2)
		cout <<" ------------General Event----------------\n";

	int type =  commonframe->GetFrameType();

	if ((type == MFM_MERGE_EN_FRAME_TYPE)or(type == MFM_MERGE_TS_FRAME_TYPE)) {
		UserMergeFrame(commonframe);
	}  

	if ((type == MFM_COBO_FRAME_TYPE) or (type== MFM_COBOF_FRAME_TYPE)) {
		UserCoboFrame(commonframe);
	}

	if (type == MFM_SIRIUS_FRAME_TYPE){
		UserSiriusFrame(commonframe);
	}

	if(type == MFM_REA_GENE_FRAME_TYPE){
		UserGenericFrame(commonframe);
	}

}
//--------------------------------
//  Treatment of Generic frames
//---------------------------------
void GUser::UserGenericFrame(MFMCommonFrame* commonframe)
{

	fGenericframe->SetAttributs(commonframe->GetPointHeader());
	framesize=commonframe->GetFrameSize();
	channel =fGenericframe->GetChannelId();
	board =  fGenericframe->GetBoardId();
	iboard =  s1->fConvertNoBoardIndexLocal_Tunnel[board];
	value = fGenericframe->GetEnergy();
	timestamp = fGenericframe->GetTimeStamp();
	h_tunnel[iboard][channel]->Fill (value);

	if(s1->fverbose > 0){
		cout<<"-------Generic Frame-------\n";
		printf (" board = %d , channel =%d , energy=%d , timestamp = %lu\n", board, channel, value, timestamp);
	}
	if (fVerbose>5){
		if (framesize < maxdump) dumpsize = framesize;else dumpsize = maxdump;
		commonframe->DumpRaw(dumpsize, 0);
	}
}

//----------------------------------
// Treatment of Sirius frames
// ----------------------------------
void GUser::UserSiriusFrame( MFMCommonFrame* commonframe){
	fSiriusframe->SetAttributs(commonframe->GetPointHeader());
	framesize =fSiriusframe->GetFrameSize();
	channel = (int)fSiriusframe->GetChannelId();

	board = (int)fSiriusframe->GetBoardId();
	iboard =  (int)s1->fConvertNoBoardIndexLocal[board];

	timestamp = fSiriusframe->GetTimeStamp();
	eventnumber = fSiriusframe->GetEventNumber();
	gain = fSiriusframe->GetGain();

	dData->set_channelID(channel);
	dData->set_boardID(board);
	dData->set_boardIdx(iboard);
	dData->set_timestamp( timestamp);
	dData->set_eventnumber( eventnumber);
	dData->set_gain(gain);
	int stripnumber = dData->get_DSSD_strip_number(&board, &channel);


	if(s1->fverbose > 1){
		cout<<"-------Sirius Frame-------\n";
		cout<<"boardId:  "<<board<<" , channel:  "<<channel<<"  strip "<<stripnumber<<" TimeStamp:  "<<timestamp<<" , EventNumber: "<<eventnumber<<endl;


	}
	// hGain[iboard][channel]->Fill(gain);
	// hFeedBack[iboard][channel]->Fill(fSiriusframe->GetFeedBack(0));
	// NbItems= fSiriusframe->GetNbItems();
	for (int i = 0; i < 992 - s1->nEnd_trace; i++) {


		fSiriusframe->GetParameters(i+s1->nStart_trace, &value);
		hTrace_Sum[iboard][channel]->Fill(i+1,value);
		if(i < s1->TRACE_SIZE)
			dData->set_trace_value(i, value);
	}

	treat_sirius_frames();
}
//--------------------------------
//  Treatment of Cobo frames
//---------------------------------
void GUser::UserCoboFrame(MFMCommonFrame* commonframe){
	fCoboframe->SetAttributs(commonframe->GetPointHeader());
	framesize=fCoboframe->GetFrameSize();
	eventnumber =fCoboframe->GetEventNumber();
	timestamp = (uint64_t)(fCoboframe->GetTimeStamp());
	if(s1->fverbose >0){
		cout<<"-------Cobo Frame-------\n";
		cout << " EN = "<<eventnumber <<" TS = " << timestamp <<endl;
	}
	if (fVerbose>3){
		fCoboframe->HeaderDisplay();
	}
	if (fVerbose>5){
		if (framesize < maxdump) dumpsize = framesize;
		else dumpsize = maxdump;
		fCoboframe->DumpRaw(dumpsize, 0);
	}

}

//------------------------------
// Setting up Root Tree
//--------------------------------
void GUser::InitTTreeUser()
{
	GetTree()->SetName("rawDataTree");
	GetTree()->Branch("Time", &timestamp, "Time/l");
	GetTree()->Branch("EventNo",  &eventnumber, "EventNo/i");
	GetTree()->Branch("Trace",  dData->get_trace(), "Trace[991]/s");
	GetTree()->Branch("Gain",  &gain, "Gain/s");
	GetTree()->Branch("BoardID",  &board, "BoardID/s");
	GetTree()->Branch("ChannelID",  &channel, "ChannelID/s");
}

void GUser::treat_sirius_frames(){
	dData->GetSignalInfo();

	//get trapezoidal amplitude
	if(s1->filterAlgorithm == "Trap1"){
		if(s1->useDefaultFilterParameters)
			filt_Amplitude = filter->trapezoidal_filter_algorithm1( dData,s1->default_trap_k, s1->default_trap_m, hTrap[iboard][channel]);
		else
			filt_Amplitude = filter->trapezoidal_filter_algorithm1( dData, hTrap[iboard][channel]);
	}
	else if(s1->filterAlgorithm == "Trap2"){
		if(s1->useDefaultFilterParameters)
			filt_Amplitude = filter->trapezoidal_filter_algorithm2( dData,s1->default_trap_k, s1->default_trap_m, hTrap[iboard][channel]);
		else
			filt_Amplitude = filter->trapezoidal_filter_algorithm2( dData, hTrap[iboard][channel]);
	}
	else if(s1->filterAlgorithm == "Trap3"){
		if(s1->useDefaultFilterParameters)
			filt_Amplitude = filter->trapezoidal_filter_algorithm3( dData,s1->default_trap_k, s1->default_trap_m, hTrap[iboard][channel]);
		else
			filt_Amplitude = filter->trapezoidal_filter_algorithm3( dData, hTrap[iboard][channel]);
	}

	else if(s1->filterAlgorithm == "Trap4"){
		if(s1->useDefaultFilterParameters)
			filt_Amplitude = filter->trapezoidal_filter_algorithm4( dData,s1->default_trap_k, s1->default_trap_m, hTrap[iboard][channel]);
		else
			filt_Amplitude = filter->trapezoidal_filter_algorithm4( dData, hTrap[iboard][channel]);
	}



	hRaw[iboard][channel]->Fill(filt_Amplitude);
	//cout<<"filt Amplitude "<<filt_Amplitude<<endl;

	if(dData->get_stripnumber() < 128)
		h_raw_strip->Fill(filt_Amplitude, dData->get_stripnumber());
	else
		h_raw_strip->Fill(filt_Amplitude, dData->get_stripnumber()+s1->DSSD_FrontBackGap);
	//get CFD time
	cfd_time = (timestamp * timestamp_unit) - (s1->pre_trig_buffer*sampling_period) + static_cast<ULong64_t>( cfd->getCFD_Time2(dData));
	if(s1->fverbose > 1)
		cout<<"Trap amplitude: "<<filt_Amplitude<<"  cfd corrected time: "<<cfd_time<<endl;


	//Fill baseline graphs
	gr_baseline[iboard][channel]->SetPoint(dssd_event_counter[iboard][channel],dssd_event_counter[iboard][channel],dData->get_Baseline());
	dssd_event_counter[iboard][channel]++;


	//calibrate
	//filt_Amplitude = calib->perform(dData);

	if(dData->get_stripnumber() < 128){  
		h_calib_strip->Fill(filt_Amplitude,dData->get_stripnumber()+ s1->DSSD_FrontBackGap);
		h_dssd_count->Fill(dData->get_stripnumber() );
	}else{
		h_calib_strip->Fill(filt_Amplitude,dData->get_stripnumber()+s1->DSSD_FrontBackGap);
		h_dssd_count->Fill(dData->get_stripnumber()+s1->DSSD_FrontBackGap);
	}
	//Fill data set
	if(cfd_time < 1E15){
		//----this condition ensures that the time stamp is ok
		dPoint.time = cfd_time;
		dPoint.strip= dData->get_stripnumber();
		dPoint.energy=filt_Amplitude;
		dataVec.push_back(dPoint);
		if(dataVec.size() == 200){
			reconstruct_events(dataVec);
		}

	}

}

void GUser::reconstruct_events(std::vector<DataPoint> &dataSet){

	if(!dataSet.empty()){
		//---------sort the events in time-----------
		std::sort(dataSet.begin(), dataSet.end(),compareTime);
		//--------------count no of events------------
		double time_diff = static_cast<double>(dataSet.back().time -  dataSet.front().time);//in ns
		//convert to sec
		time_diff = time_diff * 1E-9;
		dataSet_counter++;
		//reset dssd rate counter
		for(int n =0; n<256;n++){dssd_rate_counter[n] =0;}
		//
		for(UShort_t i = 0;i < dataSet.size();i++){
			dssd_rate_counter[dataSet[i].strip]++;
		}
		//
		for(UShort_t i = 0; i < 256;i++){
			dssd_rate_counter[i] = dssd_rate_counter[i]/time_diff;
			//do cumulative moving average here
			dssd_rate_CMA[i] = (dssd_rate_counter[i] + (dataSet_counter - 1)*dssd_rate_CMA[i])/ dataSet_counter;
			if(i < 128)
				h_dssd_rate->SetBinContent(i+1, dssd_rate_CMA[i]);
			else
				h_dssd_rate->SetBinContent(i+1+ s1->DSSD_FrontBackGap, dssd_rate_CMA[i]);

		}

		//------------------reconstruct events based on coincidence------------
		myEvent.push_back(dataSet[0]);
		for(UShort_t i = 1;i < dataSet.size();i++){
			//  cout<<"i: "<<i<<" strip "<<dataSet[i].strip<<"  time "<<dataSet[i].time<<"  energy "<<dataSet[i].energy<<endl;
			start_new_event = false;
			//---------------------delta T--------
			if(dataSet[i].is_frontStrip()){
				if(dataSet[i-1].is_frontStrip()){
					td_ff =static_cast<Long64_t>( dataSet[i].time - dataSet[i-1].time);
					h_delT_ff->Fill (td_ff);
					// cout<<"ff "<<td_ff<<endl;
					if( abs(td_ff) <= s1->ff_window){
						myEvent.push_back(dataSet[i]);// cout<<"ff coincidence"<<endl;
					}else start_new_event = true;

				}else{
					td_fb =  static_cast<Long64_t>(dataSet[i].time - dataSet[i-1].time);
					h_delT_fb->Fill(td_fb);
					//cout<<"fb "<<td_fb<<endl;
					if( abs(td_fb) <= s1->fb_window){
						myEvent.push_back(dataSet[i]);// cout<<"fb coincidence"<<endl;
					}else start_new_event = true;    
				}

			}
			else{
				if(dataSet[i-1].is_frontStrip()){
					td_bf =  static_cast<Long64_t>(dataSet[i].time -  dataSet[i-1].time);
					h_delT_bf->Fill(td_bf);
					// cout<<"bf "<<td_bf<<endl;
					if( abs(td_bf) <= s1->bf_window){
						myEvent.push_back(dataSet[i]); //cout<<"bf coincidence"<<endl;
					}else start_new_event = true;
				}
				else{
					td_bb =  static_cast<Long64_t>(dataSet[i].time - dataSet[i-1].time);
					h_delT_bb->Fill (td_bb);
					//cout<<"bb "<<td_bb<<endl;
					if( abs(td_bb) <=s1->bb_window){
						myEvent.push_back(dataSet[i]);// cout<<"bb coincidence"<<endl;
					}else start_new_event = true;
				}
			}

			//---------------make pixel-----------
			if(start_new_event == true){
				// cout<<"here "<<"size "<<myEvent.size()<<endl;
				ePoint = determine_dssd_pixel(myEvent);
				//--------
				if(ePoint.energyX > 0. && ePoint.energyY > 0.){
					h_E_frontBack->Fill(ePoint.energyX, ePoint.energyY);
					h_DSSD_XY_hit->Fill(ePoint.X, ePoint.Y);
					// cout<<"pixel x: "<< ePoint.X<<"  y: "<< ePoint.Y<<"  e1 "<< ePoint.energyX<<"  e2 "<<ePoint.energyY<<endl;
				}
				//---------------
				myEvent.push_back(dataSet[i]);
			}
		}

		//------------clear the data container--
		dataSet.clear();

	}

}

EventPoint GUser::determine_dssd_pixel(std::vector<DataPoint> &myEvent){
	EventPoint event;
	event.clear();

	if(!myEvent.empty()){
		Double_t emax_front = 0., emax_back =0.;
		int emax_front_strip = 0, emax_back_strip = 0;
		ULong64_t emax_front_time =0;
		if(s1->sum_neighboring_strips){
			//-----------sum-energy of neighboring strips
			for(UShort_t n =0; n < myEvent.size();n++){
				if(myEvent[n].is_frontStrip())myEvent_front.push_back(myEvent[n]);
				else myEvent_back.push_back(myEvent[n]);
			}
			if(myEvent_front.size() >1){
				//sort in energy in descending order
				sort(myEvent_front.begin(), myEvent_front.end(),compareEnergy);
				//assumptions:
				//1. maxE strip (i) defines the postion
				//2. the other strips must be the neighboring one, i+1 or i-1

				UShort_t element =1, i =1;
				UShort_t old_size = myEvent_front.size();
				while(element != old_size){
					element++;
					if(!myEvent_front[i].is_neighboringStripOf(myEvent_front[0])){
						//ignore induced charge events == non neighboring strip
						myEvent_front.erase ( myEvent_front.begin()+i);
					}
					else{i++;}
				}
			}
			//back
			if(myEvent_back.size() >1){
				//sort in energy in descending order
				sort(myEvent_back.begin(), myEvent_back.end(),compareEnergy);

				UShort_t element =1, i =1;
				UShort_t old_size = myEvent_back.size();
				while(element != old_size){
					element++;
					if(!myEvent_back[i].is_neighboringStripOf(myEvent_back[0])){
						//ignore induced charge events == non neighboring strip
						myEvent_back.erase ( myEvent_back.begin()+i);
					}
					else{i++;}
				}
			}
			//---------------
			//------sum the energies
			//--front
			for(UShort_t n =0; n < myEvent_front.size();n++){
				emax_front += myEvent_front[n].energy;
			}
			//--back
			for(UShort_t n =0; n < myEvent_back.size();n++){
				emax_back += myEvent_back[n].energy;
			}
			//position maxE strips
			if(!myEvent_front.empty()){
				emax_front_strip =  myEvent_front[0].strip;
				//time = time of maxE front 
				emax_front_time = myEvent_front[0].time;
			}
			if(!myEvent_back.empty()){
				emax_back_strip =  myEvent_back[0].strip;
			}

		}else{
			//---------get max energy strip
			for(UShort_t n =0; n < myEvent.size();n++){
				//cout<<"strip "<<myEvent[n].strip<< "  energy "<<myEvent[n].energy<<"  time "<<myEvent[n].time<<endl;
				if(myEvent[n].is_frontStrip()){
					//get max energy strip
					if(myEvent[n].energy > emax_front){
						emax_front = myEvent[n].energy;
						emax_front_strip =myEvent[n].strip;
						emax_front_time = myEvent[n].time;
					}
				}else{
					if(myEvent[n].energy > emax_back){
						emax_back = myEvent[n].energy;
						emax_back_strip =myEvent[n].strip;
					}
				}
			}
		}

		if(emax_front > 0. && emax_back > 0.){
			event.X = emax_front_strip;
			event.Y = emax_back_strip -128;
			event.time = emax_front_time;
			event.energyX = emax_front;
			event.energyY = emax_back;
		}
		myEvent.clear();
		if(s1->sum_neighboring_strips){
			myEvent_back.clear();
			myEvent_front.clear();
		}
	}
	return event;
}


