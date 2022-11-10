/*!
 * \file GUser.h
 * \author Luc Legeard
 * \brief This class manages user methods
 * \details Modification: Rikel CHAKMA
 *
 */

#ifndef __GUser__
#define __GUser__

#include <TObject.h>
#include "General.h"
#include "GAcq.h"
#include "GDevice.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "MFMExogamFrame.h"
#include "MFMMergeFrame.h"
#include "MFMCoboFrame.h"
#include "MFMExogamFrame.h"
#include "MFMEbyedatFrame.h"
#include "MFMNumExoFrame.h"
#include "MFMSiriusFrame.h"
#include "MFMAllFrames.h"
#include "MFMReaGenericFrame.h"
#include "Rtypes.h"
#include "TRandom.h"
#include "global.h"
#include "dssdData.h"
#include "tunnelData.h"
#include "digitalFilters.h"
#include "digitalCFD.h"
#include "calibration.h"
#include "dssdDataPoint.h"
#include "dssdEvent.h"
#include "dssdEvent.h"

#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>    // std::sort
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <csignal>
#include <stdlib.h>
#include <unistd.h>

#define DSSD_DETECTOR 1
#define TUNNEL_DETECTOR 2

/*! It is the main class of the program. Here, we get the unpacked data.*/
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class GUser : public  GAcq{
	protected:
		//-----------Frames------------
		MFMMergeFrame             * fMergeframe ;/**< MergerFrame type */
		MFMCoboFrame              * fCoboframe ;/**< CoboFrame for the tracker data */
		MFMCommonFrame            * fInsideframe ; /**< MFMCommonFrame type. Used for pointing to frames inside a MFMMergeFrame */
		MFMSiriusFrame            * fSiriusframe ; /**< MFMSiriusFrame type for treating DSSD data.*/
		MFMReaGenericFrame        * fGenericframe;/**< MFMReaGenericFrame type for treating TUnnel data. */
	private:
		//---------User Lib---------
		myGlobal                  * s1;/**< pointer to the instance of the Singleton myGlobal class. */
		digitalFilters            * filter;/**< instance for the digital filter algorithms. */
		digitalCFD                * cfd; /**< instance for the digital constant fraction discriminator. */
		calibration               * calib; /**< instance for doing calibration of the detectors. */
		dssdData                  * dData; /*!< dssdData object*/
		dssdEvent                 * dEvent;/*!< dssdEvent object*/
		tunnelData                * tData;/*!< tunnelData object*/
		//long long int fBoardHitPattern[NBOARDS];
		//---------DSSD Histograms----------
		//TH1I   *** hGain ; //!
		//TH1I   *** hFeedBack; //!
		TH1F     *** hBaseline;//!
		TH1F     *** hNoise;//!
		TH1I     *** hRisetime;//!
		TH2F     *** hTrace_Sum ;  //! -- this silences the error
		TH1F     *** hRaw; //!
		TH1F     *** hCalib; //!
		TH2F     *** hTrap; //!
		TGraph   *** gr_baseline;//!
		TGraph    ** gr_rate_dssdBoard;//!
		TH1I       * h_delT_ff; //!
		TH1I       * h_delT_fb; //!
		TH1I       * h_delT_bf; //!
		TH1I       * h_delT_bb; //!
		TH2F       * h_E_frontBack; //!
		TH2I       * h_DSSD_XY_hit; //!
		TH1I       * h_dssd_count_strip; //!
		TH1I       * h_dssd_count_board; //!
		TH2F       * h_calib_strip; //!
		TH2F       * h_raw_strip; //!
		TH1I       * h_dssdStrip_rate; //!
		TH1I       * h_dssdBoard_rate; //!
		TH2F       * h_dssd_baseline;
		TH2F       * h_dssd_noise;
		TH2F       * h_dssdE_Tof;
		//---------TUNNEL Histograms----------
		TH1I     *** h_tunnelRaw;//!
		TH1I     *** h_tunnelCalib;//!
		TGraph    ** gr_rate_tunnelBoard;//!
		TH2I      ** h_TUNNEL_XY_hit; //!
		TH2F       * h_calib_pad; //!
		TH2F       * h_raw_pad; //!
		TH1I       * h_tunnelPad_rate; //!
		TH1I       * h_tunnelBoard_rate; //!
		TH1I       * h_tunnel_count_pad; //!
		TH1I       * h_tunnel_count_board; //!
		//-----------------------------
		double     * tunnel_rate_pad; //!
		double     * tunnel_rate_board; //!
		ullint      * frameCounter; //!
		double     * dssd_rate_strip; //!
		double     * dssd_rate_board; //!
		ullint     * rate_counterPoint_dssd;//!
		ullint     * rate_counterPoint_tunnel;//!
		ullint    ** dssd_event_counter;//!
		double       dataSet_counter;
		uint         eventnumber;
		ullint       timestamp;
		ullint       old_timeStamp;
		ullint       rate_counter_dssd;
		ullint       rate_counter_tunnel;
		llint        deltaT_rates;
		ushort       gain;
		ushort       trace_size;
		int          dumpsize;
		int          framesize;
		int          maxdump;
		int          channel;
		int          board;
		int          iboard;
		int          stripnumber;
		int          dssdBoardNo;
		int          tunnelBoardNo;
		int          tunnelPadNo;
		int          tunnelDetectorNo;
		uint16_t     value;
		int64_t      rate_calcul_timestamp_lapse;		
		Double_t     Energy;
		double       calibEnergy;
		ULong64_t    Time;		
		dssdDataPoint dPoint;
		std::vector<dssdDataPoint>dssdDataVec;
		std::vector<dssdPixel>dssdEventVec;
		void get_Count_Rates( int &board, int &stripNumber, ullint &time, int type);
	public:
		static GUser *g_instance;
		GUser(GDevice* _fDevIn= NULL, GDevice* _fDevOut= NULL) ;   // default constructor of GUser object
		~GUser() ;
		virtual void InitUser();
		virtual void InitUserRun();
		virtual void User();
		virtual void EndUserRun();
		virtual void EndUser();
		virtual void InitTTreeUser();
		virtual void UserFrame(MFMCommonFrame * commonframe);
		virtual void UserMergeFrame(MFMCommonFrame * commonframe);
		virtual void UserSiriusFrame( MFMCommonFrame * commonframe);
		virtual void UserGenericFrame(MFMCommonFrame * commonframe);
		virtual void UserCoboFrame(MFMCommonFrame * commonframe);
		ClassDef (GUser ,1); // User Treatment of Data

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
