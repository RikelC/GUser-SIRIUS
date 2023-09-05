#include "RecoilEvent.h"
#include <iostream>

RecoilEvent::RecoilEvent() : Event (){

	fToFCoBo =0;
	fToFNumexo2X =0;
	fToFNumexo2Y =0;
	fCFDToFNumexo2X =0.;
	fCFDToFNumexo2Y =0.;
	flagCoBo =false;
	flagSedNumexo2=false;
	fTrackLength = 0.;

	for(int i=0; i< NPOINTS;i++){
		fTrackX[i]=0.;
		fTrackY[i]=0.;
	}
}

RecoilEvent::~RecoilEvent(){

}
void RecoilEvent::Clear(){
	fDssdEvent.Clear();
	fTrackerCoBoEvent.Clear();
	fTrackerNumexo2Event.Clear();
	fToFCoBo =0;
	fToFNumexo2X =0;
	fToFNumexo2Y = 0;
	fCFDToFNumexo2X = 0.;
	fCFDToFNumexo2Y = 0.;
	flagCoBo = false;
	flagSedNumexo2 = false;
	fTrackLength = 0.;
	fTrackSlopeXZ = 0.;
	fTrackSlopeYZ = 0.;
	for(int i =0; i < NPOINTS; i++){
		fTrackX[i] = 0.;
		fTrackY[i] = 0.;
	}


}


void RecoilEvent::ReconstructTrajectory(){
	//	if(flagCoBo){
	//position 1 : at the tracker foil
	double x1 = fTrackerCoBoEvent.GetBaryCenterXm()/10.;
	double y1 = fTrackerCoBoEvent.GetBaryCenterYm()/10.;
	double z1 = fTrackerCoBoEvent.GetBaryCenterZm()/10.;
	//position 2 : at the DSSD
	double x2 = ((fDssdEvent.GetPixel().GetX() + 1.) *(10./128.)) - 5.;
	double y2 = ((fDssdEvent.GetPixel().GetY() + 1.) *(10./128.)) - 5.;
	double z2 = DISTANCE;//do not remove as we keep copying objects in other class

	double x21 = x2 - x1;
	double y21 = y2 - y1;
	double z21 = z2 - z1;

	fTrackLength = sqrt(x21*x21 + y21*y21 + z21*z21);
	fTrackSlopeXZ = x21/z21;
	fTrackSlopeYZ = y21/z21;
	//-------------track fill part starts here--------
	double z =0.;
	for(int i=0;i< NPOINTS;i++){
		z = z1 + double(i);
		fTrackX[i] = (z -z1)*fTrackSlopeXZ + x1;
		fTrackY[i] = (z -z1)*fTrackSlopeYZ + y1;
	}
	/*	}else{
		fToFCoBo =0;
		fToFNumexo2X =0;
		fToFNumexo2Y =0;
		fCFDToFNumexo2X =0.;
		fCFDToFNumexo2Y =0.;
		fTrackLength = 0.;
		for(int i=0; i< NPOINTS;i++){
		fTrackX[i]=0.;
		fTrackY[i]=0.;
		}


		}*/
}
