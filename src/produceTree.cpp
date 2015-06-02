//
//  produceTDCTTree.C
//  
//
//  Created by Brian L Dorney on 29/01/15.
//
//

//C++ Includes
#include <stdio.h>
#include <string>

//ROOT Includes

//My Includes
#include "treeProducerTDC.h"

int main(){
    //Variable Declaration
    int iDetRequest = 0;

    std::string treeName_GE11_IV = "GE11_IV";
    std::string outputData_GE11_IV = "TestBeamDataset_TDC_Fall2014_GE11_IV.root";
    std::string inputData_GE11_IV = "/afs/cern.ch/user/d/dorney/scratch0/CMS_GEM/RunList_2014_GE11IV.txt";

    std::string treeName_GE11_IV_GIF = "GE11_IV_GIF";
    std::string outputData_GE11_IV_GIF = "TestBeamDataset_TDC_Fall2014_GE11_IV_GIF.root";
    std::string inputData_GE11_IV_GIF = "/afs/cern.ch/user/d/dorney/scratch0/CMS_GEM/RunList_2014_GE11IVGIF.txt";

    std::string treeName_GE11_V = "GE11_V";
    std::string outputData_GE11_V = "TestBeamDataset_TDC_Fall2014_H2_GE11_V.root";
    std::string inputData_GE11_V = "/afs/cern.ch/user/d/dorney/scratch0/CMS_GEM/RunList_H2_GE11V.txt";

    treeProducerTDC myProducer;

    //Ignored Parameter List (All)
    myProducer.setIgnoredParameter("TDC.ROOT");
    myProducer.setIgnoredParameter("Trial01");
    myProducer.setIgnoredParameter("Lat24");
    myProducer.setIgnoredParameter("Lat26");
    myProducer.setIgnoredParameter("Lat27");
    myProducer.setIgnoredParameter("Lat30");
    myProducer.setIgnoredParameter("Lat31");
    myProducer.setIgnoredParameter("Ch10");
    myProducer.setIgnoredParameter("Ch10GE2.root");
    myProducer.setIgnoredParameter("VFAT0.root");
    myProducer.setIgnoredParameter("VFAT2.root");
    myProducer.setIgnoredParameter("VFAT3.root");
    myProducer.setIgnoredParameter("WithFuncGen");
    myProducer.setIgnoredParameter("Optimal");
    myProducer.setIgnoredParameter("values");

    cout<<"Make TTree for which detector? (1 = GE1/1-IV; 2 = GE1/1-IV-GIF; 3 = GE1/1-V)\n";
    cin>>iDetRequest;

    myProducer.setFitOption("NR");
    //myProducer.setHistoRebinFactor(2);
    myProducer.setVerboseModeIO(false);
    //myProducer.setVerboseModeIO(true);
    myProducer.setVerboseModePFN(false);
    //myProducer.setVerboseModePFN(true);
    myProducer.setVerboseModeLUT(false);
    //myProducer.setVerboseModeLUT(true);
    myProducer.setVerboseModePrintRuns(true);
    
    if(iDetRequest == 1){
    	myProducer.setFilesData(inputData_GE11_IV);
    	myProducer.writeTree(treeName_GE11_IV, outputData_GE11_IV);
    }
    else if(iDetRequest == 2){
    	myProducer.setFilesData(inputData_GE11_IV_GIF);
    	myProducer.writeTree(treeName_GE11_IV_GIF, outputData_GE11_IV_GIF);
    }
    else if(iDetRequest == 3){
    	myProducer.setFilesData(inputData_GE11_V);
    	myProducer.writeTree(treeName_GE11_V, outputData_GE11_V);
    }
    else{
       cout<<"Requested Input Not Understood; Exiting!!!\n";
    }
    
    return 0;
} //End main()
