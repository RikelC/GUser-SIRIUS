
#include "UTTree.h"
UTTree::UTTree(){
	s1 = MyGlobal::GetInstance();
}
UTTree::~UTTree(){

}
void UTTree::Initialize(char *filename, DssdData *dssd, TunnelData *tunnel, TrackerData *tracker)
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
void UTTree::FillSiriusFrames(){

	if(s1->fsaveTree && fSiriusTTree)fSiriusTTree->Fill();

}

void UTTree::FillReaGenericFrames(){

	if(s1->fsaveTree && fReaGenericTTree)fReaGenericTTree->Fill();

}

void UTTree::FillCoBoFrames(){

	if(s1->fsaveTree && fCoBoTTree)fCoBoTTree->Fill();

}
void UTTree::Save(){
	if(s1->fsaveTree && fTreeFile){
		fTreeFile->cd();
		fSiriusTTree->Write();
		fReaGenericTTree->Write();
		fCoBoTTree->Write();
		fTreeFile->Close();
		cout<<BLUE<<"TTree saved in file : "<<GREEN<<fTreeFileName<<RESET <<endl;
	}
}


