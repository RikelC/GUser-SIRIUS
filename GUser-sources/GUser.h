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
#include "TF1.h"
#include "TGraph.h"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
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
#include "global.h"
#include "dssdData.h"
#include "tunnelData.h"
#include "digitalFilters.h"
#include "digitalCFD.h"
#include "calibration.h"
#include "dssdDataPoint.h"
#include "tunnelDataPoint.h"
#include "dssdEvent.h"
#include "tunnelEvent.h"

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

#ifndef NB_COBO
#define NB_COBO 1
#endif

#ifndef NB_ASAD
#define NB_ASAD 1
#endif

#ifndef NB_AGET
#define NB_AGET 4
#endif

#ifndef NB_CHANNEL
#define NB_CHANNEL 68
#endif

#ifndef NB_SAMPLES
#define NB_SAMPLES 256
#endif

#ifndef BLCALC
#define BLCALC 50
#endif

#ifndef NSIG
#define NSIG 4.
#endif

#ifndef DSSD_DETECTOR
#define DSSD_DETECTOR 1
#endif

#ifndef TUNNEL_DETECTOR
#define TUNNEL_DETECTOR 2
#endif

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
		MFMCommonFrame * frame;
		MFMMutantFrame* fMutantframe;
		MFMEbyedatFrame* fEbyedatframe;
	private:
		//---------User Lib---------
		myGlobal                  * s1;/**< pointer to the instance of the Singleton myGlobal class. */
		digitalFilters            * filter;/**< instance for the digital filter algorithms. */
		digitalCFD                * cfd; /**< instance for the digital constant fraction discriminator. */
		calibration               * calib; /**< instance for doing calibration of the detectors. */
		dssdData                  * dData; /*!< dssdData object*/
		dssdEvent                 * dEvent;/*!< dssdEvent object*/
		//dssdEvent                 * trEvent;
		tunnelEvent               * tEvent;/*!<tunnelEvent object*/
		tunnelData                * tData;/*!< tunnelData object*/
		//long long int fBoardHitPattern[NBOARDS];
		//---------DSSD Histograms----------
		//TH1I   *** hGain ; //!
		//TH1I   *** hFeedBack; //!
		TH1I     * htunnel1_dt;//!
		TH1I     * htunnel2_dt;//!
		TH1I     * htunnel3_dt;//!
		TH1I     * htunnel4_dt;//!
		TH2F       * htunnel1_E1E2; //!
		TH2F       * htunnel2_E1E2; //!
		TH2F       * htunnel3_E1E2; //!
		TH2F       * htunnel4_E1E2; //!
		TH1F     *** hTrigger;//!
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
		TH2F       * h_dssdE_Tof, * h_dssdE_Tof2;
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
		TGraph *grUnsortedT;
		TGraph *grT;
		TGraph *grDT;
		TH2F* h_front_traceGS;
		TH2F* h_back_traceGS;
		TH2F* h_front_traceNGS;
		TH2F* h_back_traceNGS;
		TH1F* h_front_rNGS;
		TH1F* h_back_rNGS;
		TH1F* h_front_rGS;
		TH1F* h_back_rGS;
		TH1F* h_tac;
		TH1F *h_sum_front, *h_sum_back;
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
		//ULong64_t    Timetracker; // testing stuff....			
		//ULong64_t    Timedssd;			
		dssdDataPoint dPoint;
		tunnelDataPoint tPoint;
		tunnelDataPoint prev_tPoint;
		std::vector<dssdDataPoint>dssdDataVec;
		std::vector<dssdDataPoint>newDssdDataVec;
		std::vector<dssdDataPoint>dssdDataVec2;
		std::vector<dssdDataPoint>trackerNumexoDataVec;
		std::vector<tunnelDataPoint>tunnelDataVec;
		std::vector<dssdPixel>dssdEventVec;
		//std::vector<dssdPixel>trackerEventVec;
		int nCoboFrame=0;

		int sed_counter1 =0;
		int dssd_counter1 =0;




		///////////////////////////////////
		// Analysis parameters: decoding //
		///////////////////////////////////
		unsigned int buckidx;
		unsigned int chanidx;
		unsigned int agetidx;
		unsigned int asadidx;
		unsigned int coboidx;
		unsigned int nbitems;
		unsigned int sample;

		int lastCellRead[NB_COBO][NB_ASAD][NB_AGET];
		int MultCoBo[NB_COBO]; 
		///////////////////////////////////
		// Analysis parameters: treating //
		///////////////////////////////////
		//float baseline[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		//float sigbsline[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		float amplitude[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int tmax[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		//float tstart[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		//int rt[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		//int tt[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int naget[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		int nv[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL];
		float TRACE[NB_COBO][NB_ASAD][NB_AGET][NB_CHANNEL][NB_SAMPLES];

		///////////////////////////////////////
		// Analysis parameters: storage Tree //
		///////////////////////////////////////
		int nvoie;



		TGraph* Sampling[NB_COBO][NB_ASAD][NB_AGET][NB_CHANNEL];



		/////////////////////////
		// Function prototypes //
		/////////////////////////

		void TreatPulseGet();


		void get_Count_Rates( int &board, int &stripNumber, ullint &time, int type);
		int prev_padNo;
		int ny;
		int nx;

		int cut;
		int npb;
		float threshold;

		float barxm;
		float barym;
		//float coshxm;
		//float coshym;
		//double p1x;
		//double p1y;


		int *chan;//!
		int *ind;//!

		float *ampliy;//!
		float *amplix;//!

		float *startx;//!
		float *starty;//!

		TF1 *fitx;
		TF1 *fity;


		//fichier calib Get

		float *coefx;//! 
		float *coefy;//!
		//lecture lookuptable 
		int *aget0; //!
		int *aget1; //!
		int *aget2; //!
		int *aget3;//!
		int aget;
		int nc;


		TH1F *hx;
		TH1F *hy;
		TH1F *hyinit;
		TH1F *hxinit;
		TH1F *hxa;
		TH1F *hya;

		TH1F *hmaxx;
		TH1F *hmaxy;
		TH1F *hmultx; 
		TH1F *hmulty;
		TH2F *hmult_xy;

		TH1F *hmaxx1d;
		TH1F *hmaxx2d;
		TH1F *hmaxx1g;
		TH1F *hmaxx2g;

		TH1F *hmaxy1d;
		TH1F *hmaxy2d; 
		TH1F *hmaxy1g;
		TH1F *hmaxy2g;

		TH1F *htmaxx; 
		TH1F *htmaxy;
		TH1F *himaxx; 
		TH1F *himaxy;

		TH1F *hbarx;
		TH1F *hbary;
		TH1F *hbarxm;	
		TH1F *hbarym;	
		//TH1F *hcoshxm;	
		//TH1F *hcoshym;  

		TH2F *hbar2Dm;
		//TH2F *hbarcosh2Dm;	
		TH2F *hsum_xy;



		TFile* treeFile;
		TTree *siriusTTree;
		TTree *reaGenericTTree;
		TTree *coboTTree;
		uint16_t reaGenericEnergy, reaGenericTime;
		int readMode =1;
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
