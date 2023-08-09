/*!
 * \file TunnelParameters.h
 * \author Rikel CHAKMA
 * \brief namespace for global variables and functions
 *
 *
 * */
#pragma once
#ifndef TUNNEL_PARAMETERS_h
#define TUNNEL_PARAMETERS_h 1
#include <map>
namespace TUNNEL_PARAMETERS{

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
