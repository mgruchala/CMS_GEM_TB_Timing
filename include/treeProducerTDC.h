//
//  treeProducerTDC.h
//  
//
//  Created by Brian L Dorney on 15/01/15.
//
//

#ifndef ____treeProducerTDC__
#define ____treeProducerTDC__

//C++ Includes
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
//#include <tuple>
#include <utility>
#include <vector>

//My Includes
#include "TRunParameters.h"
//#include "TRunParameters.cpp"

//ROOT Includes
#include "TBranch.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TROOT.h"
#include "TSpectrum.h"
#include "TTree.h"

using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::list;
using std::map;
using std::string;
//using std::tuple;
using std::vector;

using namespace ROOT;

//Create a container like std::tuple, but 4 elements
template<typename T1, typename T2, typename T3, typename T4>
struct quad{
    T1 first;
    T2 second;
    T3 third;
    T4 fourth;
};

template<typename T1, typename T2, typename T3, typename T4>
quad<T1,T2,T3,T4> make_quad(const T1 &m1, const T2 &m2, const T3 &m3, const T4 &m4){
    quad<T1,T2,T3,T4> quadInstance;
    quadInstance.first = m1;
    quadInstance.second = m2;
    quadInstance.third = m3;
    quadInstance.fourth = m4;
    
    return quadInstance;
}

class treeProducerTDC {
public:
    //Constructor
    treeProducerTDC();
    
    //Getters - Methods that Get (i.e. Return) Something
    //------------------------------------------------------------------------------------------------------------------------------------------
    
    //Printers - Methods that Print Something
    //------------------------------------------------------------------------------------------------------------------------------------------
    virtual void printStoredData(TTree *inputTree);
    
    //Setters - Methods that Set Something
    //------------------------------------------------------------------------------------------------------------------------------------------
    //Set data File
    virtual void setFilesData(string inputFileName){fileName_Data = inputFileName; return;};
    //virtual void setFilesLUT(string inputFileName){ fileName_LUT = inputFileName; return;};
    
    //virtual void setFitFormula(string inputOption){ fitFormula = inputOption; return;};
    virtual void setFitOption(string inputOption){fitOption = inputOption; return;};
    
    virtual void setNumericDeconvoModel(string inputFormula, vector<float> inputParameters);
    
    virtual void setIgnoredParameter(string inputParameter);
    
    virtual void setHistoRebinFactor(int inputFactor){
        iRebinFactor = inputFactor;
        return;
    };
    
    //Set Verbose Print Modes
    virtual void setVerboseModeIO(bool inputMode){verbose_IO = inputMode; return;};
    virtual void setVerboseModeLUT(bool inputMode){verbose_LUT = inputMode; return;};
    virtual void setVerboseModePFN(bool inputMode){verbose_PFN = inputMode; return;};
    virtual void setVerboseModePkCalc(bool inputMode){verbose_PkCalc = inputMode; return;};
    virtual void setVerboseModePrintRuns(bool inputMode){verbose_PrintRuns = inputMode; return;};
    //virtual void setVerboseModeMappedData(bool inputMode){verbose_MappedData = inputMode; return;};
    
    //Write To File (I/O)
    //------------------------------------------------------------------------------------------------------------------------------------------
    virtual void writeTree(string inputTreeName, string outputDataFile);
    
    //Miscillaneous Methods
    //------------------------------------------------------------------------------------------------------------------------------------------
    virtual void clearIgnoredParameters(){ vecIgnoredParam.clear(); return; };
    
private:
    //Data Members
    bool verbose_IO;            //Output IO Related Statements
    bool verbose_LUT;           //Output Parsed Look-up-table
    bool verbose_PFN;           //Output Parsed File Name
    bool verbose_PkCalc;        //Outputs Peak Integral Iterative Calculations During getPeakIntegral()
    bool verbose_PrintRuns;     //Outputs a Table of all stored run information at the end of analysis
    //bool verbose_MappedData;    //Output Stored Mapped Data
    
    int iRebinFactor;
    
    string fitOption;
    //string fitFormula;
    
    string fileName_Data;   //List of Root Files
    string fileName_LUT;    //Look Up File
    string fileName_ROOT;   //Specific root file found in fileName_Data;
    
    string secName_AUTO;
    string secName_DET;
    string secName_END;
    string secName_MAN;
    
    TF1 *func_NumericDeconvoModel;
    
    TH1F timingHisto;
    
    //Run run;
    //TRunParameters runLogger;
    
    vector<string> vecIgnoredParam; //Vector of Parameters to be ignored during getParsedFileName()
    
    enum dataType_t {
        typeBool=0,
        typeFloat,
        typeInt,
        typeString
    };
    
    //Methods
    
    //Getters - Methods that Get (i.e. Return) Something
    //------------------------------------------------------------------------------------------------------------------------------------------
    float getPeakIntegral(TH1F *hInputHisto, float fInputPeakPos_X);
    
    list<string> getParsedFileName(string inputFileName);
    
    //TH1F * getHistogram(string inputFileName, int chanNum);
    
    vector<quad<string,vector<string>,dataType_t,int> > getLookUpTable(string inputFileName, TRunParameters &runLogger);
    
    //Given an input object returns a string (using stringstream)
    //Note this will probably only work for standard numeric containers in C++
    //  i.e. int, long, float, double, etc...
    template<class T>
    string getString(T input);
    
    //Printers - Methods that Print Something
    //------------------------------------------------------------------------------------------------------------------------------------------
    virtual void printStreamStatus(ifstream &inputStream);
    
    //Setters - Methods that Set Something
    //------------------------------------------------------------------------------------------------------------------------------------------
    virtual void setHistogram(string inputFileName, TH1F &inputHisto, int chanNum, bool &bExitFlag);
    
    virtual void setMappedParam(string &parsedInput, quad<string,vector<string>,dataType_t,int> &lutItem, TRunParameters &runLogger);
    virtual void setMappedParam(list<string> &parsedFileNames, vector<quad<string,vector<string>,dataType_t,int> > &lookUpTable, TRunParameters &runLogger);
    
    virtual void setParsedLUTLine(string &inputLine, vector<string> &vec_strLUTIdents, string &strTreeName, string &strDataType, int &iMthdIdx, bool &bExitFlag);
    
    virtual void setRun(string inputROOTFileName, string inputLUTFileName, TRunParameters &runLogger);
    
}; //End Class Definition treeProducerTDC

//template method must be defined in the header file
template<class T>
string treeProducerTDC::getString(T input){
    std::stringstream sstream;
    sstream<<input;
    return sstream.str();
} //End treeProducerTDC::getString()

#endif /* defined(____treeProducerTDC__) */
