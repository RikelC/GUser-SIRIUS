#ifndef userTTree_h
#define userTTree_h 1
#include "RTypes.h"

class userTTree
{
	private:
		UInt_t       eventnumber;
		ULong64_t    timestamp;
		UShort_t     gain;
		UShort_t     trace_size;
		UShort_t     Trace[992];
		UShort_t     channel;
		UShort_t     board;
		Uint16_t     value;
		Ushort_t     frametype;//cobo frame = 0, rea generic frame = 1, sirius frame = 2, exogam frame = 3

};

#endif
