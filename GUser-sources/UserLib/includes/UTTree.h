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
#include "TrackerData.h"
#include "Global.h"
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
		std::string fTreeFileName;

	public:
		UTTree();
		~UTTree();	
		void Initialize(char* filename, DssdData *dssd, TunnelData *tunnel, TrackerData *tracker);
		void FillSiriusFrames();
		void FillReaGenericFrames();
		void FillCoBoFrames();
		void Save();
};
#endif
//---------------ooooooooooooooo---------------ooooooooooooooo---------------ooooooooooooooo---------------
