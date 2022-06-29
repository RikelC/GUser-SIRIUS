#ifndef MyGlobal_h
#define MyGlobal_h 1


#include <iostream>
#include <string>
#include <fstream>
#include <locale>
#include <vector>
#define timestamp_unit 10 //ns
#define NCHANNELS 16
#define sampling_period 5 //ns
#define Resistance 700. //All the ASIC resistors are 700 k Ohms
//-----------------------
//   DSSD  NUMEXO Boards
//-----------------------

#define Si  2   // channel number
#define PMT 1
#define SED 0  //channel number

using namespace std;

class myGlobal
{
	public:
		static myGlobal *getInstance();

		int nStart_trace;
		int nEnd_trace;
		int pre_trig_buffer;
		int fverbose;
		double FPCSA_GAIN;
		double shaperAmplificationGain;
		std::string dssd_calib_filename;
		std::string tunnel_calib_filename;
		std::string gamma_calib_filename;
		std::string trap_parameter_filename;
		bool sum_neighboring_strips; 
		bool useDefaultFilterParameters;		
		int DSSD_FrontBackGap; 
		std::string filterAlgorithm;
		int default_trap_k;
		int default_trap_m;
		int NBOARDS_TUNNEL; 
		int NBOARDS_DSSD; 
		int NSTRIPS_DSSD;
		int TRACE_SIZE;

		int MB1_P4_BOARD1;
		int MB1_P4_BOARD2;
		int MB1_P5_BOARD1;
		int MB1_P5_BOARD2;

		int MB2_P4_BOARD1;
		int MB2_P4_BOARD2; 
		int MB2_P5_BOARD1;
		int MB2_P5_BOARD2;

		int MB3_P4_BOARD1;
		int MB3_P4_BOARD2;
		int MB3_P5_BOARD1;
		int MB3_P5_BOARD2;

		int MB4_P4_BOARD1;
		int MB4_P4_BOARD2;
		int MB4_P5_BOARD1;
		int MB4_P5_BOARD2;

		string frameName[8];
		int *fBoardList_Tunnel;
		int *fConvertNoBoardIndexLocal_Tunnel;
		int *fBoardList_DSSD;
		int *fConvertNoBoardIndexLocal;
		int ff_window, fb_window, bf_window, bb_window;


		void readRunConfig();
	private:
		myGlobal();
		static myGlobal *instance;
		virtual ~myGlobal();
		std::vector<int> list_dssd;
		std::vector<int> list_tunnel;

};

//called by myGlobal* s1 = myGlobal::getInstance();
/****************

  physical
  strip      0--------------------------->127
  ------
  MB
  strip      64<-----------1------------->64            128 
  | P5 | P4 |=======| P5 | P4 |      1        ^
  ___________________________  ____ ^        |
  |   MB1           MB2       |  P4  |        |
  |                           | ____ |        |
  |                       MB3 |  P5  |        |
  |            DSSD           | ____ |        |
  |                           |               |
  |                           |      64       |
  |            Back           | ____ ^        |
  |                           |  P4  |        |
  |                       MB4 | ____ |        |
  |                           |  P5  |        | 
  |___________________________| ____ |       255
  1
  MB strip  physical strip

//--------------------
MB1<-->Box1
//-------------------
BOX ch ----> HDMI Cable
====P4====
1 ----> 008
2 ----> 009
3 ----> 010
4 ----> 011
5 ----> 012
6 ----> 013
7 ----> 015
8 ----> 056
====P5=====
9 ----> 057
10 ----> 058
11 ----> 059
12 ----> 061
13 ----> 063
14 ----> 064
15 ----> 065
16 ----> 066
//-------------
MB2<-->Box2
//------------
BOX ch ----> HDMI Cable
====P5====
1 ----> 024
2 ----> 025
3 ----> 026
4 ----> 027
5 ----> 028
6 ----> 029
7 ----> 030
8 ----> 031
====P4=====
9 ----> 032
10 ----> 033
11 ----> 034
12 ----> 035
13 ----> 036
14 ----> 037
15 ----> 038
16 ----> 039
//-------------
MB3<-->Box3
//------------
BOX ch ----> HDMI Cable
====P5====
1 ----> 040
2 ----> 041
3 ----> 042
4 ----> 043
5 ----> 044
6 ----> 045
7 ----> 046
8 ----> 047
====P4=====
9 ----> 048
10 ----> 049
11 ----> 050
12 ----> 051
13 ----> 052
14 ----> 053
15 ----> 054
16 ----> 055
//-------------
MB4<-->Box4
//------------
BOX ch ----> HDMI Cable
====P5====
1 ----> 001
2 ----> 002
3 ----> 003
4 ----> 004
5 ----> 005
6 ----> 006
7 ----> 007
8 ----> 014
====P4=====
9 ----> 016
10 ----> 017
11 ----> 018
12 ----> 019
13 ----> 020
14 ----> 021
15 ----> 022
16 ----> 023
***********************/
#endif
