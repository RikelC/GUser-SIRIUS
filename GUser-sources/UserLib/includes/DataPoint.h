#ifndef DataPoint_h
#define DataPoint_h 1
#include "Rtypes.h"
class DataPoint
{
	public:
		DataPoint();
		~ DataPoint();
		int strip;
		ULong64_t time;
		Double_t energy;
		void clear(){strip =0; time = 0; energy =0;}
		Bool_t is_frontStrip()const{if(strip < 128)return true; else return false;}
		Bool_t is_backStrip()const{if(strip > 127)return true; else return false;}
		Bool_t is_neighboringStripOf(DataPoint &d){
			if(strip == d.strip+1 || strip == d.strip-1)return true; else return false;
		}


};

#endif
