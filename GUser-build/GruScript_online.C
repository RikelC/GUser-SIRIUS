// GRU 'GruScript.C(parameter)'
// or
// GRU, .x GruScript.C(parameters)
// or
// GRU, .L GruScript.C, GruScript(parameters)
void GruScript_online(){
	gROOT->Reset();

	char command[100];
	if (strncmp(gROOT->GetVersion(), "5.3",3)==0){
		printf ("version de root 5.3XXXX\n");

		TString test = gSystem->Getenv("GRUDIR");
		if (test.CompareTo("")==0) sprintf(command ,".include /home/acqexp/GRU/GRUcurrent/include");
		else sprintf(command ,".include %s/include",test.Data());
		gROOT->ProcessLine(command);
		gROOT->ProcessLine(".L ./GUser_C.so");//load and compile TUiser class
	}else{
		R__LOAD_LIBRARY(libHist);
		R__LOAD_LIBRARY(GUser_C);
	}
	ofstream gru_pid;
	gru_pid.open("gruScript_pid.log");
	gru_pid << gSystem->GetPid()<<endl;
	gru_pid.close();


	GNetClientNarval  *net = new GNetClientNarval("localhost"); //
	net->SetPort (10216);
	net->SetBufferSize(16384);

	GUser * a= new GUser(net);          // creat user treatement environement
	GNetServerRoot * serv = new GNetServerRoot(9090,a);
	a->EventInit("dssd_acquisition","mfm");                      // event initialisation
	a->SetSpectraMode(1);                // Declare all raw parameters as histograms
	a->InitUser();
	serv->StartServer();
	a->DoRun();                          // a->DoRun(2000);do treaments on 2000 first events ( 0 = all);

	net->Close();
	a->EndUser();

	// must be explicitly called , if it needs
	a->histFileName("histo.root");
	a->SpeSave("histo.root"); // save all declared histogram
	delete (a);   // finish
	gROOT->ProcessLine(".q");
}


