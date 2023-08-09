/*!
 * \file MakeTunnelEvents.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */

#include "MakeTunnelEvents.h" 
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in energy 
bool CompareEnergy(TunnelDataPoint const & i, TunnelDataPoint const & j)
{
	return std::tie(i.fEnergy, i.fTimeStamp) > std::tie(j.fEnergy, j.fTimeStamp);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in time
bool CompareTime(TunnelDataPoint const & i, TunnelDataPoint const & j)
{
	return std::tie(i.fTimeStamp, i.fEnergy) < std::tie(j.fTimeStamp, j.fEnergy);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor 
MakeTunnelEvents::MakeTunnelEvents(){
	s1 = MyGlobal::GetInstance();
	td =0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
MakeTunnelEvents::~MakeTunnelEvents(){
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

void MakeTunnelEvents::Construct(std::vector<TunnelDataPoint> &dataSet, TH2* h1, TH2* h2, TH2* h3, TH2* h4, TH1 *hdt1, TH1* hdt2, TH1* hdt3, TH1* hdt4){
	//------------------------
	if(!dataSet.empty()){
		std::sort(dataSet.begin(), dataSet.end(),CompareTime);
		for(ushort i = 1;i < dataSet.size();i++){

			if(dataSet[i].GetMacroPixel().GetDetector() == dataSet[i-1].GetMacroPixel().GetDetector()){
				td =static_cast<llint>( dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());
				if(dataSet[i].GetMacroPixel().GetDetector() == 1){
					hdt1->Fill(td);
				}		
				if(dataSet[i].GetMacroPixel().GetDetector() == 2){
					hdt2->Fill(td);
				}					
				if(dataSet[i].GetMacroPixel().GetDetector() == 3){
					hdt3->Fill(td);
				}	
				if(dataSet[i].GetMacroPixel().GetDetector() == 4){
					hdt4->Fill(td);
				}
				if(TMath::Abs(td) <= 10){
					float hood = dataSet[i].GetNeighborhoodness(&dataSet[i-1]);
					if(hood>0){
						if(dataSet[i].GetMacroPixel().GetDetector() == 1){
							h1->Fill(dataSet[i].GetEnergy(), dataSet[i-1].GetEnergy());
							h1->Fill(dataSet[i-1].GetEnergy(), dataSet[i].GetEnergy());
						}	
						if(dataSet[i].GetMacroPixel().GetDetector() == 2){
							h2->Fill(dataSet[i].GetEnergy(), dataSet[i-1].GetEnergy());
							h2->Fill(dataSet[i-1].GetEnergy(), dataSet[i].GetEnergy());
						}	
						if(dataSet[i].GetMacroPixel().GetDetector() == 3){
							h3->Fill(dataSet[i].GetEnergy(), dataSet[i-1].GetEnergy());
							h3->Fill(dataSet[i-1].GetEnergy(), dataSet[i].GetEnergy());
						}		
						if(dataSet[i].GetMacroPixel().GetDetector() == 4){
							h4->Fill(dataSet[i].GetEnergy(), dataSet[i-1].GetEnergy());
							h4->Fill(dataSet[i-1].GetEnergy(), dataSet[i].GetEnergy());
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

