/*!
 * \file TrackerCoBoParameters.h
 * \author Rikel CHAKMA
 * \brief namespace for global variables and functions
 *
 *
 * */
#pragma once
#ifndef TRACKER_COBO_PARAMETERS_h
#define TRACKER_COBO_PARAMETERS_h 1
#include <map>
namespace TRACKER_COBO_PARAMETERS{

extern	std::map<int, int> NChannelOfBoard;
extern	std::string filterAlgorithm;
extern	bool use_default_filter_parameters;
extern	int default_trap_k;
extern	int default_trap_m;
extern	double shaperAmplificationGain;
extern	int trap_flattop_mode;
extern	double height_fraction_MaxTrapHeight;
extern	std::string trap_parameter_filename;
extern	int CFDMethod;
extern	double CFDFraction;
extern	int CFDDelay;
extern	bool use_default_CFD_parameters;
extern	std::string cfd_parameter_filename;

}


#endif
