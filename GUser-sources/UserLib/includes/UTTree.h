/*! \file UTTree.h
 *  \author Rikel CHAKMA
 *  \brief 
 *  \details 
 */
#pragma once

#ifndef UTTree_h
#define UTTree_h 1
#include "TFile.h"
#include "TTree.h"
#include "DssdData.h"
#include "TunnelData.h"
#include "TrackerCoBoData.h"
#include "Global.h"
#include "RecoilEvent.h"
#include "DecayEvent.h"
#include <string>
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
class UTTree
{
	private:
		MyGlobal *s1; /*!< pointer to the instance of the myGlobal singleton class. */ 
	protected:
		TFile * fTreeFile;
		TTree * fSiriusTTree;
		TTree * fReaGenericTTree;
		TTree * fCoBoTTree;
		TTree * fRecoilTTree;
		TTree * fDecayTTree;
		std::string fTreeFileName;

	public:
		UTTree();
		~UTTree();	
		void Initialize(char* filename, DssdData *dssd, TunnelData *tunnel, TrackerCoBoData *tracker);
		void Initialize(char *filename, RecoilEvent & recoil, DecayEvent & decay);
		void FillSiriusFrames();
		void FillReaGenericFrames();
		void FillCoBoFrames();
		void FillRecoilTTree();
		void FillDecayTTree();
		void Save();
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
