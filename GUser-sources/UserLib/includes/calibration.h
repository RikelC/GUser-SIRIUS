/*! \file calibration.h
 *  \author Rikel CHAKMA
 *  \brief
 *  \details
 *
 */
#ifndef calibration_h
#define calibration_h 1
#include "global.h"
#include "dssdData.h"
#include "tunnelData.h"
#include "geData.h"
#include "trackerData.h"
#include <fstream>
#include <iostream>
#include  "TRandom3.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
/*!
 * Calibration class 
 *
 */
class calibration
{
	private:
		myGlobal* s1;/*!< A pointer to the instance of the myGlobal class. */
		double **calib_m_hg; /*!< 2D array for the high gain calibration parameter m (gain) for the DSSD. */ //!
		double **calib_c_hg; /*!< 2D array for the high gain calibration parameter c (offset) for the DSSD. */ //!
		double **calib_m_lg; /*!< 2D array for the low gain calibration parameter m (gain) for the DSSD. */ //!
		double **calib_c_lg; /*!< 2D array for the low gain calibration parameter c (offset) for the DSSD. *///!
		double **calib_m_t; /*!< 2D array for the calibration parameter m (offset) for the Tunnel detectors. */ //!
		double **calib_c_t; /*!< 2D array for the calibration parameter c (offset) for the Tunnel detectors. *///!
		TRandom3 * rand;/*!< Pointer to a random number generator. */
	public:
		calibration();
		~calibration();
		void assign_dssd_calibration_parameters();
		void assign_tunnel_calibration_parameters();
		void assign_ge_calibration_parameters();
		double perform(dssdData* const);
		double perform(tunnelData* const);
		double perform(geData* const);
//		double perform(trackerData* const);
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
