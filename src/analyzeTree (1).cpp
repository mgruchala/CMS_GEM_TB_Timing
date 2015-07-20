//
//  analyzeTree.cpp
//  
//
//  Created by Brian L Dorney on 02/04/15.
//
//

//C++ includes
#include <algorithm>
#include <stdio.h>

//My includes
#include "treeAnalyzer.h"
#include "treeAnalyzerTDC.h"

int main( int argc_, char * argv_[] ){
    treeAnalyzerTDC myTDCAnalyzer;

    std::vector<string> vec_strInputArgs;
    
    //Transfer Input Arguments into vec_strInputArgs
    //------------------------------------------------------
    vec_strInputArgs.resize(argc_);
    std::copy(argv_, argv_ + argc_, vec_strInputArgs.begin() );
    
    //Setup Output Filename
    //------------------------------------------------------
    string strOutputFileName = vec_strInputArgs[1];
    
    if (strOutputFileName.find(".txt") != string::npos ) {
        strOutputFileName.erase(strOutputFileName.find(".txt"), strOutputFileName.length() - strOutputFileName.find(".txt") );
        strOutputFileName = strOutputFileName + ".root";
    }
    else{
        strOutputFileName = "treeAnalyzerTDCOutput.root";
    }
    

    //Verbose Printing Options
    myTDCAnalyzer.setVerboseIO( false );
    myTDCAnalyzer.setSelection( vec_strInputArgs[1] );
    myTDCAnalyzer.applySelectionTDC( strOutputFileName );

    cout<<"root -l " << ( strOutputFileName ).c_str() << endl;

    return 0;
} //End main()
