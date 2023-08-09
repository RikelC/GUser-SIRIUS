#include "TrackerNumexo2Parameters.h"

namespace TRACKER_NUMEXO2_PARAMETERS{

	std::map<int, int> NChannelOfBoard;
	std::string filterAlgorithm;
	bool use_default_filter_parameters;
	int default_trap_k;
	int default_trap_m;
	double shaperAmplificationGain;
	int trap_flattop_mode;
	double height_fraction_MaxTrapHeight;
	std::string trap_parameter_filename;
	int CFDMethod;
	double CFDFraction;
	int CFDDelay;
	bool use_default_CFD_parameters;
	std::string cfd_parameter_filename;

}


