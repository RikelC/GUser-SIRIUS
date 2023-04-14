
void GruScript_offline(){  

	// specify good include dir!
	gROOT->Reset();
	char command[100];
	if (strncmp(gROOT->GetVersion(), "5.3",3)==0){
		printf ("version de root 5.3XXXX\n");
		TString test = gSystem->Getenv("GRUDIR");
		if (test.CompareTo("")==0) sprintf(command ,".include /home/acqexp/GRU/GRUcurrent/include");
		else sprintf(command ,".include %s/include",test.Data());

		gROOT->ProcessLine(".L ./GUser_C.so");//load and compile TUiser class
	}else{
		R__LOAD_LIBRARY(libHist)
			R__LOAD_LIBRARY(GUser_C)
	}

	std::ifstream rList; 
	std::string rName; 
	char treeFileName[100];
	char histFileName[100];

	ofstream gru_pid;
	gru_pid.open("gruScript_offline_pid.log");
	gru_pid << gSystem->GetPid()<<endl;
	gru_pid.close();

	rList.open("ListRun.txt", std::ifstream::in);
	if(rList.is_open()){
		while(!rList.eof()){
			getline(rList, rName);
			if(rName.empty()) continue;
			std::size_t pos = rName.find_first_of("_");
			cout<<rName<<endl;
			std::string i =  rName;
			i.erase(0, pos+1);

			pos = i.length();
			i.erase(4, pos);

			//cout<<i<<endl;
			std::string j = rName;

			pos = rName.find_last_of("s");

			j.erase(0, pos+2);
			if(j.empty()) j = "0";
			//cout<<j<<endl;

			sprintf(treeFileName,"/data/siriusX/test/acquisition/RootFiles/Tree_%s_r%s-Numexo2.root", i.data(), j.data());
			//puts(treeFileName);

			GMFMFile *file = new GMFMFile(rName.data());   // Get first Device ( in this case  run  file)
			file->Open();                          // Open Device
			GUser *a= new GUser(file);            // creat user treatement environement
			a->g_instance = a;
			a->EventInit("dssd_acquisition","mfm",false);                        // event initialisation
			a->SetSpectraMode(1);                  // Declare all raw parameters as histograms
			//a->SetScalerMode(1,"./scale.root");    // Do not make a run with scaler  events
			//a->SetTTreeMode (1,"./runstd.root");   // Do a standart TTree ( one leaf  for each parameter) of events

			//sprintf(treeFileName,"/home/sirius/Chakma/GRUtestsirius_current/data/Tree_%s_r%s-Numexo2.root", i.data(), j.data());
			//puts(treeFileName);
			a->SetTTreeMode(3,treeFileName);
			a->InitUser();     // Do Init User()
			//file->Rewind();        // rewind run
			a->DoRun();        // do treaments of all events
			//a->DoRun(2000);  // do treaments on first 2000 events
			file->Close();
			//..if you want to add a second run in same analyse uncomment next line.........................
			//file->SetDevice("run2.dat"); // change of run
			//file->Open();
			//a->DoRun();
			//file->Close();
			//...........

			a->EndUser();              // must be explicitly called , if it needs
			//sprintf(histFileName,"/home/sirius/Chakma/GRUtestsirius_current/results/histo_%s_r%s-Numexo2_sumEtest2.root",i.data(), j.data());
			sprintf(histFileName,"/data/siriusX/test/acquisition/RootFiles/histo_%s_r%s-Numexo2.root",i.data(), j.data());
			//a->histFileName = histFileName;
			a->SpeSave(histFileName);
			delete (a);   // finish
			gROOT->ProcessLine(".q");
		}

		rList.close();
	}

	gROOT->ProcessLine(".q");
}

