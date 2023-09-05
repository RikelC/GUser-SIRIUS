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
#include "TH3.h"
#include "TF1.h"
#include "TGraph.h"
#include "TMath.h"
#include "TMultiGraph.h"
#include "MFMCommonFrame.h"
#include "MFMExogamFrame.h"
#include "MFMMergeFrame.h"
#include "MFMCoboFrame.h"
#include "MFMExogamFrame.h"
#include "MFMEbyedatFrame.h"
#include "MFMNumExoFrame.h"
#include "MFMSiriusFrame.h"
#include "MFMAllFrames.h"
#include "MFMReaGenericFrame.h"
#include "MFMMutantFrame.h"
#include "Rtypes.h"
#include "TRandom.h"
#include "Global.h"
#include "DssdData.h"
#include "TunnelData.h"
#include "DigitalFilters.h"
#include "DigitalCFD.h"
#include "Calibration.h"
#include "DssdDataPoint.h"
#include "Correlation.h"
#include "TrackerNumexo2Event.h"
#include "TunnelDataPoint.h"
#include "MakeDssdEvents.h"
#include "MakeTunnelEvents.h"
#include "TimeAlignment.h"
#include "UTTree.h"
#include "TrackerCoBoData.h"
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

enum Detector {Dssd_Detector = 1, Tunnel_Detector = 2, Tracker_Detector = 3, ExoGam_Detector = 4, Veto_Detector = 5};
enum PrintLevel{ 
	dumpsize1 = 16,
	dumpsize2 = 32,
	dumpsize3 = 48,
	dumpsize4 = 64,
	dumpsize5 = 80,
	dumpsize6 = 96,
	dumpsize7 = 112,
	dumpsize8 = 128,
};

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
		MFMCommonFrame            * frame;
		MFMMutantFrame            * fMutantframe;
		MFMEbyedatFrame           * fEbyedatframe;
	private:
		//----------------------------
		// User Lib pointers
		//----------------------------
		MyGlobal                  * s1;/**< pointer to the instance of the Singleton myGlobal class. */
		DigitalFilters            * filter;/**< instance for the digital filter algorithms. */
		DigitalCFD                * cfd; /**< instance for the digital constant fraction discriminator. */
		Calibration               * calib; /**< instance for doing calibration of the detectors. */
		DssdData                  * dData; /*!< dssdData object*/
		MakeDssdEvents            * dEvent;/*!< dssdEvent object*/
		//dssdEvent                 * trEvent;
		MakeTunnelEvents          * tEvent;/*!<tunnelEvent object*/
		TunnelData                * tData;/*!< tunnelData object*/
		TrackerCoBoData               * coboData;
		TimeAlignment             * timeAlign;
		Correlation               * correlation;
		UTTree                    * userTree;
		RecoilEvent  fRecoilEvent;
		DecayEvent  fDecayEvent;
		//----------------------------
		// DSSD Histogram pointers
		//----------------------------
		//------ 1D Histograms ---
		//TH1I   *** hGain ; //!
		//TH1I   *** hFeedBack; //!
		TH1F     *** h_zeroCTime;//!
		TH1I     *** h_MaxPosTime;//!
		TH1F     *** hRaw; //!
		TH1F     *** hCalib; //!
		TH1F     *** hCalib_GS; //!
		TH1F     *** hTrigger;//!
		TH1F     *** hBaseline;//!
		TH1F     *** hNoise;//!
		TH1I     *** hRisetime;//!
		TH1I       * h_dssd_count_strip;
		TH1I       * h_dssd_count_board;
		TH1I       * h_dssdStrip_rate;
		TH1I       * h_dssdBoard_rate;
		TH1I       * h_delT_ff; 
		TH1I       * h_delT_fb;
		TH1I       * h_delT_bf;
		TH1I       * h_delT_bb;
		TH1F       * h_sum_front;
		TH1F       * h_sum_back;
		//------ 2D Histograms ---
		TH2F     *** hCFD; //!
		TH2F     *** hTrap; //!
		TH2F     *** hTrace_Sum ;  //! -- this silences the error
		TH2F *hCFDTimeDiff[16];
		//TH2F* hToFabove5;
		TH2F       * h_E_frontBack;
		TH2I       * h_DSSD_XY_hit;
		TH2I       * h_DSSD_XY_hit_recoil;
		TH2I       * h_DSSD_XYPhysical_hit_recoil;
		TH2F       * h_calib_strip;
		TH2F       * h_raw_strip;
		TH2F       * h_dssd_baseline;
		TH2F       * h_dssd_noise;


		TH2F       * h_FrontE_ToFNumexo; 
		TH2F       * h_BackE_ToFNumexo; 
		TH2F       * h_FrontE_CFDToFNumexo; 
		TH2F       * h_BackE_CFDToFNumexo; 
		TH2F       * h_stripX_ToFNumexo;
		TH2F       * h_stripY_ToFNumexo;
		TH2F       * h_stripX_CFDToFNumexo;
		TH2F       * h_stripY_CFDToFNumexo;
		TH2F       * h_Efront_Esed;
		TH2F       * h_Eback_Esed;
		TH2F       * h_stripX_Esed;
		TH2F       * h_stripY_Esed;
		TH2F       * h_FrontE_ToFCobo; 
		TH2F       * h_BackE_ToFCobo; 
		TH2F       * h_stripX_ToFCobo; 
		TH2F       * h_stripY_ToFCobo; 
		TH2F       * h_trackZSeDX;
		TH2F       * h_trackZSeDY;
		//TH3F       * h_trackXYZ;
		TH2F       * h_XDistance;
		TH2F       * h_YDistance;

		TH2F       * h_front_traceGS;
		TH2F       * h_back_traceGS;
		TH2F       * h_front_traceNGS;
		TH2F       * h_back_traceNGS;
		TH2F       * h_front_rNGS;
		TH2F       * h_back_rNGS;
		TH2F       * h_front_rGS;
		TH2F       * h_back_rGS;
		TH2F       * h_strip_trigger;
		TH2F       * h_strip_zeroCrTime;
		TH2F       * h_board_zeroCrTime;
		TH2F       * h_strip_MaxPosTime;
		TH2F       * h_board_MaxPosTime;
		TH2F       * h_E_MaxPosTime;
		TH2F       * h_E_zeroCrTime;
		TH2F       * h_strip_SignalHeight;
		//------ Graphs ---
		TGraph   *** grTimestamp;//!		
		TGraph   *** gr_baseline;//!
		TGraph    ** gr_rate_dssdBoard;//!

		//----------------------------
		// TUNNEL Histogram pointers
		//----------------------------
		TH1I     *** h_tunnelRaw;//!
		TH1I     *** h_tunnelCalib;//!
		TGraph    ** gr_rate_tunnelBoard;//!
		TH2I      ** h_TUNNEL_XY_hit; //!
		TH2F       * h_calib_pad;
		TH2F       * h_raw_pad;
		TH1I       * h_tunnelPad_rate;
		TH1I       * h_tunnelBoard_rate;
		TH1I       * h_tunnel_count_pad;
		TH1I       * h_tunnel_count_board;
		TH1I       * htunnel1_dt;
		TH1I       * htunnel2_dt;
		TH1I       * htunnel3_dt;
		TH1I       * htunnel4_dt;
		TH2F       * htunnel1_E1E2;
		TH2F       * htunnel2_E1E2;
		TH2F       * htunnel3_E1E2;
		TH2F       * htunnel4_E1E2;
		//----------------------------
		// Tracker Histogram pointers
		//----------------------------
		TH1F       * hx;
		TH1F       * hy;
		TH1F       * hyinit;
		TH1F       * hxinit;
		TH1F       * hxa;
		TH1F       * hya;
		TH1F       * hmaxx;
		TH1F       * hmaxy;
		TH1F       * hmultx; 
		TH1F       * hmulty;
		TH1F       * hmaxx1d;
		TH1F       * hmaxx2d;
		TH1F       * hmaxx1g;
		TH1F       * hmaxx2g;
		TH1F       * hmaxy1d;
		TH1F       * hmaxy2d; 
		TH1F       * hmaxy1g;
		TH1F       * hmaxy2g;
		TH1F       * htmaxx; 
		TH1F       * htmaxy;
		TH1F       * himaxx; 
		TH1F       * himaxy;
		TH1F       * hbarx;
		TH1F       * hbary;
		TH1F       * hbarxm;	
		TH1F       * hbarym;	
		//TH1F     * hcoshxm;	
		//TH1F     * hcoshym;  
		TH2F       * hmult_xy;
		TH2F       * hbar2Dm;
		TH2F       * hbar2DmP;
		TH2F       * hbar2DmPRecoil;
		//TH2F     * hbarcosh2Dm;	
		TH2F       * hsum_xy;
		//------------------
		// Variables
		//-----------------
		ullint    ** evtCounter;//!
		double     * tunnel_rate_pad;//! 
		double     * tunnel_rate_board;//!
		ullint     * frameCounter;//!
		double     * dssd_rate_strip;//!
		double     * dssd_rate_board;//!
		ullint     * rate_counterPoint_dssd;//!
		ullint     * rate_counterPoint_tunnel;//!
		ullint       dssd_event_counter[17][16];
		double       dataSet_counter;
		uint         eventnumber;
		ullint       timestamp;
		ullint       old_timeStamp;
		ullint       prev_timestamp = 0;
		ullint       rate_counter_dssd;
		ullint       rate_counter_tunnel;
		llint        deltaT_rates;
		ushort       gain;
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
		double       Time;
		ullint       sed_tot_counter;
		ullint       dssd_tot_counter;	
		int          nCoboFrame;
		int          sed_counter1;
		int          dssd_counter1;
		int          prev_padNo;
		uint16_t     reaGenericEnergy;
		uint16_t     reaGenericTime;
		int          readMode =1; 
		bool belong_to_a_merge_frame;
		//ULong64_t    Timetracker; // testing stuff....			
		//ULong64_t    Timedssd;			
		DssdDataPoint   dPoint;
		TrackerNumexo2Event   sedNumexo2Event;
		TunnelDataPoint tPoint;
		TunnelDataPoint prev_tPoint;

		double x1 =0., y1 =0., z1 =0.;//point 1
		double x2 =0., y2 =0., z2 =0.;//point 2
		double x21 =0., y21 =0.,z21 =0.;//point2-point1
		double distance =0.;//cm
		double slopeXZ =0, slopeYZ =0.;
		double x=0., y=0.;
		bool reached_eof;
		llint jitter;
		//----------------------------------
		// Bufferes for making Correlations
		//----------------------------------
		std::vector<DssdDataPoint>dssdDataPointVec;// for pixel construction
		std::vector<DssdDataPoint>dssdDataPointVec_merged;

		std::vector<TrackerNumexo2Event>trackerNumexo2EventVec;
		std::vector<TrackerNumexo2Event>trackerNumexo2EventVec_merged;

		std::vector<TrackerCoBoEvent>trackerCoBoEventVec;
		std::vector<TrackerCoBoEvent>trackerCoBoEventVec_merged;

		std::vector<TunnelDataPoint>tunnelDataVec;
		std::vector<TunnelDataPoint>tunnelDataVec_merged;

		std::vector<DssdEvent>dssdEventVec;
		std::vector<DssdEvent>dssdEventVec_merged;
		std::vector<RecoilEvent> recoilTypeEvents;
		std::vector<DecayEvent> decayTypeEvents;
		//-------------------
		// Private Methods
		//-------------------
		void get_Count_Rates(int type);
		void FindCorrelations(std::string mode);
		void FindCorrelationsIn(std::vector<DssdDataPoint> &dssdDataPoints, std::vector<DssdEvent> &dssdEvents, std::vector<TrackerNumexo2Event> &trackerNumexo2Events, std::vector<TrackerCoBoEvent> &trackerCoboEvents);
		void CreateHistograms();
		void DeleteHistograms();
		void ResetHistograms();
		void PrintInfo(MFMCommonFrame *commonframe);

		void CreateAnalysisHistograms ();
	public:
		static GUser *g_instance;
		GUser(int mode , GDevice* _fDevIn= NULL, GDevice* _fDevOut= NULL) ;   // default constructor of GUser object
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
		virtual void UserMutantFrame(MFMCommonFrame * commonframe);
		virtual void UserEbyedatframeFrame(MFMCommonFrame * commonframe);
		void SaveUserTTree();
		void InitUserTTree(char *filename);
		void set_read_mode(int n){readMode =n;}
		ClassDef (GUser ,1); // User Treatment of Data

};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
