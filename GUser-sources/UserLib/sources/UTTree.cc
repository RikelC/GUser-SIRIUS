
#include "UTTree.h"
UTTree::UTTree(){
	s1 = MyGlobal::GetInstance();
}
UTTree::~UTTree(){

}
void UTTree::Initialize(char *filename, DssdData *dssd, TunnelData *tunnel, TrackerCoBoData *tracker)
{
	fTreeFileName = filename;
	fTreeFile = new TFile(filename, "RECREATE");
	fSiriusTTree = new TTree("fSiriusTTree", "fSiriusTTree");
	fSiriusTTree->Branch("Time", dssd->GetCFDTimeAddress(), "Time/D");
	fSiriusTTree->Branch("Timestamp", dssd->GetTimeStampAddress(), "Timestamp/l");
	fSiriusTTree->Branch("TraceSize",  &s1->TRACE_SIZE, "TraceSize/s");
	fSiriusTTree->Branch("Trace",  dssd->GetTrace(), "Trace[TraceSize]/s");
	fSiriusTTree->Branch("Board",  dssd->GetBoardAddress(), "Board/s");
	fSiriusTTree->Branch("Channel",  dssd->GetChannelAddress(), "Channel/s");
	fSiriusTTree->Branch("StripNumber",  dssd->GetStripNumberAddress(), "StripNumber/s");
	fSiriusTTree->Branch("Energy",  dssd->GetRawEnergyAddress(), "Energy/D");
	fSiriusTTree->Branch("Baseline",  dssd->GetBaselineAddress(), "Baseline/d");
	fSiriusTTree->Branch("Trigger",  dssd->GetTriggerAddress(), "Trigger/s");
	fSiriusTTree->Branch("MaxPos",  dssd->GetMaximumPositionAddress(), "MaxPos/s");

	fReaGenericTTree = new TTree("fReaGenericTTree","fReaGenericTTree");	
	fReaGenericTTree->Branch("EventNo",  tunnel->GetEventNumberAddress(), "EventNo/i");
	fReaGenericTTree->Branch("Board",  tunnel->GetBoardAddress(), "Board/s");
	fReaGenericTTree->Branch("Channel",  tunnel->GetChannelAddress(), "Channel/s");
	fReaGenericTTree->Branch("Energy",  tunnel->GetRawEnergyAddress(), "Energy/s");
	fReaGenericTTree->Branch("Time",  tunnel->GetTimeStampAddress(), "Timestamp/s");

	fCoBoTTree = new TTree("fCoBoTTree", "fCoBoTTree");
	fCoBoTTree->Branch("Timestamp", tracker->GetTimeStampAddress(), "Timestamp/l");
	fCoBoTTree->Branch("EventNo",  tracker->GetEventNumberAddress(), "EventNo/i");
	fCoBoTTree->Branch("SumX",  tracker->GetSumXAddress(), "SumX/D");
	fCoBoTTree->Branch("SumY",  tracker->GetSumYAddress(), "SumY/D");
	fCoBoTTree->Branch("MaxX",  tracker->GetMaxXAddress(), "MaxX/D");
	fCoBoTTree->Branch("MaxY",  tracker->GetMaxYAddress(), "MaxY/D");
}


