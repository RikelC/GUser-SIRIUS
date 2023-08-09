#include "TimeAlignment.h"

TimeAlignment::TimeAlignment(){
	s1 = MyGlobal::GetInstance();

	fTimeStampShift = new int *[s1->NBOARDS_DSSD];
	fCFDTimeStampShift = new double *[s1->NBOARDS_DSSD];
	for(int i =0; i< s1->NBOARDS_DSSD; i++){
		fTimeStampShift[i] = new int[NCHANNELS];
		fCFDTimeStampShift[i] = new double[NCHANNELS];
	}
	//----------------
	//first initialize
	//-----------------

	for(int i =0; i< s1->NBOARDS_DSSD; i++){
		for(int j =0; j< NCHANNELS; j++){
			fTimeStampShift[i][j]=0;
			fCFDTimeStampShift[i][j]=0.;
		}
	}
	std::string line; int boardID, bIdx, chID;
	int tShift; double tShift_cfd;

	std::ifstream myfile;
	myfile.open(s1->timestampAlignment_filename,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> tShift;

			bIdx = s1->boardIndex_DSSD[boardID];
			if(bIdx < s1->NBOARDS_DSSD){
				if(tShift != -100)fTimeStampShift[bIdx][chID] = tShift;
				//cout<< " board "<<boardID<<"  channel "<<chID<<"  bindex "<<bIdx<<"  timeshift "<<tShift<<endl;
			}
		}
		myfile.close();
	}
	else{
		std::cout <<RED<<s1->timestampAlignment_filename<<" file could not be opened..."<<RESET<<std::endl;
		std::cout <<BLUE<<" No timestamp alignment will be performed"<<RESET<<std::endl;
		s1->PressEnterToContinue();
	}

	// CFD time alignment parameters

	myfile.open(s1->cfd_timeAlignment_filename,std::ifstream::in);
	if (myfile.is_open())
	{
		getline (myfile,line);
		while (!myfile.eof() )
		{
			myfile >> boardID >> chID >> tShift_cfd;

			bIdx = s1->boardIndex_DSSD[boardID];
			if(bIdx < s1->NBOARDS_DSSD){
				if(tShift_cfd != -100.)fCFDTimeStampShift[bIdx][chID] = tShift_cfd;
			}
		}
		myfile.close();
	}
	else{
		std::cout <<RED<<s1->cfd_timeAlignment_filename<<" file could not be opened..."<<RESET<<std::endl;
		std::cout <<BLUE<<" No cfd time alignment will be performed"<<RESET<<std::endl;
		s1->PressEnterToContinue();
	}

}

TimeAlignment::~TimeAlignment(){

	for(int i =0; i< s1->NBOARDS_DSSD; i++){
		delete [] fTimeStampShift[i];
		delete [] fCFDTimeStampShift[i];
	}
	delete [] fTimeStampShift;
	delete [] fCFDTimeStampShift;
}

void TimeAlignment::AlignTime(double &time, int &board, int &channel){
	int iboard =  (int)s1->boardIndex_DSSD[board];
	time = time - fCFDTimeStampShift[iboard][channel];
}

void TimeAlignment::AlignTrigger(ushort &trigger, int &board, int &channel){
	int iboard =  (int)s1->boardIndex_DSSD[board];
	trigger = trigger - fTimeStampShift[iboard][channel];
}

void TimeAlignment::AlignTimeStamp(ullint &timestamp, int &board, int &channel){
	int iboard =  (int)s1->boardIndex_DSSD[board];
	timestamp = timestamp - fTimeStampShift[iboard][channel];
}

int TimeAlignment::GetTimeStampShift(DssdData * const data){
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	return fTimeStampShift[b][ch];
}

double TimeAlignment::GetCFDTimeShift(DssdData * const data){
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	return fCFDTimeStampShift[b][ch];
}

ullint TimeAlignment::AlignTimeStamp(DssdData * const data){
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	ullint newTimeStamp = data->GetTimeStamp() - fTimeStampShift[b][ch];

	return newTimeStamp;
}

double TimeAlignment::AlignTime(DssdData * const data){
	int b = data->GetBoardIndex();
	int ch = data->GetChannel();
	double newTime = data->GetCFDTime() - fCFDTimeStampShift[b][ch];

	return newTime;
}


ullint TimeAlignment::AlignTimeStampWithTrigger(DssdData * const data){
	int diff = s1->pre_trig_buffer - (int)(data->GetTrigger() + s1->nStart_trace);
	ullint newTimeStamp = data->GetTimeStamp() - diff/2;
	//ushort newTrigger = (ushort)(data->GetTrigger() + diff);
	//ushort newMaxPos = (ushort)(data->GetMaximumPosition() + diff);
	data->SetTimeStamp(newTimeStamp);
	//data->set_Trigger(newTrigger );
	//data->set_MaxPos(newMaxPos );
	return newTimeStamp;
}

ullint TimeAlignment::AlignTimeStampWithMaximumPosition(DssdData * const data){
	int diff = s1->pre_trig_buffer - (int)(data->GetMaximumPosition() + s1->nStart_trace);
	ullint newTimeStamp = data->GetTimeStamp() - diff/2;
	//ushort newTrigger = (ushort)(data->GetTrigger() + diff);
	//ushort newMaxPos = (ushort)(data->GetMaximumPosition() + diff);
	data->SetTimeStamp(newTimeStamp);
	//data->set_Trigger(newTrigger );
	//data->set_MaxPos(newMaxPos );
	return newTimeStamp;
}
