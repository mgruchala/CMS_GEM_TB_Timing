//
//  treeAnalyzer.h
//  
//
//  Created by Brian L Dorney on 30/03/15.
//
//

#ifndef ____treeAnalyzer__
#define ____treeAnalyzer__

//C++ Includes
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <istream>
#include <limits>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

//My Includes

//ROOT Includes
//#include "Rtypes.h"     //Color
#include "TAxis.h"
#include "TCanvas.h"
#include "TEntryList.h"
#include "TFile.h"
//#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TROOT.h"
#include "TTree.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;

using namespace ROOT;

class treeAnalyzer {
    
public:
    
    //Constructor======================================
    treeAnalyzer();
    
    //Data Members=====================================
    struct PlotInfo{
        bool bPointLabels;      //Add labels next to each point data point
        
        bool bXAxis_UserRange;  //Use Range Specified by User
        bool bYAxis_UserRange;  //Use Range Specified by User
        
        int iColor;             //Line and Marker Color
        
        int iStyleLine;         //Line Style
        int iStyleMarker;       //Marker Style
        
        float fPtLbls_Angle;    //Angle at which point labels should drawn at
        
        float fSizeLine;          //Line Width
        float fSizeMarker;        //Marker Size
        
        float fXAxis_Min;       //Min X-Axis Value
        float fXAxis_Max;       //Max X-Axis Value
        
        float fYAxis_Min;       //Min Y-Axis Value
        float fYAxis_Max;       //Max Y-Axis Value
        
        string strDirectory;    //Name of the TDirectory data is stored in (CanvasInfo.strDirectory -> this directory)
        
        string strExpress;      //Expression that makes the plot
        
        string strFitFormula;   //Fit Formula
        
        string strFriendBranch; //Branch name of a complex object that relates to the TBranch described in strVarDepend (e.g. if strVarDepend is a float taken from a fit, strFriendBranch is the branch that stores the TF1 this came from)
        
        string strLegEntry;     //Legend Entry
        
        string strName;         //TName of TGraph
        string strNameROOTFile; //Root file strNameTree is found in
        string strNameTree;     //Tree TGraph named strName should be created from
        
        string strOptionDraw;   //Draw Option
        string strOptionFit;    //Fit Option
        
        string strPtLbls_Brnch; //Branch name of an alpha numeric data type that is to be added to plot as point labels
        string strPtLbls_Type;  //Data type for the point labels; supported "INT, FLOAT, STRING"
        
        string strSelLocal;     //Local selection, Plot is made with && of Master and Local Selection
        
        string strVarDepend;    //TBranch the dependent variable is from
        string strVarDependErr; //TBranch the error on the dependent variable is from
        
        string strVarIndep;     //TBranch the independent variable is from
        string strVarIndepErr;  //TBranch the error on the indepdendent variable is from
        
        string strXAxis_Title;  //X-Axis Title
        
        string strYAxis_Title;  //Y-Axis Title
        
        //Specify default behavior
        PlotInfo(){
            bPointLabels        = false;
            
            bXAxis_UserRange    = false;
            bYAxis_UserRange    = false;
            
            fPtLbls_Angle       = 0.;
        }
    }; //End PlotInfo
    
    struct CanvasInfo{
        bool bLog_X, bLog_Y;//, bLog_Z;         //Logarithmic Axis
        bool bGrid_X, bGrid_Y;//, bGrid_Z;      //Grid
        
        int iSize_X, iSize_Y;                   //Canvas Size
        
        float fLegNDCPos_X1, fLegNDCPos_X2;     //NDC X-Position of Legend
        float fLegNDCPos_Y1, fLegNDCPos_Y2;     //NDC Y-Position of Legend
        
        std::vector<float> vec_fLatexNDCPos_X;  //NDC X-Position of TLatex
        std::vector<float> vec_fLatexNDCPos_Y;  //NDC Y-Position of TLatex

        string strDirectory;                    //Name of the TDirectory data is stored in (SelInfo.strDirectory -> this directory)
        
        string strName;                         //TName
        
        string strTitle;                        //Canvas Title
        
        std::vector<string> vec_strLatexLine;   //String Container
        
        std::map<string, PlotInfo> mapPlot;
        std::map<string, std::vector<std::tuple<float,float,std::string> > > mapPlotLabels;
        
        TLegend *leg;
        
        TMultiGraph *mGraph;
        
