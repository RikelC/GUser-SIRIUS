#include "DssdParameters.h"

namespace DSSD_PARAMETERS{

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
	bool use_default_CFD_parameters;
	std::string cfd_parameter_filename_dssd;/*!< The name of the optimized cfd_parameter file.*/
	std::string cfd_parameter_filename_sed;/*!< The name of the optimized cfd_parameter file.*/
	//DSSD board list	
	int NBOARDS_DSSD; /*!< The number of active DSSD boards. */
	int NSTRIPS_DSSD;/*!< The number of active DSSD strips.*/
}


