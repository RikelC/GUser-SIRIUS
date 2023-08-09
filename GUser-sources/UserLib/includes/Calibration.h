/*! \file Calibration.h
 *  \author Rikel CHAKMA
 *  \brief
 *  \details
 *
 */
#pragma once

#ifndef Calibration_h
#define Calibration_h 1
#include "Global.h"
#include "DssdData.h"
#include "TunnelData.h"
#include "GeData.h"
#include <fstream>
#include <iostream>
#include  "TRandom3.h"
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
/*!
 * Calibration class 
 *
 */
class Calibration
{
	private:
		MyGlobal* s1;/*!< A pointer to the instance of the myGlobal class. */
		double **fGainHG; /*!< 2D array for the high gain Calibration parameter m (gain) for the DSSD. */ //!
		double **fOffsetHG; /*!< 2D array for the high gain Calibration parameter c (offset) for the DSSD. */ //!
		double **fGainLG; /*!< 2D array for the low gain Calibration parameter m (gain) for the DSSD. */ //!
		double **fOffsetLG; /*!< 2D array for the low gain Calibration parameter c (offset) for the DSSD. *///!
		double **fGainTunnel; /*!< 2D array for the Calibration parameter m (offset) for the Tunnel detectors. */ //!
		double **fOffsetTunnel; /*!< 2D array for the Calibration parameter c (offset) for the Tunnel detectors. *///!
		TRandom3 * rand;/*!< Pointer to a random number generator. */
	public:
		Calibration();
		~Calibration();
		void AssignDssdCalibrationParameters();
		void AssignTunnelCalibrationParameters();
		void AssignGeCalibrationParameters();
		double Perform(DssdData* const);
		double Perform(TunnelData* const);
		double Perform(GeData* const);
		//		double Perform(TrackerData* const);
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