        //Specify default behavior
        CanvasInfo(){
            bLog_X=bLog_Y=false;
            bGrid_X=bGrid_Y=false;
            
            //iSize_X=iSize_Y=700;
            
            //fLegNDCPos_X1=fLegNDCPos_X2=0;
            //fLegNDCPos_Y1=fLegNDCPos_Y2=0;
            
            //strName="";
            //strTitle="";
        }
    }; //End CanvasInfo
    
    struct SelInfo{
        string strDirectory;    //Name of the TDirectory data is stored in (top-level)
        
        string strIdent;
        
        string strSel;
        
        std::map<string, CanvasInfo> mapCanvas;
        
        //Specify default behavior
        //SelInfo(){strIdent="";strSel="";}
    }; //End SelInfo
    
    //Getters==========================================
    
    //Setters==========================================
    //Sets the selection
    virtual void setSelection(string strInputTextFile);
    
    //Sets verbose printing - File Input/Output
    virtual void setVerboseIO(bool bInputMode){bVerbose_IO = bInputMode; return;};
    
    //Actors===========================================
    //A set of plots is created for each master selection.  Each plot within this may have a local selection
    virtual void applySelection(string strInputTextFile);
    
//private:
protected:
    //Data Members=====================================
    bool bVerbose_IO;
    
    string strSecBegin_Sel;
    string strSecBegin_Canvas;
    string strSecBegin_Plot;
    
    string strSecEnd_Sel;
    string strSecEnd_Canvas;
    string strSecEnd_Plot;
    
    union UserInputFormat{
        int iInput;
        float fInput;
        string strInput;
        
        //Constructor
        UserInputFormat(){};
        
        //Deconstructor
        ~UserInputFormat(){};
    };
    
    //Master selection
    //  vecSelGlobal[i].first -> Identifier
    //  vecSelGlobal[i].second -> Selection
    //std::vector<std::pair<std::string,std::string> > vecSelGlobal;
    std::vector<SelInfo> vecSelInfo;
    
    //Actors===========================================
    
    //Getters==========================================
    std::istream & getlineNoSpaces(std::istream & stream, string & str);
    
    virtual float getOffsetPosition(float fPos, float fAngle, bool bXCord);
    virtual float getPosAngle(float fAngle){return (fAngle > 0.) ? fAngle : getPosAngle(fAngle+360.); };
    
    //Gets a Parsed line of the selection file
    //Expects an input of the form "<field name>=<field value>;"
    //  ret_Pair.first -> field name
    //  ret_Pair.second -> field value
    virtual std::pair<string,string> getParsedLine(string strInputLine, bool &bExitSuccess);
    
    //Gets a vector of ordered pairs (x_pos,y_pos,label) passing selection for a given input PlotInfo,
    virtual std::vector<std::tuple<float,float,string> > getSelOrderedPairs(PlotInfo &inputInfo, TTree *treeInput);
    
    //Given an input object returns a string (using stringstream)
    //Note this will probably only work for standard numeric containers in C++
    //  i.e. int, long, float, double, etc...
    template<class T>
    string getString(T input);
    
    virtual TCanvas * getCanvas(CanvasInfo inputInfo);
    virtual TFile * getFile(string strInputFile, string strInputOption, bool &bExitSuccess);
    virtual TGraphErrors * getGraph(PlotInfo inputInfo, TGraphErrors *inputGraph);
    virtual TH1F * getHistogram(PlotInfo inputInfo, TH1F *hInput);
    virtual TMultiGraph * getMultiGraph(CanvasInfo inputInfo);
    //TTree * getTTree(PlotInfo inputInfo);
    
    //Misc=============================================
    virtual bool convert2bool(string str, bool &bExitSuccess);
    virtual int convert2Color(string str);
    
    //Printers=========================================
    virtual void printStreamStatus(ifstream &inputStream);
    
    //Setters==========================================
    virtual void setParamCanvas(string strField, string strVal, CanvasInfo &inputInfo);
    virtual void setParamPlot(string strField, string strVal, PlotInfo &inputInfo);
    virtual void setParamSel(string strField, string strVal, SelInfo &inputInfo);
    
    //virtual void setStyleCanvas(TCanvas &inputCanvas);
    //virtual void setStyleGraph(TGraphErrors &inputGraph);
};

//template method must be defined in the header file
template<class T>
string treeAnalyzer::getString(T input){
    std::stringstream sstream;
    sstream<<input;
    return sstream.str();
} //End treeAnalyzer::getString()

#endif /* defined(____treeAnalyzer__) */
