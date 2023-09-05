/*!
 * \file Global.h
 * \author Rikel CHAKMA
 * \brief A singleton class for global variables
 * \details The instance of this class is being invoked within the UserLib library and the GUser class. The values of the variables declared in this class are generally defined in the Run.config file. 
 *
 *
 * */

#ifndef MyGlobal_h
#define MyGlobal_h 1
#include <iostream>
#include <string>
#include <fstream>
#include <locale>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include "UTypes.h"
#define timestamp_unit 10 //ns
#define NCHANNELS 16
#define sampling_period 5 //ns
#define Resistance 700. //All the ASIC resistors are 700 k Ohms
#define RESET   "\033[1;0m"
#define BLACK   "\033[1;30m"
#define RED     "\033[1;31m" 
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
using namespace std;
class MyGlobalDestroyer;
class MyGlobal
{
	public:
		static MyGlobal *GetInstance(); /*!< A static instance of the class. It is called by MyGlobal* s1 = MyGlobal::GetInstance();*/
		static MyGlobal *GetInstance(int mode); /*!< A static instance of the class. It is called by MyGlobal* s1 = MyGlobal::GetInstance();*/
		//some general variables
		std::string acquisitionMode;
		bool fsaveTree;
		bool fsaveHisto;
		int fverbose; /*!< verbose level for printing information.*/
		bool data_merged;/*!< Data merging flag. If MFMMerger library is used to merge different data frames, it is set as True else, it is false by default. When Merger is used the DSSD pixels and correlations between the events are made within the Guser::UserMergeFrame(MFMCommonFrame * commonframe) else the data are temporarily stored in a buffer of size buffer_size to make correlations. */
		uint buffer_size;/*!< size of the buffer when MFMMerger is not used. */

		//DSSD related
		int pre_trig_buffer;/*!< size of the pretrig buffer useful for finding the trigger. */
		int nStart_trace;/*!< The number of samples to be ignored in the begining of the trace.*/
		int nEnd_trace;/*!< The number of samples to be ignored at the end of the trace. */
		int TRACE_SIZE;/*!< The size of the trace used for actual treatment of the signal. Note nStart_trace + nEnd_trace + TRACE_SIZE = 992 samples. */

		//back
		std::string BACK_FPCSA_GAIN_MODE;/*!< DSSD Back side FPCSA gain mode (lg: Low Gain, hg: High Gain, auto: Auto Gain)*/
		double backHighGain;/*!< High gain value in Capacitance (pF) */
		double backLowGain;/*!< Low gain value in Capacitance (pF) */
		//front
		std::string FRONT_FPCSA_GAIN_MODE; /*!< DSSD Front side FPCSA gain mode.*/
		double frontHighGain;/*!< High gain value in Capacitance (pF)*/
		double frontLowGain;/*!< Low Gain value in Capacitance (pF)*/
		//gain switching
		ushort mv_window_calcul_gainSwitch;/*!< moving window size in samples to determine the gain switching */
		ushort front_gainSwitch_threshold; /*!< Threshold level for Front side gain switch */
		ushort back_gainSwitch_threshold;/*!< Threshold level for the Back side gain switch */

		bool use_default_FPCSA_Gain; /*!< Flag whether to use the default FPCSA gain in the Filtering algorithms. */
		// HG decay constant calculation
		ushort mv_window_calcul_HG;/*!< Size of the moving window in samples to calculate the High gain */
		ushort skip_nSamples_calcul_HG;/*!< Number of samples to be ignored to calculate the high gain*/
		//LG decay constant calculatin
		ushort mv_window_calcul_LG;/*!< Size of the moving window to calculate the low gain.*/
		ushort skip_nSamples_calcul_LG;/*!< Number of sampples to be ignored in calculating th elow gain. */

		//signal treatment
		ushort decayTime_threshold; /*!< The minimum decay time to be considered as a signal. If the calculated decay time falls below this threshold the signal is treated as noisy signal.*/
		ushort saturationTime_threshold;/*!< The maximum decay time to be considered as a signal. If the calculated decay time is above this threhold the signal is treated as a saturated signal. */
		//definitions
		ushort riseTime_def;/*!< The definition of risetime. If riseTime_def = 1, then, rise time is defined as the time taken for the signal to reach from 10 % to  90 % of the signal height. If riseTime_def = 2, then, it is defined as the time taken to reach from the Trigger to the Maximum*/
		ushort trigger_def; /*!< The defintion of trigger. If trigger_def = 1, the trigger is measured at the position of the signal when its hight crosses 3*Noise with respect to the baseline.  */

