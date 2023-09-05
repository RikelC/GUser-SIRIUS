#include "Correlation.h"
bool CompareTimeDssdEvent(DssdEvent const & i, DssdEvent const & j)
{	
	return std::tie(i.fTimeStampX, i.fEnergyX, i.fEnergyY) < std::tie(j.fTimeStampX, j.fEnergyX, j.fEnergyY);
}

bool CompareTimeTrackerCoBoEvent(TrackerCoBoEvent const & i, TrackerCoBoEvent const & j)
{	
	return std::tie(i.fTimeStamp, i.fSumX, i.fSumY) < std::tie(j.fTimeStamp, j.fSumX, j.fSumY);
}

bool CompareTimeTrackerNumexo2Event(TrackerNumexo2Event const & i, TrackerNumexo2Event const & j)
{	
	return std::tie(i.fTimeStamp, i.fEnergy) < std::tie(j.fTimeStamp, j.fEnergy);
}



Correlation::Correlation(){
	s1 = s1->GetInstance();
}

Correlation::~Correlation(){
	RecoilEventVec.clear();
	DecayEventVec.clear();
	uDssd.clear();
	uTrackerNumexo2.clear();
	uTrackerCobo.clear();
	unCorrelatedDssd.clear();
	unCorrelatedTrackerNumexo2.clear();
	unCorrelatedTrackerCobo.clear();
}
void Correlation::Clear(){
	RecoilEventVec.clear();
	DecayEventVec.clear();
	uDssd.clear();
	uTrackerNumexo2.clear();
	uTrackerCobo.clear();
	unCorrelatedDssd.clear();
	unCorrelatedTrackerNumexo2.clear();
	unCorrelatedTrackerCobo.clear();

}
void Correlation::Find(std::vector<DssdEvent> &dssd, std::vector<TrackerNumexo2Event> &tracker){
	SortInTime(dssd);
	SortInTime(tracker);
	RecoilEventVec.clear();
	DecayEventVec.clear();
	bool tof_found = false;
	llint tofX = 0, tofY =0;

	RecoilEvent recoil;
	DecayEvent decay;
	unsigned int start =0, found = 0;
	for(unsigned int i = 0; i < dssd.size(); i++){
		tof_found = false;
		recoil.Clear();
		decay.Clear();
		for(unsigned int j = start; j < tracker.size(); j++){
			tofX = static_cast<llint>(dssd[i].GetTimeStampX() - tracker[j].GetTimeStamp());
			if(tofX < s1->ToF_lower_numexo2 ) continue;
			else if(tofX >= s1->ToF_lower_numexo2 && tofX <= s1->ToF_upper_numexo2){
				tof_found = true;
				tofY = static_cast<llint>(dssd[i].GetTimeStampY() - tracker[j].GetTimeStamp());
				found = j;
				start = j+1;
				break;
			}
		}
		//-------------------
		if(tof_found){
			recoil.SetDssdEvent(dssd[i]);
			recoil.SetTrackerNumexo2Event(tracker[found]);
			recoil.SetToFNumexo2X(tofX);
			recoil.SetToFNumexo2Y(tofY);
			recoil.ReconstructTrajectory();
			RecoilEventVec.push_back(recoil);
		}
		else{
			decay.SetDssdEvent(dssd[i]);
			DecayEventVec.push_back(decay);

		}
	}
	dssd.clear();
	tracker.clear();
}

