#ifndef EventPoint_h
#define EventPoint_h 1
#include "Rtypes.h"

class EventPoint
{
	public:
		EventPoint();
		~EventPoint();
		//int detectorID;//DSSD,SED,GE,TUNNEL
		//int type;//RECOIL,DECAY
		int X;
		int Y;
		ULong64_t time;
		Double_t energyX;
		Double_t energyY;
		void clear(){
			X = 0; Y = 0; time = 0; energyX = 0; energyY = 0;
		}
};
#endif
