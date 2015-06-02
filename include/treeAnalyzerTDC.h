//
//  treeAnalyzerTDC.h
//  
//
//  Created by Brian L Dorney on 30/03/15.
//
//

#ifndef ____treeAnalyzerTDC__
#define ____treeAnalyzerTDC__

//C++ Includes
#include <stdio.h>
#include <string>

//My Includes
//#include "TRunParameters.h"
#include "treeAnalyzer.h"

//ROOT Includes
#include "TBranch.h"
//#include "TEventList.h"
#include "TEntryList.h"
#include "TF1.h"
#include "TH1F.h"

using std::string;

using namespace ROOT;

class treeAnalyzerTDC : public treeAnalyzer {
    
public:
    //Constructor======================================
    treeAnalyzerTDC();
    
    //Actors===========================================
    //A set of plots is created for each master selection.  Each plot within this may have a local selection
    virtual void applySelectionTDC(string strOutputROOTFile);
    
    
};

#endif /* defined(____treeAnalyzerTDC__) */