void Correlation::Find(std::vector<DssdEvent> &dssd, std::vector<TrackerCoBoEvent> &tracker){
	SortInTime(dssd);
	SortInTime(tracker);
	RecoilEventVec.clear();
	DecayEventVec.clear();
	bool tof_found = false;
	llint tof = 0;

	RecoilEvent recoil;
	DecayEvent decay;
	unsigned int start =0, found = 0;
	for(unsigned int i = 0; i < dssd.size(); i++){
		tof_found = false;
		recoil.Clear();
		decay.Clear();
		for(unsigned int j = start; j < tracker.size(); j++){
			tof = static_cast<llint>(dssd[i].GetTimeStampX() - tracker[j].GetTimeStamp());
			if(tof < s1->ToF_lower_cobo ) continue;
			else if(tof >= s1->ToF_lower_cobo && tof <= s1->ToF_upper_cobo){
				tof_found = true;
				found = j;
				start = j+1;
				break;
			}
		}
		//-------------------
		if(tof_found){
			recoil.SetDssdEvent(dssd[i]);
			recoil.SetTrackerCoBoEvent(tracker[found]);
			recoil.SetToFCoBo(tof);
			recoil.ReconstructTrajectory();
			RecoilEventVec.push_back(recoil);
		}
		else{
			decay.SetDssdEvent(dssd[i]);
			DecayEventVec.push_back(decay);

		}
	}
	dssd.clear();
	tracker.clear();
}
std::vector<DssdEvent> Correlation::GetUnCorrelatedDssdEvents(){
	return unCorrelatedDssd;
}
std::vector<TrackerNumexo2Event> Correlation::GetUnCorrelatedTackerNumexo2Events(){
	return unCorrelatedTrackerNumexo2;
}
std::vector<TrackerCoBoEvent> Correlation::GetUnCorrelatedTackerCoBoEvents(){
	return unCorrelatedTrackerCobo;
}
void Correlation::Find(std::vector<DssdEvent> &dssd, std::vector<TrackerNumexo2Event> &trackerNumexo2, std::vector<TrackerCoBoEvent> &trackerCobo, bool reached_eof ){

	RecoilEventVec.clear();
	DecayEventVec.clear();

	if(dssd.size() > 1)SortInTime(dssd);
	if(trackerNumexo2.size() > 1)SortInTime(trackerNumexo2);
	if(trackerCobo.size() > 1)SortInTime(trackerCobo);
	//cout<<"--------------------------xxxxxxxxxxx------------"<<endl;
	//cout<<"Correlation dssd size: "<<dssd.size()<<" tracker numexo2: "<<trackerNumexo2.size()<<"  cobo: "<<trackerCobo.size()<<endl;
	//Add the .previous uncorrelated events

	/*	if(uDssd.size() > 0){
		dssd.insert(dssd.end(), uDssd.begin(), uDssd.end());
		uDssd.clear();
		SortInTime(dssd);
		}
		if(uTrackerNumexo2.size() >0){
		trackerNumexo2.insert(trackerNumexo2.begin(), uTrackerNumexo2.begin(), uTrackerNumexo2.end());
		uTrackerNumexo2.clear();
		SortInTime(trackerNumexo2);
		}
		if(uTrackerCobo.size() >0){
		trackerCobo.insert(trackerCobo.begin(), uTrackerCobo.begin(), uTrackerCobo.end());
		uTrackerCobo.clear();
		SortInTime(trackerCobo);
		}
		*/
	llint dt1 =0, dt2 =0, dt3 =0;
	if(uDssd.size() > 0){
		unCorrelatedDssd.clear();
		for(std::vector<DssdEvent>::iterator it = uDssd.begin(); it != uDssd.end(); ++it){
			if(!dssd.empty())dt1 = static_cast<llint>(dssd[0].GetTimeStamp() - (*it).GetTimeStamp());
			if(!trackerNumexo2.empty())dt2 = static_cast<llint>(trackerNumexo2[0].GetTimeStamp() - (*it).GetTimeStamp());
			if(!trackerCobo.empty())dt3 = static_cast<llint>(trackerCobo[0].GetTimeStamp() - (*it).GetTimeStamp());
			if((dt1 < MAX_TIME_WINDOW) || (dt2 < MAX_TIME_WINDOW) || (dt3 < MAX_TIME_WINDOW)) 
				dssd.push_back((*it));
			else unCorrelatedDssd.push_back((*it));
		}
		uDssd.clear();
	}
	//
	if(uTrackerNumexo2.size() > 0){
		unCorrelatedTrackerNumexo2.clear();
		dt1 =0, dt2 =0, dt3 =0;
		for(std::vector<TrackerNumexo2Event>::iterator it = uTrackerNumexo2.begin(); it != uTrackerNumexo2.end(); ++it){
			if(!dssd.empty())dt1 = static_cast<llint>(dssd[0].GetTimeStamp() - (*it).GetTimeStamp());
			if(!trackerNumexo2.empty())dt2 = static_cast<llint>(trackerNumexo2[0].GetTimeStamp() - (*it).GetTimeStamp());
			if(!trackerCobo.empty())dt3 = static_cast<llint>(trackerCobo[0].GetTimeStamp() - (*it).GetTimeStamp());
			if((dt1 < MAX_TIME_WINDOW) || (dt2 < MAX_TIME_WINDOW) || (dt3 < MAX_TIME_WINDOW)) 
				trackerNumexo2.push_back((*it));
			else unCorrelatedTrackerNumexo2.push_back((*it));
		}
		uTrackerNumexo2.clear();
	}	
	//
	if(uTrackerCobo.size()){
		unCorrelatedTrackerCobo.clear();
		for(std::vector<TrackerCoBoEvent>::iterator it = uTrackerCobo.begin(); it != uTrackerCobo.end(); ++it){
			if(!dssd.empty())dt1 = static_cast<llint>(dssd[0].GetTimeStamp() - (*it).GetTimeStamp());
			if(!trackerNumexo2.empty())dt2 = static_cast<llint>(trackerNumexo2[0].GetTimeStamp() - (*it).GetTimeStamp());
			if(!trackerCobo.empty())dt3 = static_cast<llint>(trackerCobo[0].GetTimeStamp() - (*it).GetTimeStamp());
			if((dt1 < MAX_TIME_WINDOW) || (dt2 < MAX_TIME_WINDOW) || (dt3 < MAX_TIME_WINDOW)) 
				trackerCobo.push_back((*it));
			else unCorrelatedTrackerCobo.push_back((*it));
		}
		uTrackerCobo.clear();
	}

	//cout<<"After inserting  dssd "<<dssd.size()<<"  trackerNumexo2: "<<trackerNumexo2.size()<<"  trackerCobo: "<<trackerCobo.size()<<endl;
	//cout<<"After inserting: uncorrelated dssd "<<unCorrelatedDssd.size()<<"  trackerNumexo2: "<<unCorrelatedTrackerNumexo2.size()<<"  trackerCobo: "<<unCorrelatedTrackerCobo.size()<<endl;

	bool tof1_found = false;
	bool tof2_found = false;
	llint tof1X = 0, tof1Y =0;
	llint tof2 = 0;
	bool *flagTrackerNumexo2;
	bool *flagTrackerCobo;
	if(!trackerNumexo2.empty()){
		flagTrackerNumexo2 = new bool[trackerNumexo2.size()];
		for(unsigned int i =0; i< trackerNumexo2.size();i++)flagTrackerNumexo2[i]=false;
	}
	if(!trackerCobo.empty()){
		flagTrackerCobo = new bool[trackerCobo.size()];
		for(unsigned int i =0; i< trackerCobo.size();i++) flagTrackerCobo[i]=false;
	}
	//cout<<"toto 1"<<endl;
	RecoilEvent recoil;
	DecayEvent decay;
	unsigned int start1 =0, found1 = 0;
	unsigned int start2 =0, found2 = 0;
	double cfdTofFront =0., cfdTofBack =0.;
	for(unsigned int i = 0; i < dssd.size(); i++){
		tof1_found = false;
		tof2_found = false;
		recoil.Clear();
		decay.Clear();
		//cout<<"i: "<<i<<endl;
		//---------------------xx-------------------
		for(unsigned int j = start1; j < trackerNumexo2.size(); j++){
			tof1X = static_cast<llint>(dssd[i].GetTimeStampX() - trackerNumexo2[j].GetTimeStamp());
			if(tof1X >= s1->ToF_lower_numexo2 && tof1X <= s1->ToF_upper_numexo2){
				tof1_found = true;
				found1 = j;
				flagTrackerNumexo2[j]=true;
				start1 = j+1;
				tof1Y = static_cast<llint>(dssd[i].GetTimeStampY() - trackerNumexo2[j].GetTimeStamp());
				cfdTofFront =  static_cast<double>(dssd[i].GetCFDTimeX() - trackerNumexo2[j].GetCFDTime());
				cfdTofBack =  static_cast<double>(dssd[i].GetCFDTimeY() - trackerNumexo2[j].GetCFDTime());
				break;
			}
		}
		//---------------------xx-------------------
		for(unsigned int j = start2; j < trackerCobo.size(); j++){
			tof2 = static_cast<llint>(dssd[i].GetTimeStamp() - trackerCobo[j].GetTimeStamp());
			if(tof2 >= s1->ToF_lower_cobo && tof2 <= s1->ToF_upper_cobo){
				tof2_found = true;
				found2 = j;
				flagTrackerCobo[j]=true;
				start2 = j+1;
				break;
			}
		}
		//---------------------xx-------------------

		//cout<<"i--: "<<i<<endl;
		if(!tof1_found && !tof2_found){
			uDssd.push_back(dssd[i]);
			//			decay.SetDssdEvent(dssd[i]);
			//			DecayEventVec.push_back(decay);
		}else{
			recoil.SetDssdEvent(dssd[i]);
			if(tof1_found){
				recoil.SetTrackerNumexo2Event(trackerNumexo2[found1]);
				recoil.SetToFNumexo2X(tof1X);
				recoil.SetToFNumexo2Y(tof1Y);
				recoil.SetCFDToFNumexo2X(cfdTofFront);
				recoil.SetCFDToFNumexo2Y(cfdTofBack);
				recoil.SetTrackerNumexo2Flag(true);
			}
			if(tof2_found){
				recoil.SetTrackerCoBoEvent(trackerCobo[found2]);
				recoil.SetToFCoBo(tof2);
				recoil.SetCoBoFlag(true);
				recoil.ReconstructTrajectory();
			}
			RecoilEventVec.push_back(recoil);
		}
	}

	//cout<<"toto 2"<<endl;
	dssd.clear();
	if(!trackerNumexo2.empty()){
		for(unsigned int i =0; i < trackerNumexo2.size();i++){
			if(!flagTrackerNumexo2[i])uTrackerNumexo2.push_back(trackerNumexo2[i]);
		}
		trackerNumexo2.clear();
		delete [] flagTrackerNumexo2;
	}
	if(!trackerCobo.empty()){
		for(unsigned int i=0; i< trackerCobo.size();i++){
			if(!flagTrackerCobo[i])uTrackerCobo.push_back(trackerCobo[i]);
		}
		trackerCobo.clear();
		delete [] flagTrackerCobo;
	}



	//cout<<"recoil + decay size: "<<(RecoilEventVec.size() + DecayEventVec.size())<<"  recoils: "<<RecoilEventVec.size()<<"  decays: "<<DecayEventVec.size()<<endl;
	//get the uncorrelated events
	//cout<<"uncorrel in this data sets dssd: "<<uDssd.size()<<"  tackerN: "<<uTrackerNumexo2.size()<<"  trackerCob: "<<uTrackerCobo.size()<<endl;
	if(!unCorrelatedDssd.empty()){
		for(std::vector<DssdEvent>::iterator it = unCorrelatedDssd.begin(); it != unCorrelatedDssd.end(); ++it){
			decay.SetDssdEvent((*it));
			DecayEventVec.push_back(decay);
		}
		unCorrelatedDssd.clear();
	}
	if(!unCorrelatedTrackerNumexo2.empty()){

		unCorrelatedTrackerNumexo2.clear();
	}

	if(!unCorrelatedTrackerCobo.empty()){

		unCorrelatedTrackerCobo.clear();
	}
	//---------------At the end of a run--------------------
	if(reached_eof){
		if( !uDssd.empty() >0){
			////cout<<"uDssd: "<<uDssd.size()<<endl;
			for(std::vector<DssdEvent>::iterator it = uDssd.begin(); it != uDssd.end(); ++it){
				decay.SetDssdEvent((*it));
				DecayEventVec.push_back(decay);
			}
			uDssd.clear();
		}

		if(!uTrackerNumexo2.empty()){

			uTrackerNumexo2.clear();
		}

		if(!uTrackerCobo.empty()){

			uTrackerCobo.clear();
		}
		//---------
		//
	}
	//cout<<" uncorrelated dssd: "<<uDssd.size()<<"  trackerNumexo: "<<uTrackerNumexo2.size()<<"  trackerCobo: "<<uTrackerCobo.size()<<endl;
	//cout<<"After the uncorrelated events: recoil + decay size: "<<(RecoilEventVec.size() + DecayEventVec.size())<<"  recoils: "<<RecoilEventVec.size()<<"  decays: "<<DecayEventVec.size()<<endl;


}


void Correlation::SortInTime(std::vector<DssdEvent> &dataSet){
	std::sort(dataSet.begin(), dataSet.end(),CompareTimeDssdEvent);
}
void Correlation::SortInTime(std::vector<TrackerCoBoEvent> &dataSet){
	std::sort(dataSet.begin(), dataSet.end(),CompareTimeTrackerCoBoEvent);
}
void Correlation::SortInTime(std::vector<TrackerNumexo2Event> &dataSet){
	std::sort(dataSet.begin(), dataSet.end(),CompareTimeTrackerNumexo2Event);
}

