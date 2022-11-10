/*!
 * \file dssdEvent.cc
 * \author Rikel CHAKMA
 * \brief
 * \details
 *
 *
 */

#include "dssdEvent.h" 
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in energy 
bool compareEnergy(dssdDataPoint const & i, dssdDataPoint const & j)
{
	return std::tie(i.energy, i.time, i.strip) > std::tie(j.energy, j.time , j.strip);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! functions for sorting algorithms in time
bool compareTime(dssdDataPoint const & i, dssdDataPoint const & j)
{
	return std::tie(i.time, i.energy, i.strip) < std::tie(j.time, j.energy, j.strip);
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Constructor 
dssdEvent::dssdEvent(){
	start_new_event = false;
	s1 = myGlobal::getInstance();
	td_ff =0;
	td_fb =0;
	td_bf =0;
	td_bb =0;
	//pixel_counter =0;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
dssdEvent::~dssdEvent(){
	myEvent.clear();
	tempEvent.clear();
	myEvent_front.clear();
	myEvent_back.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Making of DSSD pixels
std::vector<dssdPixel> dssdEvent::construct(std::vector<dssdDataPoint> &dataSet){
	std::vector<dssdPixel> eventSet;
	//-----------------------
	// Add previous tempEvent
	//------------------------

	if(tempEvent.size() >0){

		if(tempEvent.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEvent.begin(),tempEvent.end());
		else dataSet.insert (dataSet.begin(),tempEvent.begin()+(s1->buffer_size/2),tempEvent.end());
		//cout<<"dataSet size "<<dataSet.size()<<"  temp size "<<tempEvent.size()<<" pixels "<<pixel_counter<<endl;
		tempEvent.clear();
		//pixel_counter =0;
	}
	//------------------------
	if(!dataSet.empty()){
		//---------sort the events in time-----------
		std::sort(dataSet.begin(), dataSet.end(),compareTime);
		//------------------reconstruct events based on coincidence------------
		myEvent.push_back(dataSet[0]);
		for(ushort i = 1;i < dataSet.size();i++){
			start_new_event = false;
			//---------------------delta T--------
			if(dataSet[i].is_frontStrip()){
				if(dataSet[i-1].is_frontStrip()){
					td_ff =static_cast<llint>( dataSet[i].get_time() - dataSet[i-1].get_time());
					if(TMath::Abs(td_ff) <= s1->ff_window){
						myEvent.push_back(dataSet[i]);
					}else start_new_event = true;

				}else{
					td_fb =  static_cast<llint>(dataSet[i].get_time() - dataSet[i-1].get_time());
					if(TMath::Abs(td_fb) <= s1->fb_window){
						myEvent.push_back(dataSet[i]);
					}else start_new_event = true;    
				}

			}
			else{
				if(dataSet[i-1].is_frontStrip()){
					td_bf =  static_cast<llint>(dataSet[i].get_time() -  dataSet[i-1].get_time());
					if(TMath::Abs(td_bf) <= s1->bf_window){
						myEvent.push_back(dataSet[i]);
					}else start_new_event = true;
				}
				else{
					td_bb =  static_cast<llint>(dataSet[i].get_time() - dataSet[i-1].get_time());
					if(TMath::Abs(td_bb) <=s1->bb_window){
						myEvent.push_back(dataSet[i]);
					}else start_new_event = true;
				}
			}

			//---------------make pixel-----------
			if(start_new_event == true){
				ePoint = determine_dssd_pixel(myEvent);
				eventSet.push_back(ePoint);				
				//---------push the new point-----
				myEvent.push_back(dataSet[i]);
			}
		}
		//---------------------------
		// treatment of the last event
		// ----------------------------
		ePoint = determine_dssd_pixel(myEvent);
		eventSet.push_back(ePoint);
		//------------clear the data container--
		dataSet.clear();
	}

	return eventSet;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Making of DSSD pixels
std::vector<dssdPixel> dssdEvent::construct(std::vector<dssdDataPoint> &dataSet, TH1 *h_ff=nullptr, TH1 *h_fb=nullptr, TH1 *h_bf=nullptr, TH1 *h_bb=nullptr){
	std::vector<dssdPixel> eventSet;
	//-----------------------
	// Add previous tempEvent
	//------------------------

	if(tempEvent.size() >0){

		if(tempEvent.size() < s1->buffer_size) dataSet.insert (dataSet.begin(),tempEvent.begin(),tempEvent.end());
		else dataSet.insert (dataSet.begin(),tempEvent.begin()+(s1->buffer_size/2),tempEvent.end());
		//cout<<"dataSet size "<<dataSet.size()<<"  temp size "<<tempEvent.size()<<" pixels "<<pixel_counter<<endl;
		tempEvent.clear();
		//pixel_counter =0;
	}
	if(!dataSet.empty()){
		//---------sort the events in time-----------
		std::sort(dataSet.begin(), dataSet.end(),compareTime);
		//------------------reconstruct events based on coincidence------------
		myEvent.push_back(dataSet[0]);
		for(ushort i = 1;i < dataSet.size();i++){
			start_new_event = false;
			//cout<<"time "<<dataSet[i].time <<"  strip "<<dataSet[i].strip<<"  energy "<<dataSet[i].energy<<endl;
			//---------------------delta T--------
			if(dataSet[i].is_frontStrip()){
				if(dataSet[i-1].is_frontStrip()){
					td_ff =static_cast<llint>( dataSet[i].get_time() - dataSet[i-1].get_time());
					if(h_ff)h_ff->Fill (td_ff);
					if(TMath::Abs(td_ff) <= s1->ff_window){
						myEvent.push_back(dataSet[i]);
					}else start_new_event = true;

				}else{
					td_fb =  static_cast<llint>(dataSet[i].get_time() - dataSet[i-1].get_time());
					if(h_fb)h_fb->Fill(td_fb);
					if(TMath::Abs(td_fb) <= s1->fb_window){
						myEvent.push_back(dataSet[i]);
					}else start_new_event = true;    
				}

			}
			else{
				if(dataSet[i-1].is_frontStrip()){
					td_bf =  static_cast<llint>(dataSet[i].get_time() -  dataSet[i-1].get_time());
					if(h_bf)h_bf->Fill(td_bf);
					if(TMath::Abs(td_bf) <= s1->bf_window){
						myEvent.push_back(dataSet[i]);
					}else start_new_event = true;
				}
				else{
					td_bb =  static_cast<llint>(dataSet[i].get_time() - dataSet[i-1].get_time());
					if(h_bb)h_bb->Fill (td_bb);
					if(TMath::Abs(td_bb) <=s1->bb_window){
						myEvent.push_back(dataSet[i]);
					}else start_new_event = true;
				}
			}

			//---------------make pixel-----------
			if(start_new_event == true){
				//cout<<"myEvent size "<<myEvent.size()<<endl;
				ePoint = determine_dssd_pixel(myEvent);
				eventSet.push_back(ePoint);				
				//---------push the new point-----
				myEvent.push_back(dataSet[i]);
			}
		}
		//---------------------------
		//Treatment of the last event
		//---------------------------
		//cout<<"myEvent size "<<myEvent.size()<<endl;
		ePoint = determine_dssd_pixel(myEvent);
		eventSet.push_back(ePoint);
		//------------clear the data container--
		dataSet.clear();

	}

	return eventSet;
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//-------------
// make pixel
//------------
dssdPixel dssdEvent::determine_dssd_pixel(std::vector<dssdDataPoint> &myEvent){
	dssdPixel event;
	event.clear();
	if(!myEvent.empty()){
		double emax_front = 0., emax_back =0.;
		int emax_front_strip = 0, emax_back_strip = 0;
		ullint emax_front_time =0;
		if(s1->sum_neighboring_strips){
			//-----------sum-energy of neighboring strips
			for(ushort n =0; n < myEvent.size();n++){
				if(myEvent[n].is_frontStrip())myEvent_front.push_back(myEvent[n]);
				else myEvent_back.push_back(myEvent[n]);
			}
			//------
			// front
			// -----
			if(myEvent_front.size() >1){
				//sort in energy in descending order
				sort(myEvent_front.begin(), myEvent_front.end(),compareEnergy);
				//assumptions:
				//1. maxE strip (i) defines the postion
				//2. the other strips must be the neighboring one, i+1 or i-1

				ushort element =1, i =1;
				ushort old_size = myEvent_front.size();
				while(element != old_size){
					element++;
					if(!myEvent_front[i].is_neighboringStripOf(myEvent_front[0])){
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
				sort(myEvent_back.begin(), myEvent_back.end(),compareEnergy);

				ushort element =1, i =1;
				ushort old_size = myEvent_back.size();
				while(element != old_size){
					element++;
					if(!myEvent_back[i].is_neighboringStripOf(myEvent_back[0])){
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
			for(ushort n =0; n < myEvent_front.size();n++){
				emax_front += myEvent_front[n].get_energy();
			}
			//-----
			//back
			//-----
			for(ushort n =0; n < myEvent_back.size();n++){
				emax_back += myEvent_back[n].get_energy();
			}
			//position maxE strips
			if(!myEvent_front.empty()){
				emax_front_strip =  myEvent_front[0].get_strip();
				//time = time of maxE front 
				emax_front_time = myEvent_front[0].get_time();
			}
			if(!myEvent_back.empty()){
				emax_back_strip =  myEvent_back[0].get_strip();
			}

		}else{
			//---------get max energy strip
			for(ushort n =0; n < myEvent.size();n++){
				//get max energy strip
				if(myEvent[n].is_frontStrip()){
					if(myEvent[n].get_energy() > emax_front){
						emax_front = myEvent[n].get_energy();
						emax_front_strip =myEvent[n].get_strip();
						emax_front_time = myEvent[n].get_time();
					}
				}else{

					if(myEvent[n].get_energy() > emax_back){
						emax_back = myEvent[n].get_energy();
						emax_back_strip =myEvent[n].get_strip();
					}
				}
			}
		}
		//-----------------------------
		//Set Values to the event point
		//----------------------------
		if(emax_front > 0. && emax_back > 0.){
			event.set_X(emax_front_strip);
			event.set_Y(emax_back_strip -128);
			event.set_time(emax_front_time);
			event.set_energyX( emax_front);
			event.set_energyY(emax_back);
			//pixel_counter++;
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

