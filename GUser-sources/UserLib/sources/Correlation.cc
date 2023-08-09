#include "Correlation.h"
bool CompareTimeDssdEvent(DssdEvent const & i, DssdEvent const & j)
{	
	return std::tie(i.fTimeStamp, i.fEnergyX, i.fEnergyY) < std::tie(j.fTimeStamp, j.fEnergyX, j.fEnergyY);
}

bool CompareTimeTrackerEvent(TrackerEvent const & i, TrackerEvent const & j)
{	
	return std::tie(i.fTimeStamp, i.fSumX, i.fSumY) < std::tie(j.fTimeStamp, j.fSumX, j.fSumY);
}



Correlation::Correlation(){
	s1 = s1->GetInstance();
}

Correlation::~Correlation(){
	RecoilEventVec.clear();
	DecayEventVec.clear();
}

void Correlation::Find(std::vector<DssdEvent> &dssd, std::vector<TrackerEvent> &tracker){
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
		for(unsigned int j = start; j < tracker.size(); j++){
			tof = static_cast<llint>(dssd[i].GetTimeStamp() - tracker[j].GetTimeStamp());
			if(tof < s1->ToF_lower ) continue;
			else if(tof >= s1->ToF_lower && tof <= s1->ToF_upper){
				tof_found = true;
				found = j;
				start = j+1;
				break;
			}
		}
		//-------------------
		if(tof_found){
			recoil.SetDssdEvent(dssd[i]);
			recoil.SetTrackerEvent(tracker[found]);
			recoil.SetToF(tof);
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
void Correlation::SortInTime(std::vector<DssdEvent> &dataSet){
	std::sort(dataSet.begin(), dataSet.end(),CompareTimeDssdEvent);
}
void Correlation::SortInTime(std::vector<TrackerEvent> &dataSet){
	std::sort(dataSet.begin(), dataSet.end(),CompareTimeTrackerEvent);
}