void UTTree::Initialize(char *filename, RecoilEvent & recoil, DecayEvent & decay){

	fTreeFileName = filename;
	fTreeFile = new TFile(filename, "RECREATE");
	fRecoilTTree = new TTree("fRecoilTTree","fRecoilTTree");
	fRecoilTTree->Branch("ToFCoBo",recoil.GetAddressOfToFCoBo(), "ToFCoBo/L" );
	fRecoilTTree->Branch("ToFNumexo2X",recoil.GetAddressOfToFNumexo2X(), "ToFNumexo2X/L" );
	fRecoilTTree->Branch("ToFNumexo2Y",recoil.GetAddressOfToFNumexo2Y(), "ToFNumexo2Y/L" );
	fRecoilTTree->Branch("CFDToFNumexo2X",recoil.GetAddressOfCFDToFNumexo2X(), "CFDToFNumexo2X/D" );
	fRecoilTTree->Branch("CFDToFNumexo2Y",recoil.GetAddressOfCFDToFNumexo2Y(), "CFDToFNumexo2Y/D" );
	fRecoilTTree->Branch("TrackLength",recoil.GetTrackLengthAddress(), "TrackLength/D" );
	fRecoilTTree->Branch("FlagCoBo",recoil.GetAddressOfFlagCoBo(), "FlagCoBo/O" );
	fRecoilTTree->Branch("FlagTrackerNumexo2",recoil.GetAddressOfFlagSedNumexo2(), "FlagTrackerNumexo2/O" );
	fRecoilTTree->Branch("CFDTime",recoil.GetAddressOfDssdEvent()->GetCFDTimeAddress(), "CFDTime/D" );
	fRecoilTTree->Branch("Timestamp",recoil.GetAddressOfDssdEvent()->GetTimeStampAddress(), "Timestamp/l" );
	fRecoilTTree->Branch("EX",recoil.GetAddressOfDssdEvent()->GetEnergyXAddress(), "EX/D" );
	fRecoilTTree->Branch("EY",recoil.GetAddressOfDssdEvent()->GetEnergyYAddress(), "EY/D" );
	fRecoilTTree->Branch("X",recoil.GetAddressOfDssdEvent()->GetPixelAddress()->GetXAddress(), "X/s" );
	fRecoilTTree->Branch("Y",recoil.GetAddressOfDssdEvent()->GetPixelAddress()->GetYAddress(), "Y/s" );
	fRecoilTTree->Branch("TraceX",recoil.GetAddressOfDssdEvent()->GetTraceX(), "TraceX[992]/s" );
	fRecoilTTree->Branch("TraceY",recoil.GetAddressOfDssdEvent()->GetTraceY(), "TraceY[992]/s" );
	fRecoilTTree->Branch("SedTrace",recoil.GetAddressOfTrackerNumexo2Event()->GetTrace(), "SedTrace[992]/s" );
	fRecoilTTree->Branch("SedE",recoil.GetAddressOfTrackerNumexo2Event()->GetAddressOfEnergy(), "SedE/D" );
	fRecoilTTree->Branch("SedCFDTime",recoil.GetAddressOfTrackerNumexo2Event()->GetAddressOfCFDTime(), "SedCFDTime/D" );
	fRecoilTTree->Branch("SedTimestamp",recoil.GetAddressOfTrackerNumexo2Event()->GetAddressOfTimeStamp(), "SedCFDTimestamp/l" );
	fRecoilTTree->Branch("Xm",recoil.GetAddressOfTrackerCoBoEvent()->GetAddressOfBaryCenterXm(), "Xm/D" );
	fRecoilTTree->Branch("Ym",recoil.GetAddressOfTrackerCoBoEvent()->GetAddressOfBaryCenterYm(), "Ym/D" );
	fRecoilTTree->Branch("Zm",recoil.GetAddressOfTrackerCoBoEvent()->GetAddressOfBaryCenterZm(), "Zm/D" );
	fRecoilTTree->Branch("CoBoTimestamp",recoil.GetAddressOfTrackerCoBoEvent()->GetAddressOfTimeStamp(), "CoBoTimestamp/l" );
	fRecoilTTree->Branch("TrackX",recoil.GetTrackXAddress(), "TrackX[101]/D" );
	fRecoilTTree->Branch("TrackY",recoil.GetTrackYAddress(), "TrackY[101]/D" );



	fDecayTTree = new TTree("fDecayTTree","fDecayTTree");
	fDecayTTree->Branch("CFDTime",recoil.GetAddressOfDssdEvent()->GetCFDTimeAddress(), "CFDTime/D" );
	fDecayTTree->Branch("Timestamp",recoil.GetAddressOfDssdEvent()->GetTimeStampAddress(), "Timestamp/l" );
	fDecayTTree->Branch("EX",decay.GetAddressOfDssdEvent()->GetEnergyXAddress(), "EX/D" );
	fDecayTTree->Branch("EY",decay.GetAddressOfDssdEvent()->GetEnergyYAddress(), "EY/D" );
	fDecayTTree->Branch("X",decay.GetAddressOfDssdEvent()->GetPixelAddress()->GetXAddress(), "X/s" );
	fDecayTTree->Branch("Y",decay.GetAddressOfDssdEvent()->GetPixelAddress()->GetYAddress(), "Y/s" );
	fDecayTTree->Branch("TraceX",decay.GetAddressOfDssdEvent()->GetTraceX(), "TraceX[992]/s" );
	fDecayTTree->Branch("TraceY",decay.GetAddressOfDssdEvent()->GetTraceY(), "TraceY[992]/s" );

}

void UTTree::FillRecoilTTree(){

	if(s1->fsaveTree && gROOT->FindObject("fRecoilTTree"))fRecoilTTree->Fill();
}
void UTTree::FillDecayTTree(){

	if(s1->fsaveTree && gROOT->FindObject("fDecayTTree"))fDecayTTree->Fill();
}


void UTTree::FillSiriusFrames(){

	if(s1->fsaveTree && gROOT->FindObject("fSiriusTTree"))fSiriusTTree->Fill();

}

void UTTree::FillReaGenericFrames(){

	if(s1->fsaveTree && gROOT->FindObject("fReaGenericTTree"))fReaGenericTTree->Fill();

}

void UTTree::FillCoBoFrames(){

	if(s1->fsaveTree && gROOT->FindObject("fCoBoTTree"))fCoBoTTree->Fill();

}
void UTTree::Save(){
	if(s1->fsaveTree && fTreeFile){
		fTreeFile->cd();
		if(gROOT->FindObject("fSiriusTTree"))fSiriusTTree->Write();
		if(gROOT->FindObject("fReaGenericTTree"))fReaGenericTTree->Write();
		if(gROOT->FindObject("fCoBoTTree"))fCoBoTTree->Write();
		if(gROOT->FindObject("fRecoilTTree"))fRecoilTTree->Write();
		fTreeFile->Close();
		cout<<BLUE<<"TTree saved in file : "<<GREEN<<fTreeFileName<<RESET <<endl;
	}
}


