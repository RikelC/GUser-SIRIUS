/*! \file tunnelData.cc
 *  \author Rikel CHAKMA
 *  \brief Tretment of tunnel data
 *  \details 
 *  \f{tabular}{{ccc}
 MacroPixel No  & Designation  & Physical MacroPixel No\\
 *  1-4          &   SNWQ & \\
 *  5-8          &   KEPX & \\
 *  9-12         &   DLGB & \\
 *  13-16        &   ORUM & \\
 *  17-20        &   IATV & \\
 *  21-24        &   HJCF &\\ \\
 * &    Tunnel Detector Map & \\
 *  \f}
 *  
 *  \image html Tunnel_config.png
 *  \image latex Tunnel_config.png "Tunnel Configuration" width=10cm
 */

#include "tunnelData.h"
//! Constructor
/*! Get the instance of the myGlobal class. 
 * Initialize the variables.
 * Make the mapping of the Macro pixels with the physical pixel numbers. This is important for filling the 2D histogram of the hit patterns in the tunnel detectors to see where is position of the source with respect to the detector.
 */
tunnelData::tunnelData(){
	map_detectorMacroPixels();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
/*! Clear memory
*/
tunnelData::~tunnelData(){
	macroPixelMap.clear();
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get current tunnel macro pixel
tunnelMacroPixel tunnelData::get_macroPixel(int * const p_board, int * const p_channel){
	std::pair<int,int> key = std::make_pair(*p_board, *p_channel);
	return macroPixelMap[key];
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get the Tunnel Detector Number
/*! This number is useful for targeting and filling the histograms. Here, one needs to be careful in specifying TUNNEL#_BOARD1 and TUNNEL#_BOARD2 in the Run.config file because each detector require 1 and 1/2 of a NUMEXO2 board. So, if detector 1 takes TUNNEL1_BOARD1 = NUMEXO_187 (from channel 0-15) and TUNNEL1_BOARD2 = half of NUMEXO_188 (from channel 0-7), for detector 2, TUNNEL2_BOARD2 = NUMEXO2_188 (from channel 8-15) and TUNNEL2_BOARD1 = NUMEXO2_189 (from channel 0-15).
 *
 * */
int tunnelData::get_tunnelDetectorNumber(int * const p_board, int * const p_channel){
	std::pair<int,int> key = std::make_pair(*p_board, *p_channel);
	return macroPixelMap[key].get_detector();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get the Name of the macro pixel
/*! Depending on the TUNNEL BOARD1 and BOARD2 configurations in the Run.config file, get the name of current macro pixel.
*/
char tunnelData::get_macroPixelName(int * const p_board, int * const p_channel){
	std::pair<int,int> key = std::make_pair(*p_board, *p_channel);
	return macroPixelMap[key].get_name();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Mapping of the macro Pixels with the physical sub pixels
/*! This function needs to be initialized inside the constructor.
*/
void tunnelData::map_detectorMacroPixels(){
	tunnelMacroPixel macroPixel;
	char macroPixelsList[24] = {'S','N','W','Q','K','E','P','X','D','L','G','B','O','R','U','M','I','A','T','V','H','J','C','F'};
	std::vector<std::string> token = split_string(s1->tunnelFormat,",");
	int detector, board, pixel, pixel1, pixel2, channel1, channel2;
	for(int i = 0; i< token.size(); i++){
		sscanf(token.at(i).c_str(), "%d:%d-%d:%d:%d-%d", &board, &channel1, &channel2,&detector, &pixel1, &pixel2);
		pixel = pixel1;
		for(int j = channel1; j<= channel2;j++){
			std::pair<int, int> key = std::make_pair(board,j);
			char mp = macroPixelsList[pixel];		
			switch (mp){
				case 'K': 
					macroPixel.set_name(mp);
					macroPixel.set_N(0);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 0; i <2; i++){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'L': 
					macroPixel.set_name(mp);
					macroPixel.set_N(1);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 2; i <4; i++){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'W': 
					macroPixel.set_name(mp);
					macroPixel.set_N(2);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 4; i <6; i++){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'X': 
					macroPixel.set_name(mp);
					macroPixel.set_N(3);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 6; i <8; i++){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'I': 
					macroPixel.set_name(mp);
					macroPixel.set_N(4);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 8; i <10; i++){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'J': 
					macroPixel.set_name(mp);
					macroPixel.set_N(5);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 10; i <12; i++){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'U': 
					macroPixel.set_name(mp);
					macroPixel.set_N(6);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 12; i <14; i++){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'V': 
					macroPixel.set_name(mp);
					macroPixel.set_N(7);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 14; i <16; i++){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'E': 
					macroPixel.set_name(mp);
					macroPixel.set_N(8);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 16; i <40; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'F': 
					macroPixel.set_name(mp);
					macroPixel.set_N(9);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 17; i <40; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'G': 
					macroPixel.set_name(mp);
					macroPixel.set_N(10);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 18; i <40; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'H': 
					macroPixel.set_name(mp);
					macroPixel.set_N(11);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 19; i <40; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'Q': 
					macroPixel.set_name(mp);
					macroPixel.set_N(12);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 20; i <40; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'R': 
					macroPixel.set_name(mp);
					macroPixel.set_N(13);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 21; i <40; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'S': 
					macroPixel.set_name(mp);
					macroPixel.set_N(14);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 22; i <40; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'T': 
					macroPixel.set_name(mp);
					macroPixel.set_N(15);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 23; i <40; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'A': 
					macroPixel.set_name(mp);
					macroPixel.set_N(16);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 40; i <64; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'B': 
					macroPixel.set_name(mp);
					macroPixel.set_N(17);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 41; i <64; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'C': 
					macroPixel.set_name(mp);
					macroPixel.set_N(18);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 42; i <64; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'D': 
					macroPixel.set_name(mp);
					macroPixel.set_N(19);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 43; i <64; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'M': 
					macroPixel.set_name(mp);
					macroPixel.set_N(20);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 44; i <64; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'N': 
					macroPixel.set_name(mp);
					macroPixel.set_N(21);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 45; i <64; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'O': 
					macroPixel.set_name(mp);
					macroPixel.set_N(22);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 46; i <64; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'P': 
					macroPixel.set_name(mp);
					macroPixel.set_N(23);
					macroPixel.set_detector(detector);
					macroPixel.pixels.clear();
					for(int i = 47; i <64; i=i+8){
						tunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
			}

			macroPixelMap[key] = macroPixel;
			pixel++;
		}
	}
	token.clear();
	s1->tunnelFormat.clear();
	macroPixel.pixels.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get the Macro Pixel number
/*! First, it gets the Macro pixel name, then it gets the macroPixel using this name. The macro pixel number is extracted using tunnelMacroPixel::get_N().
 *
 */
int tunnelData::get_macroPixelPhysicalNumber(int * const p_board, int * const p_channel){
	std::pair<int,int> key = std::make_pair(*p_board, *p_channel);
	return  macroPixelMap[key].get_N();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Tunnel Board number
int tunnelData::get_tunnelBoardNumber(int * const p_board){
	return s1->boardIndex_Tunnel[*p_board];
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
/*********************************************************************

  Tunnel Detector Mapping
  -------------------------

  Macro Pixel No   Designation
  1-4             SNWQ
  5-8             KEPX
  9-12            DLGB
  13-16           ORUM
  17-20           IATV
  21-24           HJCF



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


 *********************************************************/
