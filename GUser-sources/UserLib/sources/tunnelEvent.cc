/*!
 * \file tunnelEvent.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */

#include "tunnelEvent.h" 
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in energy 
bool compareEnergy(tunnelDataPoint const & i, tunnelDataPoint const & j)
{
	return std::tie(i.energy, i.time) > std::tie(j.energy, j.time);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in time
bool compareTime(tunnelDataPoint const & i, tunnelDataPoint const & j)
{
	return std::tie(i.time, i.energy) < std::tie(j.time, j.energy);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor 
tunnelEvent::tunnelEvent(){
	s1 = myGlobal::getInstance();
	td =0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
tunnelEvent::~tunnelEvent(){
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Making of DSSD pixels
void tunnelEvent::construct(std::vector<tunnelDataPoint> &dataSet, TH2* h1, TH2* h2, TH2* h3, TH2* h4, TH1 *hdt1, TH1* hdt2, TH1* hdt3, TH1* hdt4){
	//------------------------
	if(!dataSet.empty()){
		std::sort(dataSet.begin(), dataSet.end(),compareTime);
		for(ushort i = 1;i < dataSet.size();i++){

			if(dataSet[i].get_macroPixel().get_detector() == dataSet[i-1].get_macroPixel().get_detector()){
				td =static_cast<llint>( dataSet[i].get_time() - dataSet[i-1].get_time());
				if(dataSet[i].get_macroPixel().get_detector() == 1){
					hdt1->Fill(td);
				}					if(dataSet[i].get_macroPixel().get_detector() == 2){
					hdt2->Fill(td);
				}					if(dataSet[i].get_macroPixel().get_detector() == 3){
					hdt3->Fill(td);
				}					if(dataSet[i].get_macroPixel().get_detector() == 4){
					hdt4->Fill(td);
				}
				if(TMath::Abs(td) <= 10){
					float hood = dataSet[i].get_neighborhoodness(&dataSet[i-1]);
					if(hood>0){
						if(dataSet[i].get_macroPixel().get_detector() == 1){
							h1->Fill(dataSet[i].get_energy(), dataSet[i-1].get_energy());
							h1->Fill(dataSet[i-1].get_energy(), dataSet[i].get_energy());
						}					if(dataSet[i].get_macroPixel().get_detector() == 2){
							h2->Fill(dataSet[i].get_energy(), dataSet[i-1].get_energy());
							h2->Fill(dataSet[i-1].get_energy(), dataSet[i].get_energy());
						}					if(dataSet[i].get_macroPixel().get_detector() == 3){
							h3->Fill(dataSet[i].get_energy(), dataSet[i-1].get_energy());
							h3->Fill(dataSet[i-1].get_energy(), dataSet[i].get_energy());
						}					if(dataSet[i].get_macroPixel().get_detector() == 4){
							h4->Fill(dataSet[i].get_energy(), dataSet[i-1].get_energy());
							h4->Fill(dataSet[i-1].get_energy(), dataSet[i].get_energy());
						}



					}
				}

			}
		}
	}
	dataSet.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