		// Trapezoidal
		std::string filterAlgorithm; /*!< Choice of the filter algorithm to be used in processing the DSSD traces. @see digitalFilters class. */
		bool use_default_filter_parameters;/*!< Flag for the use of the default filter parametrs defined in the Run.config file. */
		int default_trap_k;/*!< value of the default trapezoidal parameter k. */
		int default_trap_m;/*!< value of the default trapezoidal parameter m*/
		double shaperAmplificationGain;/*!< The gain og the shaper amplifier used in trapezoidal algorithm 3 and 4. */
		int trap_flattop_mode;/*!< Measurement method of the trapezoidal height. If trap_flattop_mode = 1, the trapezoidal height is the average of points taken on both sides of the max position until frac*max height. If trap_flattop_mode = 2, then, trapezoidal height will be averaged from  Trigger + k to Trigger + l. If trap_flattop_mode = 3, then the trapezoidal height will be averaged from  Trigger + k to Trigger + l (with risetime correction option). If trap_flattop_mode = 4, the flat top part of the trapezoidal will be automatically detected but this iscomutationally costly.
		*/
		double height_fraction_MaxTrapHeight;/*!< If trap_flattop_mode =1, the fraction of the maximum trapezoidal height used for calculating the flat top average. */
		//optimized k and m parameter file
		std::string trap_parameter_filename;/*!< The name of the optimized trapezoidal parameter file. */
		int CFDMethod;/*!< CFD method used in calculating time*/
		double dssdCFDFraction;/*!< CFD Fraction used */
		int dssdCFDDelay;/*!< CFD delay used*/
		double sedCFDFraction;/*!< CFD Fraction used */
		int sedCFDDelay;/*!< CFD delay used*/
		bool use_default_CFD_parameters;
		std::string cfd_parameter_filename_dssd;/*!< The name of the optimized cfd_parameter file.*/
		std::string cfd_parameter_filename_sed;/*!< The name of the optimized cfd_parameter file.*/
		//DSSD board list	
		int NBOARDS_DSSD; /*!< The number of active DSSD boards. */
		int NSTRIPS_DSSD;/*!< The number of active DSSD strips.*/
		std::string fTreeType;
		bool fReverseStrip;
		std::string dssdFormat;		
		std::string trackerFormat;		
		std::string tunnelFormat;		
		//Calibration files
		std::string dssd_HGcalib_filename;/*!< Name of the DSSD high gain calibration file. */
		std::string dssd_LGcalib_filename;/*!< Name of the DSSD low gain calibration file.*/
		std::string timestampAlignment_filename;/*!< Name of the time shift file.*/
		std::string cfd_timeAlignment_filename;/*!< Name of the time shift file.*/
		//Anlysis related
		//sum energies of neighboring strips
		bool sum_neighboring_strips; /*!< To sum the energies of the neighboring DSSD strips or not. This is to sum the enries of the charge sharing strips. */
		int ff_window; /*!< Coincidence window between two front strips for making a DSSD pixel. */
		int fb_window; /*!< Coincidence window between a front strip and a back strip for making a DSSD pixel. */
		int bf_window; /*!< Coincidence window between a back strip and a front strip for making a DSSD pixel. */
		int bb_window; /*!< Coincidence window between two back strips for making a DSSD pixel.  */
		int rate_calcul_time_lapse;/*!< The time lapse for calculating the rate of events*/ //in sec		
		int front_gs_duration, back_gs_duration;

		//-----------------------------
		//Tunel related
		//-----------------------------
		int NDETECTOR_TUNNEL;/*!< The number of active tunnel detectors. */ 
		int NBOARDS_TUNNEL; /*!< The number of active tunnel boards.*/
		int *boardIndex_DSSD;/*!< List of indices of the tunnel boards */
		int *boardIndex_Tunnel;/*!< List of indices of the tunnel boards */
		std::string tunnel_calib_filename;/*!< Tunnel calibration filename */


		//Tracker
		//
		int trackerNumexoBoard;
		int trackerNumexoChannel;
		int trackerNumexoStrip;

		// Correlations toF DSSD vs CoBo
		llint ToF_lower_cobo;
		llint ToF_upper_cobo;

		llint ToF_lower_numexo2;
		llint ToF_upper_numexo2;
		//Exogam related
		std::string gamma_calib_filename;/*!< Ge detector calibration filename*/

		string frameName[9];/*!< List of the frame names "Cobo", "Eby", "Hello", "xml header", "rea trace", "rea gen", "sirius", merge, "else". */

		int NofMacroPixels;
		std::vector<int> boardList_DSSD;
		std::vector<int> nChannel_DSSD;
		std::vector<int> boardList_Tunnel;
		void ReadRunConfigFile();
		bool from_dssd(int &board);
		bool from_tunnel(int &board);
		void PressEnterToContinue();

	private:
		int readMode;
		static MyGlobal *instance;
		static MyGlobalDestroyer destroyer;
	protected:
		MyGlobal();
		MyGlobal(int mode);
		virtual ~MyGlobal();
		friend class MyGlobalDestroyer;

};
//global static functions
//called by MyGlobal* s1 = MyGlobal::GetInstance();
//-----------------------
// class to delete static instance of MyGlobal
//---------------------

std::vector<std::string> SplitString(std::string str, std::string delimiter);


class MyGlobalDestroyer
{
	public:
		MyGlobalDestroyer(){};
		MyGlobalDestroyer(MyGlobal *g ){_g =g;}
		~MyGlobalDestroyer(){delete _g; _g=NULL;}
		void SetMyGlobal(MyGlobal *g){_g = g;}
	private:
		MyGlobal *_g;
};
/***************************************************************

  DSSD Routing
  --------------


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


  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

  Tunnel Detector Routing
  -------------------------

  Macro Pixel No   Designation
  1-4             SNWQ
  5-8             KEPX
  9-12            DLGB
  13-16            ORUM
  17-20            IATV
  21-24            HJCF



  -----------------------------------
  |                                 |
  |    K       L       W       X    |
  |                                 |
  |    I       J       U       V    |
  |                                 |
  |                                 |
  |                                 |
  |                                 |
  |  E   F   G   H   Q   R   S   T  |
  |                                 |
  |                                 |
  |                                 |
  |                                 |
  |                                 |
  |                                 |
  |  A   B   C   D   M   N   O   P  |
  |                                 |
  |                                 |
  |                                 |
  -----------------------------------
  ===================================
  ========|||||========|||||=========
  ========|||||========|||||=========
  connector FTMH-120-03-L-DV (MALE)


 ***************************************************************************/
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
