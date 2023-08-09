/*! \file TunnelData.cc
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

#include "TunnelData.h"
//! Constructor
/*! Get the instance of the myGlobal class. 
 * Initialize the variables.
 * Make the mapping of the Macro pixels with the physical pixel numbers. This is important for filling the 2D histogram of the hit patterns in the tunnel detectors to see where is position of the source with respect to the detector.
 */
TunnelData::TunnelData(){
	MapDetectorMacroPixels();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Destructor
/*! Clear memory
*/
TunnelData::~TunnelData(){
	fMacroPixelMap.clear();
}

//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get current tunnel macro pixel
TunnelMacroPixel TunnelData::GetMacroPixel(){
	std::pair<int,int> key = std::make_pair(fBoard, fChannel);
	return fMacroPixelMap[key];
}

TunnelMacroPixel TunnelData::GetMacroPixel(int * const p_board, int * const p_channel){
	std::pair<int,int> key = std::make_pair(*p_board, *p_channel);
	return fMacroPixelMap[key];
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get the Tunnel Detector Number
/*! This number is useful for targeting and filling the histograms. Here, one needs to be careful in specifying TUNNEL#_BOARD1 and TUNNEL#_BOARD2 in the Run.config file because each detector require 1 and 1/2 of a NUMEXO2 board. So, if detector 1 takes TUNNEL1_BOARD1 = NUMEXO_187 (from channel 0-15) and TUNNEL1_BOARD2 = half of NUMEXO_188 (from channel 0-7), for detector 2, TUNNEL2_BOARD2 = NUMEXO2_188 (from channel 8-15) and TUNNEL2_BOARD1 = NUMEXO2_189 (from channel 0-15).
 *
 * */
int TunnelData::GetTunnelDetectorNumber(int * const p_board, int * const p_channel){
	std::pair<int,int> key = std::make_pair(*p_board, *p_channel);
	return fMacroPixelMap[key].GetDetector();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get the Name of the macro pixel
/*! Depending on the TUNNEL BOARD1 and BOARD2 configurations in the Run.config file, get the name of current macro pixel.
*/
char TunnelData::GetMacroPixelName(int * const p_board, int * const p_channel){
	std::pair<int,int> key = std::make_pair(*p_board, *p_channel);
	return fMacroPixelMap[key].GetName();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Mapping of the macro Pixels with the physical sub pixels
/*! This function needs to be initialized inside the constructor.
*/
void TunnelData::MapDetectorMacroPixels(){
	TunnelMacroPixel macroPixel;
	char macroPixelsList[24] = {'S','N','W','Q','K','E','P','X','D','L','G','B','O','R','U','M','I','A','T','V','H','J','C','F'};
	std::vector<std::string> token = SplitString(s1->tunnelFormat,",");
	int detector, board, pixel, pixel1, pixel2, channel1, channel2;
	for(int i = 0; i< token.size(); i++){
		sscanf(token.at(i).c_str(), "%d:%d-%d:%d:%d-%d", &board, &channel1, &channel2,&detector, &pixel1, &pixel2);
		pixel = pixel1;
		for(int j = channel1; j<= channel2;j++){
			std::pair<int, int> key = std::make_pair(board,j);
			char mp = macroPixelsList[pixel];		
			switch (mp){
				case 'K': 
					macroPixel.SetName(mp);
					macroPixel.SetN(0);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 0; i <2; i++){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'L': 
					macroPixel.SetName(mp);
					macroPixel.SetN(1);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 2; i <4; i++){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'W': 
					macroPixel.SetName(mp);
					macroPixel.SetN(2);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 4; i <6; i++){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'X': 
					macroPixel.SetName(mp);
					macroPixel.SetN(3);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 6; i <8; i++){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'I': 
					macroPixel.SetName(mp);
					macroPixel.SetN(4);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 8; i <10; i++){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'J': 
					macroPixel.SetName(mp);
					macroPixel.SetN(5);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 10; i <12; i++){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'U': 
					macroPixel.SetName(mp);
					macroPixel.SetN(6);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 12; i <14; i++){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'V': 
					macroPixel.SetName(mp);
					macroPixel.SetN(7);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 14; i <16; i++){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'E': 
					macroPixel.SetName(mp);
					macroPixel.SetN(8);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 16; i <40; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'F': 
					macroPixel.SetName(mp);
					macroPixel.SetN(9);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 17; i <40; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'G': 
					macroPixel.SetName(mp);
					macroPixel.SetN(10);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 18; i <40; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'H': 
					macroPixel.SetName(mp);
					macroPixel.SetN(11);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 19; i <40; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'Q': 
					macroPixel.SetName(mp);
					macroPixel.SetN(12);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 20; i <40; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'R': 
					macroPixel.SetName(mp);
					macroPixel.SetN(13);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 21; i <40; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'S': 
					macroPixel.SetName(mp);
					macroPixel.SetN(14);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 22; i <40; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'T': 
					macroPixel.SetName(mp);
					macroPixel.SetN(15);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 23; i <40; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'A': 
					macroPixel.SetName(mp);
					macroPixel.SetN(16);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 40; i <64; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'B': 
					macroPixel.SetName(mp);
					macroPixel.SetN(17);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 41; i <64; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'C': 
					macroPixel.SetName(mp);
					macroPixel.SetN(18);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 42; i <64; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'D': 
					macroPixel.SetName(mp);
					macroPixel.SetN(19);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 43; i <64; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'M': 
					macroPixel.SetName(mp);
					macroPixel.SetN(20);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 44; i <64; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'N': 
					macroPixel.SetName(mp);
					macroPixel.SetN(21);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 45; i <64; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'O': 
					macroPixel.SetName(mp);
					macroPixel.SetN(22);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 46; i <64; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
				case 'P': 
					macroPixel.SetName(mp);
					macroPixel.SetN(23);
					macroPixel.SetDetector(detector);
					macroPixel.SetBoard(board);
					macroPixel.SetChannel(j);
					macroPixel.pixels.clear();
					for(int i = 47; i <64; i=i+8){
						TunnelPixel p(i);
						macroPixel.pixels.push_back(p);
					}
					break;
			}

			fMacroPixelMap[key] = macroPixel;
			pixel++;
		}
	}
	token.clear();
	s1->tunnelFormat.clear();
	macroPixel.pixels.clear();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Get the Macro Pixel number
/*! First, it gets the Macro pixel name, then it gets the macroPixel using this name. The macro pixel number is extracted using TunnelMacroPixel:.GetN().
 *
 */
int TunnelData::GetMacroPixelPhysicalNumber(int * const p_board, int * const p_channel){
	std::pair<int,int> key = std::make_pair(*p_board, *p_channel);
	return  fMacroPixelMap[key].GetN();
}
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
//! Tunnel Board number
int TunnelData::GetTunnelBoardNumber(int * const p_board){
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
