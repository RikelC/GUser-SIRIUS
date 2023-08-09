/*!
 * \file MakeDssdEvents.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */

#include "MakeDssdEvents.h" 
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in energy 
bool CompareEnergy(DssdDataPoint const & i, DssdDataPoint const & j)
{
	return std::tie(i.fEnergy, i.fTimeStamp, i.fStrip) > std::tie(j.fEnergy, j.fTimeStamp , j.fStrip);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in time
bool CompareTime(DssdDataPoint const & i, DssdDataPoint const & j)
{
	return std::tie(i.fTimeStamp,  i.fStrip, i.fEnergy) < std::tie(j.fTimeStamp, j.fStrip, j.fEnergy);
}

bool CompareTimeEvent(DssdEvent const & i, DssdEvent const & j)
{	
	return std::tie(i.fTimeStamp, i.fEnergyX, i.fEnergyY) < std::tie(j.fTimeStamp, j.fEnergyX, j.fEnergyY);
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor 
MakeDssdEvents::MakeDssdEvents(){
	fStartNewEvent = false;
	s1 = MyGlobal::GetInstance();
	fTimeDifferenceFrontFront =0;
	fTimeDifferenceFrontBack =0;
	fTimeDifferenceBackFront =0;
	fTimeDifferenceBackBack =0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
MakeDssdEvents::~MakeDssdEvents(){
	myEvent.clear();
	tempEvent.clear();
	tempEventF.clear();
	tempEventB.clear();
	myEvent_front.clear();
	myEvent_back.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Making of DSSD pixels
std::vector<DssdEvent> MakeDssdEvents::Construct(std::string mode, std::vector<DssdDataPoint> &dataSet, TGraph* gr1, TGraph *gr, TGraph* gr2){
	std::vector<DssdEvent> eventSet;
	if(!dataSet.empty()){
		//-----------------------
		// Add previous tempEvent
		//------------------------

		if(mode.compare("FB")==0){
			if(tempEvent.size() >0){

				if(tempEvent.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEvent.begin(),tempEvent.end());
				else dataSet.insert (dataSet.begin(),tempEvent.begin()+(tempEvent.size()/2),tempEvent.end());
				//cout<<"dataSet size "<<dataSet.size()<<"  temp size "<<tempEvent.size()<<" pixels "<<fPixelCounter<<endl;
				tempEvent.clear();
				//fPixelCounter =0;
			}
		}
		else if(mode.compare("F")==0){
			if(tempEventF.size() >0){
				if(tempEventF.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEventF.begin(),tempEventF.end());
				else dataSet.insert (dataSet.begin(),tempEventF.begin()+(tempEventF.size()/2),tempEventF.end());
				tempEventF.clear();
			}
		}

		else if(mode.compare("B")==0){
			if(tempEventB.size() >0){
				if(tempEventB.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEventB.begin(),tempEventB.end());
				else dataSet.insert (dataSet.begin(),tempEventB.begin()+(tempEventB.size()/2),tempEventB.end());
				tempEventB.clear();
			}
		}

		//------------------------
		for(ulint i = 0;i < dataSet.size();i++){
			gr1->SetPoint(i,i,dataSet[i].GetTimeStamp());
		}
		//---------sort the events in time-----------
		std::sort(dataSet.begin(), dataSet.end(),CompareTime);
		//------------------reConstruct events based on coincidence------------

		myEvent.push_back(dataSet[0]);
		//cout<<"start-----------------------------"<<endl;
		//cout<<"dt windows ff "<<s1->ff_window<<"  fb "<<s1->fb_window<<"  bf "<<s1->bf_window<<"  bb "<<s1->bb_window<<endl;
		//cout<<"dataSet at "<<0<<" time "<<dataSet[0].GetTimeStamp()<<"  strip "<<dataSet[0].GetStrip()<<"  energy "<<dataSet[0].GetEnergy()<<endl;
		gr->SetPoint(0,0, dataSet[0].GetTimeStamp());
		for(ulint i = 1;i < dataSet.size();i++){

			//cout<<"dataSet at "<<i<<" time "<<dataSet[i].GetTimeStamp()<<"  strip "<<dataSet[i].GetStrip()<<"  energy "<<dataSet[i].GetEnergy()<<endl;
			fStartNewEvent = false;


			llint td =static_cast<llint>( dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());
			gr->SetPoint(i,i, dataSet[i].GetTimeStamp());
			gr2->SetPoint(i,i, td);
			//if(td < 0) cout<<" neg time "<<td<<endl;
			//---------------------delta T--------
			if(dataSet[i].IsAFrontStrip()){
				if(dataSet[i-1].IsAFrontStrip()){
					fTimeDifferenceFrontFront =static_cast<llint>( dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());
					//cout<<" ff "<<" dt "<<fTimeDifferenceFrontFront<<endl;					
					if(TMath::Abs(fTimeDifferenceFrontFront) <= s1->ff_window){
						myEvent.push_back(dataSet[i]);
						//cout<<"data pused ff"<<endl;
					}else fStartNewEvent = true;

				}else{
					fTimeDifferenceFrontBack =  static_cast<llint>(dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());
					//cout<<" fb "<<" dt "<<fTimeDifferenceFrontBack<<endl;					
					if(TMath::Abs(fTimeDifferenceFrontBack) <= s1->fb_window){
						myEvent.push_back(dataSet[i]);
						//cout<<"data pused fb"<<endl;
					}else fStartNewEvent = true;    
				}

			}
			else{
				if(dataSet[i-1].IsAFrontStrip()){
					fTimeDifferenceBackFront =  static_cast<llint>(dataSet[i].GetTimeStamp() -  dataSet[i-1].GetTimeStamp());
					//cout<<" bf "<<" dt "<<fTimeDifferenceBackFront<<endl;					
					if(TMath::Abs(fTimeDifferenceBackFront) <= s1->bf_window){
						myEvent.push_back(dataSet[i]);
						//cout<<"data pused bf"<<endl;
					}else fStartNewEvent = true;
				}
				else{
					fTimeDifferenceBackBack =  static_cast<llint>(dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());
					//cout<<" bb "<<" dt "<<fTimeDifferenceBackBack<<endl;					
					if(TMath::Abs(fTimeDifferenceBackBack) <=s1->bb_window){
						myEvent.push_back(dataSet[i]);
						//cout<<"data pused bb"<<endl;
					}else fStartNewEvent = true;
				}
			}

			//---------------make pixel-----------
			if(fStartNewEvent == true){


				if(mode.compare("FB")==0){
					fEvtPoint = DetermineDssdEvent(myEvent);
					if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
				}
				else if (mode.compare("F")==0){
					fEvtPoint = DetermineDssdFrontEvent(myEvent);
					if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() ==-100) 	eventSet.push_back(fEvtPoint);
				}
				else if (mode.compare("B")==0){fEvtPoint = DetermineDssdBackEvent(myEvent);
					if(fEvtPoint.GetPixel().GetX() ==-100 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
				}

				//---------push the new point-----
				myEvent.push_back(dataSet[i]);
			}
		}
		//---------------------------
		// treatment of the last event
		// ----------------------------
		if(mode.compare("FB")==0){
			fEvtPoint = DetermineDssdEvent(myEvent);
			if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
		}
		else if (mode.compare("F")==0){
			fEvtPoint = DetermineDssdFrontEvent(myEvent);
			if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() ==-100) 	eventSet.push_back(fEvtPoint);
		}
		else if (mode.compare("B")==0){fEvtPoint = DetermineDssdBackEvent(myEvent);
			if(fEvtPoint.GetPixel().GetX() ==-100 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
		}
		//------------clear the data container--
		dataSet.clear();
	}

	//cout<<"end-----------------------------"<<endl;
	return eventSet;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Making of DSSD pixels
std::vector<DssdEvent> MakeDssdEvents::Construct(std::string mode, std::vector<DssdDataPoint> &dataSet, TH1 *h_ff=nullptr, TH1 *h_fb=nullptr, TH1 *h_bf=nullptr, TH1 *h_bb=nullptr){
	std::vector<DssdEvent> eventSet;
	if(!dataSet.empty()){
		//-----------------------
		// Add previous tempEvent
		//------------------------
		if(mode.compare("FB")==0){
			if(tempEvent.size() >0){

				if(tempEvent.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEvent.begin(),tempEvent.end());
				else dataSet.insert (dataSet.begin(),tempEvent.begin()+(tempEvent.size()/2),tempEvent.end());
				//cout<<"dataSet size "<<dataSet.size()<<"  temp size "<<tempEvent.size()<<" pixels "<<fPixelCounter<<endl;
				tempEvent.clear();
				//fPixelCounter =0;
			}
		}
		else if(mode.compare("F")==0){
			if(tempEventF.size() >0){
				if(tempEventF.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEventF.begin(),tempEventF.end());
				else dataSet.insert (dataSet.begin(),tempEventF.begin()+(tempEventF.size()/2),tempEventF.end());
				tempEventF.clear();
			}
		}

		else if(mode.compare("B")==0){
			if(tempEventB.size() >0){
				if(tempEventB.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEventB.begin(),tempEventB.end());
				else dataSet.insert (dataSet.begin(),tempEventB.begin()+(tempEventB.size()/2),tempEventB.end());
				tempEventB.clear();
			}
		}

		//---------sort the events in time-----------
		std::sort(dataSet.begin(), dataSet.end(),CompareTime);

		//			cout<<"i "<<0<<" time "<<dataSet[0].GetTimeStamp() <<"  strip "<<dataSet[0].GetStrip()<<"  energy "<<dataSet[0].GetEnergy()<<" size "<< dataSet.size()<<endl;
		//------------------reConstruct events based on coincidence------------
		myEvent.push_back(dataSet[0]);
		for(ulint i = 1;i < dataSet.size();i++){
			fStartNewEvent = false;
			///cout<<"i "<<i<<" time "<<dataSet[i].GetTimeStamp() <<"  strip "<<dataSet[i].GetStrip()<<"  energy "<<dataSet[i].GetEnergy()<<endl;
			//---------------------delta T--------
			if(dataSet[i].IsAFrontStrip()){
				if(dataSet[i-1].IsAFrontStrip()){
					fTimeDifferenceFrontFront =static_cast<llint>( dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());
					if(h_ff)h_ff->Fill (fTimeDifferenceFrontFront);
					if(TMath::Abs(fTimeDifferenceFrontFront) <= s1->ff_window){
						myEvent.push_back(dataSet[i]);
					}else fStartNewEvent = true;

				}else{
					fTimeDifferenceFrontBack =  static_cast<llint>(dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());
					if(h_fb)h_fb->Fill(fTimeDifferenceFrontBack);
					if(TMath::Abs(fTimeDifferenceFrontBack) <= s1->fb_window){
						myEvent.push_back(dataSet[i]);
					}else fStartNewEvent = true;    
				}

			}
			else{
				if(dataSet[i-1].IsAFrontStrip()){
					fTimeDifferenceBackFront =  static_cast<llint>(dataSet[i].GetTimeStamp() -  dataSet[i-1].GetTimeStamp());
					if(h_bf)h_bf->Fill(fTimeDifferenceBackFront);
					if(TMath::Abs(fTimeDifferenceBackFront) <= s1->bf_window){
						myEvent.push_back(dataSet[i]);
					}else fStartNewEvent = true;
				}
				else{
					fTimeDifferenceBackBack =  static_cast<llint>(dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());
					if(h_bb)h_bb->Fill (fTimeDifferenceBackBack);
					if(TMath::Abs(fTimeDifferenceBackBack) <=s1->bb_window){
						myEvent.push_back(dataSet[i]);
					}else fStartNewEvent = true;
				}
			}

			//---------------make pixel-----------
			if(fStartNewEvent == true){
				//cout<<"myEvent size "<<myEvent.size()<<endl;
				if(mode.compare("FB")==0){
					fEvtPoint = DetermineDssdEvent(myEvent);
					if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
				}
				else if (mode.compare("F")==0){
					fEvtPoint = DetermineDssdFrontEvent(myEvent);
					if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() ==-100) 	eventSet.push_back(fEvtPoint);
				}
				else if (mode.compare("B")==0){fEvtPoint = DetermineDssdBackEvent(myEvent);
					if(fEvtPoint.GetPixel().GetX() ==-100 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
				}
				//---------push the new point-----
				myEvent.push_back(dataSet[i]);
			}
		}
		//---------------------------
		//Treatment of the last event
		//---------------------------
		if(mode.compare("FB")==0){
			fEvtPoint = DetermineDssdEvent(myEvent);
			if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
		}
		else if (mode.compare("F")==0){
			fEvtPoint = DetermineDssdFrontEvent(myEvent);
			if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() ==-100) 	eventSet.push_back(fEvtPoint);
		}
		else if (mode.compare("B")==0){fEvtPoint = DetermineDssdBackEvent(myEvent);
			if(fEvtPoint.GetPixel().GetX() ==-100 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
		}

		//------------clear the data container--
		dataSet.clear();

	}

	return eventSet;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------


//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Making of DSSD pixels
std::vector<DssdEvent> MakeDssdEvents::Construct(std::string mode, std::vector<DssdDataPoint> &dataSet){
	std::vector<DssdEvent> eventSet;
	if(!dataSet.empty()){
		//-----------------------
		// Add previous tempEvent
		//------------------------

		if(mode.compare("FB")==0){
			if(tempEvent.size() >0){

				if(tempEvent.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEvent.begin(),tempEvent.end());
				else dataSet.insert (dataSet.begin(),tempEvent.begin()+(tempEvent.size()/2),tempEvent.end());
				//cout<<"dataSet size "<<dataSet.size()<<"  temp size "<<tempEvent.size()<<" pixels "<<fPixelCounter<<endl;
				tempEvent.clear();
				//fPixelCounter =0;
			}
		}
		else if(mode.compare("F")==0){
			if(tempEventF.size() >0){
				if(tempEventF.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEventF.begin(),tempEventF.end());
				else dataSet.insert (dataSet.begin(),tempEventF.begin()+(tempEventF.size()/2),tempEventF.end());
				tempEventF.clear();
			}
		}

		else if(mode.compare("B")==0){
			if(tempEventB.size() >0){
				if(tempEventB.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEventB.begin(),tempEventB.end());
				else dataSet.insert (dataSet.begin(),tempEventB.begin()+(tempEventB.size()/2),tempEventB.end());
				tempEventB.clear();
			}
		}


		//---------sort the events in time-----------
		std::sort(dataSet.begin(), dataSet.end(),CompareTime);
		//------------------reConstruct events based on coincidence------------
		myEvent.push_back(dataSet[0]);
		for(ulint i = 1;i < dataSet.size();i++){
			fStartNewEvent = false;
			//cout<<"time "<<dataSet[i].time <<"  strip "<<dataSet[i].strip<<"  energy "<<dataSet[i].energy<<endl;
			//---------------------delta T--------
			if(dataSet[i].IsAFrontStrip()){
				if(dataSet[i-1].IsAFrontStrip()){
					fTimeDifferenceFrontFront =static_cast<llint>( dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());

					if(TMath::Abs(fTimeDifferenceFrontFront) <= s1->ff_window){
						myEvent.push_back(dataSet[i]);
					}else fStartNewEvent = true;

				}else{
					fTimeDifferenceFrontBack =  static_cast<llint>(dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());

					if(TMath::Abs(fTimeDifferenceFrontBack) <= s1->fb_window){
						myEvent.push_back(dataSet[i]);
					}else fStartNewEvent = true;    
				}

			}
			else{
				if(dataSet[i-1].IsAFrontStrip()){
					fTimeDifferenceBackFront =  static_cast<llint>(dataSet[i].GetTimeStamp() -  dataSet[i-1].GetTimeStamp());

					if(TMath::Abs(fTimeDifferenceBackFront) <= s1->bf_window){
						myEvent.push_back(dataSet[i]);
					}else fStartNewEvent = true;
				}
				else{
					fTimeDifferenceBackBack =  static_cast<llint>(dataSet[i].GetTimeStamp() - dataSet[i-1].GetTimeStamp());

					if(TMath::Abs(fTimeDifferenceBackBack) <=s1->bb_window){
						myEvent.push_back(dataSet[i]);
					}else fStartNewEvent = true;
				}
			}

			//---------------make pixel-----------
			if(fStartNewEvent == true){
				//cout<<"myEvent size "<<myEvent.size()<<endl;
				if(mode.compare("FB")==0){
					fEvtPoint = DetermineDssdEvent(myEvent);
					if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
				}
				else if (mode.compare("F")==0){
					fEvtPoint = DetermineDssdFrontEvent(myEvent);
					if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() ==-100) 	eventSet.push_back(fEvtPoint);
				}
				else if (mode.compare("B")==0){fEvtPoint = DetermineDssdBackEvent(myEvent);
					if(fEvtPoint.GetPixel().GetX() ==-100 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
				}
				//---------push the new point-----
				myEvent.push_back(dataSet[i]);
			}
		}
		//---------------------------
		//Treatment of the last event
		//---------------------------
		//cout<<"myEvent size "<<myEvent.size()<<endl;
		if(mode.compare("FB")==0){
			fEvtPoint = DetermineDssdEvent(myEvent);
			if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
		}
		else if (mode.compare("F")==0){
			fEvtPoint = DetermineDssdFrontEvent(myEvent);
			if(fEvtPoint.GetPixel().GetX() >= 0 && fEvtPoint.GetPixel().GetY() ==-100) 	eventSet.push_back(fEvtPoint);
		}
		else if (mode.compare("B")==0){fEvtPoint = DetermineDssdBackEvent(myEvent);
			if(fEvtPoint.GetPixel().GetX() ==-100 && fEvtPoint.GetPixel().GetY() >= 0) 	eventSet.push_back(fEvtPoint);
		}

		//------------clear the data container--
		dataSet.clear();

	}

	return eventSet;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------

//-------------
// make event
//------------
DssdEvent MakeDssdEvents::DetermineDssdEvent(std::vector<DssdDataPoint> &myEvent){
	DssdEvent event;
	event.Clear();
	if(!myEvent.empty()){
		double emax_front = 0., emax_back =0.;
		int emax_front_strip = 0, emax_back_strip = 0;
		ullint emax_front_timestamp =0;
		double emax_front_time =0.;
		if(s1->sum_neighboring_strips){
			//-----------sum-energy of neighboring strips
			for(ulint n =0; n < myEvent.size();n++){
				if(myEvent[n].IsAFrontStrip())myEvent_front.push_back(myEvent[n]);
				else myEvent_back.push_back(myEvent[n]);
			}
			//------
			// front
			// -----
			if(myEvent_front.size() >1){
				//sort in energy in descending order
				sort(myEvent_front.begin(), myEvent_front.end(),CompareEnergy);
				//assumptions:
				//1. maxE strip (i) defines the postion
				//2. the other strips must be the neighboring one, i+1 or i-1

				ulint element =1, i =1;
				ulint old_size = myEvent_front.size();
				while(element != old_size){
					element++;
					if(!myEvent_front[i].IsANeighboringStripOf(myEvent_front[0])){
						//ignore induced charge events == non neighboring strip
						myEvent_front.erase ( myEvent_front.begin()+i);
					}
					else{i++;}
				}
			}
			//------
			//back
			//------
			if(myEvent_back.size() >1){
				//sort in energy in descending order
				sort(myEvent_back.begin(), myEvent_back.end(),CompareEnergy);

				ulint element =1, i =1;
				ulint old_size = myEvent_back.size();
				while(element != old_size){
					element++;
					if(!myEvent_back[i].IsANeighboringStripOf(myEvent_back[0])){
						//ignore induced charge events == non neighboring strip
						myEvent_back.erase ( myEvent_back.begin()+i);
					}
					else{i++;}
				}
			}
			//-----------------
			//sum the energies
			//-----------------

			//------
			//front
			//--------
			for(ulint n =0; n < myEvent_front.size();n++){
				emax_front += myEvent_front[n].GetEnergy();
			}
			//-----
			//back
			//-----
			for(ulint n =0; n < myEvent_back.size();n++){
				emax_back += myEvent_back[n].GetEnergy();
			}
			//position maxE strips
			if(!myEvent_front.empty()){
				emax_front_strip =  myEvent_front[0].GetStrip();
				//time = time of maxE front 
				emax_front_timestamp = myEvent_front[0].GetTimeStamp();
				emax_front_time = myEvent_front[0].GetCFDTime();
			}
			if(!myEvent_back.empty()){
				emax_back_strip =  myEvent_back[0].GetStrip();
			}

		}else{
			//---------get max energy strip
			for(ulint n =0; n < myEvent.size();n++){
				//get max energy strip
				if(myEvent[n].IsAFrontStrip()){
					if(myEvent[n].GetEnergy() > emax_front){
						emax_front = myEvent[n].GetEnergy();
						emax_front_strip = myEvent[n].GetStrip();
						emax_front_timestamp = myEvent[n].GetTimeStamp();
						emax_front_time = myEvent[n].GetCFDTime();
					}
				}else{

					if(myEvent[n].GetEnergy() > emax_back){
						emax_back = myEvent[n].GetEnergy();
						emax_back_strip =myEvent[n].GetStrip();
					}
				}
			}
		}
		//-----------------------------
		//Set Values to the event point
		//----------------------------
		if(emax_front > 0. && emax_back > 0.){
			DssdPixel pixel(emax_front_strip,emax_back_strip -128);
			event.SetPixel(pixel);
			event.SetTimeStamp(emax_front_timestamp);
			event.SetCFDTime(emax_front_time);
			event.SetEnergyX(emax_front);
			event.SetEnergyY(emax_back);
			//fPixelCounter++;
		}
		//------------------------------------------
		// lonely events usually the last data points
		//-------------------------------------------
		else if(emax_front ==0. || emax_back == 0.){
			//cout<<"------------------"<<endl;
			//for(int i = 0; i< myEvent.size();i++)cout<<"strip "<<myEvent[i].strip<<"  energy "<<myEvent[i].energy<<"  time "<<myEvent[i].time<<endl;
			if(tempEvent.size() <s1->buffer_size)tempEvent.insert(tempEvent.end(), myEvent.begin(), myEvent.end());
		}
		//--------------------------------
		// Clear memory for the next pixel
		//--------------------------------
		myEvent.clear();
		if(s1->sum_neighboring_strips){
			myEvent_back.clear();
			myEvent_front.clear();
		}
	}
	return event;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//-------------
// make  Front event only
//------------
DssdEvent MakeDssdEvents::DetermineDssdFrontEvent(std::vector<DssdDataPoint> &myEvent){
	DssdEvent event;
	event.Clear();
	if(!myEvent.empty()){
		double emax_front = 0.;
		int emax_front_strip = 0;
		ullint emax_front_timestamp =0;
		double emax_front_time =0.;
		if(s1->sum_neighboring_strips){
			//-----------sum-energy of neighboring strips
			for(ulint n =0; n < myEvent.size();n++){
				if(myEvent[n].IsAFrontStrip())myEvent_front.push_back(myEvent[n]);
			}
			//------
			// front
			// -----
			if(myEvent_front.size() >1){
				//sort in energy in descending order
				sort(myEvent_front.begin(), myEvent_front.end(),CompareEnergy);
				//assumptions:
				//1. maxE strip (i) defines the postion
				//2. the other strips must be the neighboring one, i+1 or i-1

				ulint element =1, i =1;
				ulint old_size = myEvent_front.size();
				while(element != old_size){
					element++;
					if(!myEvent_front[i].IsANeighboringStripOf(myEvent_front[0])){
						//ignore induced charge events == non neighboring strip
						myEvent_front.erase ( myEvent_front.begin()+i);
					}
					else{i++;}
				}
			}

			//-----------------
			//sum the energies
			//-----------------

			//------
			//front
			//--------
			for(ulint n =0; n < myEvent_front.size();n++){
				emax_front += myEvent_front[n].GetEnergy();
			}
			//position maxE strips
			if(!myEvent_front.empty()){
				emax_front_strip =  myEvent_front[0].GetStrip();
				//time = time of maxE front 
				emax_front_timestamp = myEvent_front[0].GetTimeStamp();
				emax_front_time = myEvent_front[0].GetCFDTime();
			}
		}else{
			//---------get max energy strip
			for(ulint n =0; n < myEvent.size();n++){
				//get max energy strip
				if(myEvent[n].IsAFrontStrip()){
					if(myEvent[n].GetEnergy() > emax_front){
						emax_front = myEvent[n].GetEnergy();
						emax_front_strip = myEvent[n].GetStrip();
						emax_front_timestamp = myEvent[n].GetTimeStamp();
						emax_front_time = myEvent[n].GetCFDTime();
					}
				}
			}
		}
		//-----------------------------
		//Set Values to the event point
		//----------------------------
		if(emax_front > 0.){
			DssdPixel pixel(emax_front_strip, -100);
			event.SetPixel(pixel);
			event.SetTimeStamp(emax_front_timestamp);
			event.SetCFDTime(emax_front_time);
			event.SetEnergyX(emax_front);
			event.SetEnergyY(0.);
			//fPixelCounter++;
		}
		//------------------------------------------
		// lonely events usually the last data points
		//-------------------------------------------
		else if(emax_front ==0.){
			//cout<<"------------------"<<endl;
			//for(int i = 0; i< myEvent.size();i++)cout<<"strip "<<myEvent[i].strip<<"  energy "<<myEvent[i].energy<<"  time "<<myEvent[i].time<<endl;
			if(tempEventF.size() <s1->buffer_size)tempEventF.insert(tempEventF.end(), myEvent.begin(), myEvent.end());
		}
		// Clear memory for the next pixel
		//--------------------------------
		myEvent.clear();
		if(s1->sum_neighboring_strips){
			myEvent_front.clear();
		}
	}
	return event;
}

//-------------
// make event
//------------
DssdEvent MakeDssdEvents::DetermineDssdBackEvent(std::vector<DssdDataPoint> &myEvent){
	DssdEvent event;
	event.Clear();
	if(!myEvent.empty()){
		double emax_back =0.;
		int emax_back_strip = 0;
		ullint emax_back_timestamp =0;
		double emax_back_time =0.;
		if(s1->sum_neighboring_strips){
			//-----------sum-energy of neighboring strips
			for(ulint n =0; n < myEvent.size();n++){
				if(myEvent[n].IsABackStrip())
					myEvent_back.push_back(myEvent[n]);
			}

			//------
			//back
			//------
			if(myEvent_back.size() >1){
				//sort in energy in descending order
				sort(myEvent_back.begin(), myEvent_back.end(),CompareEnergy);

				ulint element =1, i =1;
				ulint old_size = myEvent_back.size();
				while(element != old_size){
					element++;
					if(!myEvent_back[i].IsANeighboringStripOf(myEvent_back[0])){
						//ignore induced charge events == non neighboring strip
						myEvent_back.erase ( myEvent_back.begin()+i);
					}
					else{i++;}
				}
			}
			//-----------------
			//sum the energies
			//-----------------

			//-----
			//back
			//-----
			for(ulint n =0; n < myEvent_back.size();n++){
				emax_back += myEvent_back[n].GetEnergy();
			}

			if(!myEvent_back.empty()){
				emax_back_strip =  myEvent_back[0].GetStrip();
				emax_back_timestamp = myEvent_back[0].GetTimeStamp();
				emax_back_time = myEvent_back[0].GetCFDTime();
			}

		}else{
			//---------get max energy strip
			for(ulint n =0; n < myEvent.size();n++){
				//get max energy strip
				if(myEvent[n].IsABackStrip()){


					if(myEvent[n].GetEnergy() > emax_back){
						emax_back = myEvent[n].GetEnergy();
						emax_back_strip =myEvent[n].GetStrip();
						emax_back_timestamp = myEvent[n].GetTimeStamp();
						emax_back_time = myEvent[n].GetCFDTime();
					}
				}
			}
		}
		//-----------------------------
		//Set Values to the event point
		//----------------------------
		if(emax_back > 0.){
			DssdPixel pixel(-100,emax_back_strip -128);
			event.SetPixel(pixel);
			event.SetTimeStamp(emax_back_timestamp);
			event.SetCFDTime(emax_back_time);
			event.SetEnergyX(0.);
			event.SetEnergyY(emax_back);
			//fPixelCounter++;
		}
		//------------------------------------------
		// lonely events usually the last data points
		//-------------------------------------------
		else if( emax_back == 0.){
			//cout<<"------------------"<<endl;
			//for(int i = 0; i< myEvent.size();i++)cout<<"strip "<<myEvent[i].strip<<"  energy "<<myEvent[i].energy<<"  time "<<myEvent[i].time<<endl;
			if(tempEventB.size() <s1->buffer_size)tempEventB.insert(tempEventB.end(), myEvent.begin(), myEvent.end());
		}

		//--------------------------------
		// Clear memory for the next pixel
		//--------------------------------
		myEvent.clear();
		if(s1->sum_neighboring_strips){
			myEvent_back.clear();
		}
	}
	return event;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------


void MakeDssdEvents::SortInTime(std::vector<DssdDataPoint> &dataSet){
	std::sort(dataSet.begin(), dataSet.end(),CompareTime);
}
void MakeDssdEvents::SortInTime(std::vector<DssdEvent> &dataSet){
	std::sort(dataSet.begin(), dataSet.end(),CompareTimeEvent);
}
